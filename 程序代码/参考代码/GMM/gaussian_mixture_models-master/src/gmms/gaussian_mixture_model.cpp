//#########################################################//
//#                                                       #//
//# gaussian_mixture_models  gaussian_mixture_model.cpp   #//
//# Roberto Capobianco  <capobianco@dis.uniroma1.it>      #//
//#                                                       #//
//#########################################################//


#include <omp.h>

#include <cmath>
#include <ctime>
#include <iostream>

#include "gaussian_mixture_model.h"

namespace gmms {
  void GaussianMixtureModel::initialize(const Eigen::MatrixXd &dataset) {
    if (num_components_ == 0) {
      throw std::runtime_error(zero_components());
    }

    int dataset_size = dataset.rows();

    KMeans km(num_components_);
    km.cluster(dataset);

    const std::vector<Eigen::VectorXd> &means = km.means();
    const std::vector<Eigen::MatrixXd> &covariances = km.covariances();
    const std::vector<int> &cluster_cardinalities = km.clusterCardinalities();

    for(int i = 0; i < num_components_; ++i) {
      gaussian_vec_[i].setMeanCovariance(means[i], covariances[i]);
      prior_vec_[i] = double(cluster_cardinalities[i]) / double(dataset_size);
    }

    initialized_ = true;
  }

  double GaussianMixtureModel::logLikelihood(const Eigen::MatrixXd &dataset) {
    if (!initialized_) {
      throw std::runtime_error(notinitialized_());
    }

    double log_likelihood = 0.0;
    int dataset_size = dataset.rows();

#pragma omp parallel for reduction (+:log_likelihood)   
    for(int i = 0; i < dataset_size; ++i) {
      log_likelihood += std::log(probability_density_function(dataset.row(i).transpose()));
    }
    
    return log_likelihood;
  }

  void GaussianMixtureModel::expectationMaximization(const Eigen::MatrixXd &dataset) {
    if (!initialized_) {
      throw std::runtime_error(notinitialized_());
    }

    double old_log_likelihood = logLikelihood(dataset);
    double log_likelihood = 0.0;
    double delta = 0.0;
    int iteration = 0;

    do {
      expectation(dataset);
      maximization(dataset);
      old_log_likelihood = log_likelihood;
      log_likelihood = logLikelihood(dataset);
      delta = log_likelihood - old_log_likelihood;
      ++iteration;
    }
    while(std::abs(delta) > delta_ && iteration < num_iterations_);
  }

  void GaussianMixtureModel::expectation(const Eigen::MatrixXd &dataset) {
    int dataset_size = dataset.rows();

    expectations_.setZero(dataset_size, num_components_);

#pragma omp parallel for
    for(int i = 0; i < dataset_size; ++i) {
      double normalization_factor = 0.f;

      for(int j = 0; j < num_components_; ++j) {
	expectations_(i, j) = prior_vec_[j] * gaussian_vec_[j].evaluate_point(dataset.row(i));

	if (i == dataset_size - 1 && expectations_.col(j).sum() == 0) {
	  expectations_(i, j) = M_EPS;
	}
      }

      double expectations_sum = expectations_.row(i).sum();

      if (expectations_sum != 0) {
	normalization_factor = std::min(1 / expectations_sum, 1e15);
	expectations_.row(i) = normalization_factor * expectations_.row(i);
      }
    }
  }

  void GaussianMixtureModel::maximization(const Eigen::MatrixXd &dataset) {
    int dataset_size = dataset.rows();
    double expectation_sum = 0.f;

    for(int i = 0; i < num_components_; ++i) {
      prior_vec_[i] = expectations_.col(i).sum();
      double inverse_cluster_expectation = 1.f / prior_vec_[i];

      Eigen::VectorXd mean = inverse_cluster_expectation * expectations_.col(i).transpose() * dataset;
      Eigen::MatrixXd covariance = Eigen::MatrixXd::Zero(dataset.cols(), dataset.cols());
      std::vector<Eigen::MatrixXd> thread_covariance;

      omp_lock_t lck;
      omp_init_lock(&lck);
#pragma omp parallel
      {
	int num_threads = omp_get_max_threads();
	int thread_dataset_size = floor(double(dataset_size)/double(num_threads));
    	int thread_id = omp_get_thread_num();
	int thread_j_min = thread_dataset_size*thread_id;
	int thread_j_max;

	Eigen::MatrixXd t_covariance = Eigen::MatrixXd::Zero(dataset.cols(), dataset.cols());;
	
	if (thread_id == num_threads - 1) {
	  thread_j_max = dataset_size;
	}
	else {
	  thread_j_max = thread_j_min + thread_dataset_size;
	}

	for(int j = thread_j_min; j < thread_j_max; ++j) {
	  Eigen::VectorXd diff = dataset.row(j).transpose() - mean;
	  t_covariance += expectations_(j, i) * diff * diff.transpose();
	}

	omp_set_lock(&lck);
	thread_covariance.push_back(t_covariance);
	omp_unset_lock(&lck);
      }

      omp_destroy_lock(&lck);

      for (int t = 0; t < thread_covariance.size(); ++t) {
	covariance += thread_covariance.at(t);
      }

      covariance = inverse_cluster_expectation * covariance;

      gaussian_vec_[i].setMeanCovariance(mean, covariance);
      prior_vec_[i] = prior_vec_[i] / double(dataset_size);
    }
  }

  double GaussianMixtureModel::bayesianInformationCriterion(const Eigen::MatrixXd &dataset) {
    int dataset_size = dataset.rows();
    int point_dimensionality = dataset.cols();
    double free_parameters = (num_components_ - 1) * num_components_ * 
      (point_dimensionality + 0.5 * point_dimensionality * (point_dimensionality + 1));

    double bic = -logLikelihood(dataset) + 0.5 * free_parameters * std::log(dataset_size);

    return bic;
  }

  void GaussianMixtureModel::load(Eigen::MatrixXd model) {
    int idx = 0;
    // covariance rows plus one mean and prior
    int dimensionality = model.cols();
    int size = dimensionality + 2;
    int num_components = model.rows() / size;
    setNumComponents(num_components);

    for (int i = 0; i < num_components; ++i) {
      double prior = model(idx, 0);
      Eigen::VectorXd mean = model.row(idx + 1);
      Eigen::MatrixXd covariance = model.block(idx + 2, 0, dimensionality, dimensionality);

      gaussian_vec_[i].setMeanCovariance(mean, covariance);
      prior_vec_[i] = prior;

      idx += size;
    }

    initialized_ = true;
  }

  Eigen::MatrixXd GaussianMixtureModel::save() {
    if (!initialized_) {
      throw std::runtime_error(notinitialized_());
    }

    int dimensionality = gaussian_vec_[0].dimensionality();
    // num_components covariances, means and priors
    int model_rows = num_components_*dimensionality + 2*num_components_;
    Eigen::MatrixXd model = Eigen::MatrixXd::Zero(model_rows, dimensionality);

    int idx = 0;
    // covariance rows plus one mean and prior
    int size = dimensionality + 2;

    for (int i = 0; i < num_components_; ++i) {
      Eigen::VectorXd mean = gaussian_vec_[i].mean();
      Eigen::MatrixXd covariance = gaussian_vec_[i].covariance();
      model(idx, 0) = prior_vec_[i];

      model.block(idx + 1, 0, 1, dimensionality) = mean.transpose();
      model.block(idx + 2, 0, dimensionality, dimensionality) = covariance;
      idx += size;
    }

    return model;
  }
} // namespace gmms
