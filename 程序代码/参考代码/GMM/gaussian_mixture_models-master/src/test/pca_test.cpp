#include <pca.h>
#include <matrix_io.h>

#include <iostream>

int main(int argc, char** argv) {
  int num_components = 2;

  std::string filename = "pca_data.dat";
  std::string output = "out.dat";

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " data_filename num_components[OPT, default: 2] ";
    std::cerr << "output_file[OPT, default: ./out.dat]" << std::endl; 
    return 1;
  }

  filename = std::string(argv[1]);

  if (argc >= 3) {
    num_components = atoi(argv[2]);
  }

  if (argc >= 4) {
    output = argv[3];
  }

  Eigen::MatrixXd dataset;
  MatrixIO mio;

  mio.readFromFile(filename, dataset);

  double retained_variance = 0.0;

  std::cout << "Number of chosen components: " << num_components << std::endl;

  gmms::PCA pca(num_components);
  Eigen::MatrixXd reduced_dataset = pca.pca(dataset, retained_variance);

  std::cout << "Retained variance: " << retained_variance << std::endl << std::endl;

  std::cout << "Writing reduced dataset to " << output << std::endl;
  mio.writeToFile(output, reduced_dataset);
}

