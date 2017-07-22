#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>
#include <cmath>
#include <numeric>
#include <functional>
#include <algorithm>

using namespace boost::assign;

class Gaussian 
{
public:
    double weight;
    double mean;
    double stddev;
    
    Gaussian(double pMean, double pStddev, double pWeight):
        weight(pWeight), mean(pMean), stddev(pStddev) {}
    
    double normProbability(double point) const
    {
        return 1 / (sqrt(2*M_PI) * stddev) * (exp(-0.5*pow((point - mean) / stddev, 2)));
    }
};

typedef std::vector<Gaussian> param_vec_t;
typedef std::vector<std::vector<double> > aux_vec_t;
typedef std::vector<double> point_vec_t;

std::vector<double> readData(const char* filename)
{
    using boost::lexical_cast;
    std::vector<double> d;
    std::ifstream inp(filename, std::ios::in);
    std::string line;
    while(!inp.eof()) {
        std::getline(inp, line);
        if(line.length() == 0) {
            break;
        }
        d.push_back(lexical_cast<double>(line));
    }
    return d;
}

struct GetAux: public std::unary_function<double, double>
{
    const Gaussian& g;
    GetAux(const Gaussian& pG): g(pG) {}
    
    double operator()(double point) const
    {
        return g.weight * g.normProbability(point);
    }
};


void computeAuxiliary(aux_vec_t& aux, std::vector<double>& sums, 
                      const point_vec_t& points, const param_vec_t& params) 
{
    std::vector<double>& ysums = aux.back();
    std::fill(ysums.begin(), ysums.end(), 0);
    aux_vec_t::iterator ait = aux.begin();
    for(param_vec_t::const_iterator pit = params.begin();
        pit != params.end(); ++pit, ++ait) {
        std::transform(points.begin(), points.end(), ait->begin(), GetAux(*pit));
        std::transform(ait->begin(), ait->end(), ysums.begin(), 
                       ysums.begin(), std::plus<double>());
    }
    ait = aux.begin();
    for(int dist = 0; dist < params.size(); ++dist, ++ait) {
        std::transform(ait->begin(), ait->end(), ysums.begin(), ait->begin(), 
                       std::divides<double>());
        sums[dist] = std::accumulate(ait->begin(), ait->end(), 0.0);
    }
}

struct MulWithSqredDev: public std::binary_function<double, double, double>
{
    const double mean;
    MulWithSqredDev(double pMean): mean(pMean) {}
    
    double operator()(double point, double y) const
    {
        return pow(point - mean, 2) * y;
    }
};

void printParams(const param_vec_t& params) 
{
    for(param_vec_t::const_iterator it = params.begin(); it != params.end(); ++it) {
        std::cout << "(" << it->mean << "," << std::endl;
        std::cout << it->stddev << "," << std::endl;
        std::cout << it->weight << ")," << std::endl;
    }
}
     
void updateWeights(Gaussian& g, double sum, int numPoints) 
{
    g.weight = sum / numPoints;
}
   
void updateMeans(Gaussian& g, double sum, const std::vector<double>& ys, const point_vec_t& points) 
{
    g.mean = std::inner_product(points.begin(), points.end(), ys.begin(), 0.0) / sum;
}

void updateStddevs(Gaussian& g, double sum, const std::vector<double>& ys, const point_vec_t& points,
                 const Gaussian& oldG) 
{
    double sum_x_y = std::inner_product(points.begin(), points.end(),
                                        ys.begin(), 0.0, std::plus<double>(), 
                                        MulWithSqredDev(oldG.mean));
    g.stddev = sqrt(sum_x_y / sum);
}

param_vec_t em(const param_vec_t& initial, const point_vec_t& points, unsigned int maxIterations) 
{
    aux_vec_t aux;
    const unsigned int dataSets = initial.size();
    std::vector<double> sums(dataSets, 0);
    for(unsigned int i = 0; i < dataSets + 1; ++i) {
        aux.push_back(std::vector<double>());
        std::vector<double>& ys = aux.back();
        ys.resize(points.size());
    }
    param_vec_t last = initial;
    for(unsigned int iteration = 0; iteration < maxIterations; ++iteration) {
        std::fill(sums.begin(), sums.end(), 0.0);
        computeAuxiliary(aux, sums, points, last);
        
        param_vec_t::iterator pit = last.begin();
        param_vec_t::const_iterator old_pit = last.begin();
        std::vector<double>::const_iterator sit = sums.begin();
        aux_vec_t::const_iterator ait = aux.begin();
        for(; pit != last.end(); ++pit, ++sit, ++ait, ++old_pit) {
            updateStddevs(*pit, *sit, *ait, points, *old_pit);
            updateMeans(*pit, *sit, *ait, points);
            updateWeights(*pit, *sit, points.size());
        }
    }
    return last;
}


int main(int argc, char** argv) 
{
    const point_vec_t points = readData("points.dat");
    param_vec_t initialGuesses;
    initialGuesses += Gaussian(10, 20, 0.1), Gaussian(20, 40, 0.1), Gaussian(30, 60, 0.8);
    param_vec_t result = em(initialGuesses, points, 1000);
    printParams(result);
    return 0;
}
