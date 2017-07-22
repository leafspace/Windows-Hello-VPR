#include <k_means.h>
#include <pca.h>
#include <matrix_io.h>

#include <string>
#include <iostream>

int main(int argc, char** argv) {
  int num_clusters = 2;
  std::string filename = "k_means_data.dat";
  std::string output = "out.dat";

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " data_filename num_clusters[OPT, default: 2] ";
    std::cerr << "output_file[OPT, default: ./out.dat]" << std::endl; 
    return 1;
  }

  filename = std::string(argv[1]);

  if (argc >= 3) { 
   num_clusters = atoi(argv[2]);
  }

  if (argc >= 4) {
    output = argv[3];
  }

  std::cout << "Reading " << filename << std::endl;
  std::cout << "Number of chosen clusters: " << num_clusters << std::endl;

  gmms::KMeans km(num_clusters);

  Eigen::MatrixXd dataset;
  MatrixIO mio;

  mio.readFromFile(filename, dataset);

  std::cout << "Initial dataset" << std::endl << dataset << std::endl << std::endl;

  km.cluster(dataset);
  const std::vector<Eigen::VectorXd> &means = km.means();
  const std::vector<Eigen::MatrixXd> &covariances = km.covariances();
  const std::vector<int> &cluster_cardinalities = km.clusterCardinalities();
  const std::vector<int> &assignments = km.assignments();

  std::cout << "Assignments:" << std::endl;

  for (int i = 0; i < assignments.size(); ++i) {
    std::cout << dataset.row(i) << "\t" << "Cluster " << assignments[i] + 1 << std::endl;
  }

  std::cout << std::endl;

  for (int i = 0; i < means.size(); ++i) {
    std::cout << "Cluster " << i + 1 << std::endl;
    std::cout << "Mean: " << means[i].transpose() << std::endl;
    std::cout << "Covariance: " << std::endl << covariances[i] << std::endl;
    std::cout << "Cluster Cardinality: " << cluster_cardinalities[i] << std::endl;
    std::cout << std::endl;
  }

  std::cout << "Reducing dataset to 2 components for visualization purposes" << std::endl;
  gmms::PCA pca(2);
  double retained_variance = 0.0;
  Eigen::MatrixXd reduced_dataset = pca.pca(dataset, retained_variance);

  std::cout << "Retained variance: " << retained_variance << std::endl << std::endl;

  std::cout << "Writing assignments to " << output << std::endl;
  Eigen::MatrixXd output_matrix(reduced_dataset.rows(), reduced_dataset.cols() + 1);
  output_matrix.block(0, 0, reduced_dataset.rows(), reduced_dataset.cols()) = reduced_dataset;

  for (int i = 0; i < reduced_dataset.rows(); ++i) {
    output_matrix(i, reduced_dataset.cols()) = assignments[i];
  }

  mio.writeToFile(output, output_matrix);

  return 0;
}

