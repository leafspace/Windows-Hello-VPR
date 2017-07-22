#include <gmm_classifier.h>
#include <pca.h>
#include <matrix_io.h>

#include <iostream>

int main(int argc, char** argv) {
  gmms::GMMClassifier model;
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
  int num_variables = 3;

  gmms::PCA pca(num_variables);
  double retained_variance = 0.0;
  Eigen::MatrixXd reduced_dataset = pca.pca(dataset.leftCols(dataset.cols() - 1), retained_variance);

  std::cout << "Retained variance: " << retained_variance << std::endl << std::endl;

  Eigen::MatrixXd final_dataset(dataset.rows(), num_variables + 1);
  final_dataset.leftCols(num_variables) = reduced_dataset;
  final_dataset.rightCols(1) = dataset.rightCols(1);

  Eigen::MatrixXd test_dataset = final_dataset.topRows(test_rows);
  Eigen::MatrixXd train_dataset = final_dataset.bottomRows(dataset.rows() - test_rows);

  std::cout << "Splitting dataset: ";
  std::cout << (1 - test_percentage)*100 << "% (" << train_dataset.rows() << " rows) for training; ";
  std::cout << (test_percentage)*100 << "% (" << test_dataset.rows() << " rows) for testing";
  std::cout << std::endl;
  
  model.setMaxIterations(100);
  model.setDelta(1e-3);
  model.train(train_dataset);
  std::vector<int> assignments = model.predict(test_dataset.leftCols(test_dataset.cols() - 1));

  Eigen::VectorXi assigned_classes = Eigen::VectorXi::Map(assignments.data(), assignments.size());
  Eigen::VectorXi ground_truth = test_dataset.rightCols(1).cast<int>();
  int correct_answers = (assigned_classes.array() != ground_truth.array()).count();

  double error = double(correct_answers) / test_dataset.rows();

  std::cout << "Test error: " << error << std::endl;

  std::cout << "Saving model to " << modelname << std::endl;
  model.save(modelname);
  gmms::GMMClassifier model2;
  std::cout << "Loading model from " << modelname << " on a new object" << std::endl;
  model2.load(modelname);

  assignments = model2.predict(test_dataset.leftCols(test_dataset.cols() - 1));
  assigned_classes = Eigen::VectorXi::Map(assignments.data(), assignments.size());
  correct_answers = (assigned_classes.array() != ground_truth.array()).count();
  error = double(correct_answers) / test_dataset.rows();

  std::cout << "Test error: " << error << std::endl;

  std::cout << "Writing assignments to " << output << std::endl;
  Eigen::MatrixXd output_matrix(test_dataset.rows(), test_dataset.cols() + 1);
  output_matrix.block(0, 0, test_dataset.rows(), test_dataset.cols()) = test_dataset;

  for (int i = 0; i < test_dataset.rows(); ++i) {
    output_matrix(i, test_dataset.cols()) = assignments[i];
  }

  mio.writeToFile(output, output_matrix);
}

