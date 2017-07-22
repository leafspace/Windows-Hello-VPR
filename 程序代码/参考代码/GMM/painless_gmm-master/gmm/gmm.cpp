/*
The MIT License (MIT)

Copyright (c) 2016 Alvaro Collet

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Neither name of this software nor the names of its contributors may be used to 
endorse or promote products derived from this software without specific
prior written permission. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <cmath>
#include <random>
#include "gmm.h"
#include "em.h"
#include "math_utils.h"
#include "KMeans.h"
#include "gaussian.h"

// Local helper functions
namespace
{
    //----------------------------------------------------------------------------
    bool CompareWeights3D(const AC::GaussianDistribution3D::SP& g1, const AC::GaussianDistribution3D::SP& g2)
    {
        return g1->Weight() > g2->Weight();
    }

    //----------------------------------------------------------------------------
    void SortModes(std::vector<AC::GaussianDistribution3D::SP>& modes)
    {
        std::sort(modes.begin(), modes.end(), CompareWeights3D);
    }
}

//----------------------------------------------------------------------------
double AC::GMM::LogSumExp(const std::vector<double>& logValues1, const std::vector<double>& logValues2)
{
    _ASSERT(logValues1.size() == logValues2.size() && L"Vectors must have the same size");

    // Trivial case 
    if (logValues1.empty() || logValues2.empty())
        return 0;

    // Find maximum value in sum of vectors.
    double maxLogValue = -std::numeric_limits<double>::max();
    for (int i = 0; i < logValues1.size(); i++) {
        double logValue = logValues1[i] + logValues2[i];
        if (logValue > maxLogValue)
            maxLogValue = logValue;
    }

    double expsum = 0;
    for (int i = 0; i < logValues1.size(); i++)
        expsum += exp(logValues1[i] + logValues2[i] - maxLogValue);

    return maxLogValue + log(expsum);
}

//----------------------------------------------------------------------------
AC::GMM::GMM3D::GMM3D(int numModes)
    : m_tmpLogWeights(numModes)
    , m_tmpLogLikelihoods(numModes)
    , m_globalWeight(1.0)
{

    // Reserve memory for our temporary vectors (to avoid allocations while processing)
    m_modes.reserve(numModes);
    // Create the set of gaussians in our GMM
    for (int k = 0; k < numModes; ++k)
        m_modes.emplace_back(new GaussianDistribution3D());
}

//----------------------------------------------------------------------------
AC::GMM::GMM3D::GMM3D(const GMM3D& rhs)
    : m_tmpLogLikelihoods(rhs.m_tmpLogLikelihoods.begin(), rhs.m_tmpLogLikelihoods.end())
    , m_tmpLogWeights(rhs.m_tmpLogWeights.begin(), rhs.m_tmpLogWeights.end())
    , m_globalWeight(rhs.m_globalWeight)
{
    m_modes.reserve(rhs.m_modes.size());
    for (auto& mode : rhs.m_modes)
        m_modes.emplace_back(new GaussianDistribution3D(*mode));
}

//----------------------------------------------------------------------------
double AC::GMM::GMM3D::LogLikelihood(const Vec3& observation)
{
    for (int k = 0; k < m_modes.size(); ++k) {
        m_tmpLogLikelihoods[k] = Modes(k)->EvaluateLog(observation);
        m_tmpLogWeights[k] = Modes(k)->LogWeight();
    }

    // We have to use the LogExpSum trick to evaluate likelihoods to avoid underflows
    return LogSumExp(m_tmpLogLikelihoods, m_tmpLogWeights);
}

//----------------------------------------------------------------------------
double AC::GMM::GMM3D::Likelihood(const Vec3& observation)
{
    double likelihood = exp(LogLikelihood(observation)) * GlobalWeight();
    return IsFinite(likelihood) ? likelihood : 0;
}

//----------------------------------------------------------------------------
double AC::GMM::GMM3D::LogLikelihood(const std::vector<Vec3>& observations)
{
    double sum = 0;
    for (const auto& observation : observations)
        sum += LogLikelihood(observation);

    // If NaN or infinite, return minimum possible value for log (corresponding to 0 probability)
    return IsFinite(sum) ? sum : -std::numeric_limits<double>::max();
}

//----------------------------------------------------------------------------
double AC::GMM::GMM3D::LogResponsibility(const Vec3& observation, int idxMode)
{
    // output: p(k|x_n) = p(x_n|k)p(k)/sum_k(p(x_n|k))
    // We need to use logs to avoid underflow, so: log p(k|x_n) = log(x_n|k) + log(p(k)) - log(sum_k(p(x_n|k))
    return Modes(idxMode)->EvaluateLog(observation) + Modes(idxMode)->LogWeight() - LogLikelihood(observation);
}

//----------------------------------------------------------------------------
bool AC::GMM::GMM3D::Process(const std::vector<Vec3>& observations, int numKMeansRestarts, Vec3* scalingFactors, double EMTolerance, int maxIterations)
{
    // Compute K-Means to initialize GMM
    AC::KMeans3D kmeans((int)Modes().size());
    std::vector<int> assignments(observations.size());
    kmeans.Process(observations, numKMeansRestarts, assignments);

    // Initialize GMM from K-Means results
    for (int k = 0; k < kmeans.numCentroids(); ++k) {
        Modes(k)->Reinitialize(kmeans.Centroids(k), kmeans.AvgVariance(k));
        Modes(k)->setWeight(kmeans.CentroidAssignmentRatio(k));
    }

    // Use EM to optimize GMM
    AC::GMM::EM EMTraining((int)observations.size(), (int)Modes().size(), EMTolerance, maxIterations);
    bool success = EMTraining.Process(observations, *this);

    // Sort modes according to weight
    SortModes(Modes());

    // Scale gaussian means and covariances if necessary
    if (scalingFactors != nullptr)
        for (int k = 0; k < Modes().size(); ++k)
            Modes(k)->Rescale(*scalingFactors);

    // Prune bad modes
    RemoveBadModes(c_SafeMinWeight);

    // Set global weight based on the number of observations (to compare against other GMMs)
    SetGlobalWeight(log(observations.size()));
    return success;
}

//----------------------------------------------------------------------------
double AC::GMM::GMM3D::ClosestMode(const Vec3& observation, int& mode) const
{
    double bestLogProbability = -std::numeric_limits<double>::max();
    mode = INVALID_MODE;
    for (int k = 0; k < (int)Modes().size(); ++k)
    {
        double prob = Modes(k)->EvaluateLog(observation);
        if (prob > bestLogProbability) {
            mode = k;
            bestLogProbability = prob;
        }
    }
    return bestLogProbability;
}

//----------------------------------------------------------------------------
size_t AC::GMM::GMM3D::RemoveBadModes(double tolerance)
{
    // Erase any mode with weight under a certain tolerance
    Modes().erase(std::remove_if(
        Modes().begin(), Modes().end(),
        [&](mode_type m) { return m->Weight() <= tolerance; }),
        Modes().end());

    // If we erased any mode, need to resize and reset the temporary vectors.
    m_tmpLogLikelihoods.resize(Modes().size(), 0.0);
    m_tmpLogWeights.resize(Modes().size(), 0.0);

    return Modes().size();
}

//----------------------------------------------------------------------------
double AC::GMM::GMMLikelihoodRatio(GMM3D& gmm1, GMM3D& gmm2, const Vec3& observation)
{
    double fgLikelihood = gmm1.Likelihood(observation);
    double bgLikelihood = gmm2.Likelihood(observation);
    return fgLikelihood / (fgLikelihood + bgLikelihood);
}

