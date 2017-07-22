//#########################################################//
//#                                                       #//
//# gaussian_mixture_models  gaussian.cpp                 #//
//# Roberto Capobianco  <capobianco@dis.uniroma1.it>      #//
//#                                                       #//
//#########################################################//

#include <cmath>
#include <Eigen/Eigenvalues>

#include "gaussian.h"

namespace gmms {
  double Gaussian::evaluate_point(const Eigen::VectorXd& pt) const {
    if (pt.size() != dimensionality_) {
      throw std::runtime_error(dimensionality_mismatch());
    }

    Eigen::VectorXd dist = pt - mean_;

    double factor = std::sqrt(std::pow(2 * M_PI, dimensionality_) * cov_abs_determinant_);
    double exp = std::exp(-0.5 * dist.transpose() * inv_covariance_ * dist);

    double result = exp / factor;

    if (result == 0) {
      srand(time(NULL));
      double random = ((double) rand() / (RAND_MAX));
      result = random * 1e-15;
    }

    return result;
  }

  void Gaussian::setCovariance(const Eigen::MatrixXd& covariance) {
    if (covariance.rows() != covariance.cols() || covariance.rows() != dimensionality_) {
      throw std::runtime_error(dimensionality_mismatch());
    }

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(covariance.cols());
    solver.compute(covariance);

    Eigen::VectorXd eigenvalues = solver.eigenvalues();
    Eigen::MatrixXd eigenvectors = solver.eigenvectors();

    for (int i = 0; i < eigenvalues.rows(); ++i) {
      if (eigenvalues(i) < M_EPS) {
	eigenvalues(i) = M_EPS;
      }
    }

    covariance_ = eigenvectors * eigenvalues.asDiagonal() * eigenvectors.inverse();
    double abs_determinant = std::fabs(covariance_.determinant());

    inv_covariance_ = covariance_.inverse();
    cov_abs_determinant_ = abs_determinant;
  }

  void Gaussian::setMeanCovariance(const Eigen::VectorXd& mean, const Eigen::MatrixXd& covariance) {
    int dimensionality = mean.size();
    dimensionality_ = dimensionality;
    setMean(mean);
    setCovariance(covariance);
  }
} // namespace gmms
