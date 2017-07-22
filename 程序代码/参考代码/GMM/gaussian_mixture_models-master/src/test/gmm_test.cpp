#include <gaussian_mixture_model.h>
#include <pca.h>
#include <matrix_io.h>

#include <iostream>

int main(int argc, char** argv) {
  gmms::GaussianMixtureModel gmm;
  int num_mixtures = 2;
  std::string filename = "gmms_data.dat";
  std::string output = "out.dat";
  std::string model = "model.dat";

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " data_filename num_mixtures[OPT, default: 2] ";
    std::cerr << "output_file[OPT, default: ./out.dat]" << std::endl; 
    return 1;
  }

  filename = std::string(argv[1]);

  if (argc >= 3) {
    num_mixtures = atoi(argv[2]);
  }

  if (argc >= 4) {
    output = argv[3];
  }

  Eigen::MatrixXd dataset;
  MatrixIO mio;

  mio.readFromFile(filename, dataset);

  gmm.setNumComponents(num_mixtures);
  gmm.initialize(dataset);
  gmm.setNumIterations(100);
  gmm.setDelta(1e-3);

  std::cout << "Computing GMMs with " << gmm.numComponents() << " components" << std::endl;
  std::cout << "Running EM for " << gmm.numIterations() << " iterations (delta: " << gmm.delta() << ")" << std::endl;
  gmm.expectationMaximization(dataset);
  std::vector<double> likelihood(dataset.rows());

  for (int i = 0; i < likelihood.size(); ++i) {
    likelihood[i] = gmm.logLikelihood(dataset.row(i));
  }

  std::cout << std::endl;
  std::cout << "Saving model to " << model << std::endl;
  gmm.save(model);
  std::cout << "Loading model from " << model << std::endl;
  gmm.load(model);
  std::string model_check = std::string("check") + model;
  std::cout << "Saving model to " << model << std::endl;
  gmm.save(model_check);

  std::cout << "BIC: " << gmm.bayesianInformationCriterion(dataset) << std::endl;
  std::cout << "Reducing dataset to 2 components for visualization purposes" << std::endl;
  gmms::PCA pca(2);
  double retained_variance = 0.0;
  Eigen::MatrixXd reduced_dataset = pca.pca(dataset, retained_variance);

  std::cout << "Retained variance: " << retained_variance << std::endl << std::endl;

  std::cout << "Writing log likelihoods to " << output << std::endl;
  Eigen::MatrixXd output_matrix(reduced_dataset.rows(), reduced_dataset.cols() + 1);
  output_matrix.block(0, 0, reduced_dataset.rows(), reduced_dataset.cols()) = reduced_dataset;

  for (int i = 0; i < reduced_dataset.rows(); ++i) {
    output_matrix(i, reduced_dataset.cols()) = likelihood[i];
  }

  mio.writeToFile(output, output_matrix);
}

