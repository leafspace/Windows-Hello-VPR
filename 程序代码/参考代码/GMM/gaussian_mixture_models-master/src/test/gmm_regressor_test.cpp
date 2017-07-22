#include <gmm_regressor.h>
#include <pca.h>
#include <matrix_io.h>

#include <iostream>

int main(int argc, char** argv) {
  gmms::GMMRegressor model;
  std::string filename = "classification_data.dat";
  std::string output = "out.dat";
  std::string modelname = "model.dat";

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " data_filename ";
    std::cerr << "output_file[OPT, default: ./out.dat]" << std::endl; 
    return 1;
  }

  filename = std::string(argv[1]);

  if (argc >= 3) {
    output = argv[2];
  }

  Eigen::MatrixXd dataset;
  MatrixIO mio;

  double test_percentage = 0.15;
  mio.readFromFile(filename, dataset);

  Eigen::VectorXi indices = Eigen::VectorXi::LinSpaced(dataset.rows(), 0, dataset.rows());
  srand(time(NULL));
  std::random_shuffle(indices.data(), indices.data() + dataset.rows());
  dataset = indices.asPermutation() * dataset;

  int test_rows = test_percentage * dataset.rows();
  Eigen::MatrixXd test_dataset = dataset.topRows(test_rows).leftCols(dataset.cols());
  Eigen::MatrixXd train_dataset = dataset.bottomRows(dataset.rows() - test_rows).leftCols(dataset.cols());

  std::cout << "Splitting dataset: ";
  std::cout << (1 - test_percentage)*100 << "% (" << train_dataset.rows() << " rows) for training; ";
  std::cout << (test_percentage)*100 << "% (" << test_dataset.rows() << " rows) for testing";
  std::cout << std::endl;
  
  model.setMaxIterations(100);
  model.setDelta(1e-4);
  model.train(train_dataset, true, 5);

  Eigen::MatrixXd regressions = model.predict(test_dataset.leftCols(test_dataset.cols() - 1));
  Eigen::MatrixXd ground_truth = test_dataset.rightCols(1);
  
  Eigen::MatrixXd regression_error = (regressions - ground_truth).rowwise().squaredNorm();
  double error = regression_error.mean();
  std::cout << "Test power: " << ground_truth.rowwise().squaredNorm().mean() << std::endl;
  std::cout << "Test MSE: " << error << std::endl;

  std::cout << "Saving model to " << modelname << std::endl;
  model.save(modelname);

  gmms::GMMRegressor model2;
  std::cout << "Loading model from " << modelname << " on a new object" << std::endl;
  model2.load(modelname);

  regressions = model2.predict(test_dataset.leftCols(test_dataset.cols() - 1));
  
  regression_error = (regressions - ground_truth).rowwise().squaredNorm();
  error = regression_error.mean();
  std::cout << "Test MSE: " << error << std::endl;

  gmms::PCA pca(2);
  double retained_variance = 0.0;
  Eigen::MatrixXd reduced_dataset = pca.pca(test_dataset, retained_variance);

  std::cout << "Retained variance: " << retained_variance << std::endl << std::endl;

  std::cout << "Writing assignments to " << output << std::endl;
  Eigen::MatrixXd output_matrix(reduced_dataset.rows(), reduced_dataset.cols() + 1);
  output_matrix.block(0, 0, reduced_dataset.rows(), reduced_dataset.cols()) = reduced_dataset;

  for (int i = 0; i < reduced_dataset.rows(); ++i) {
    output_matrix(i, reduced_dataset.cols()) = regressions(i);
  }

  mio.writeToFile(output, output_matrix);
}

