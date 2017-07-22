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
#include "gmm.h"
#include "em.h"

//----------------------------------------------------------------------------
AC::GMM::EM::EM(int numObservations, int numModes, double tolerance, int maxIterations) 
    : m_numTrainingPoints(numObservations)
    , m_tolerance(tolerance)
    , m_maxIterations(maxIterations)
{
    // Reserve memory for our temporary vectors (to avoid allocations while processing)
    m_tmpResponsibilities.reserve(numModes);
    for (int k = 0; k < numModes; ++k)
        m_tmpResponsibilities.emplace_back(std::vector<double>(numObservations));
}

//----------------------------------------------------------------------------
void AC::GMM::EM::UpdateResponsibilities(const std::vector<Vec3>& observations, GMM3D& gmm)
{
    _ASSERT(observations.size() == m_numTrainingPoints && m_numTrainingPoints >= gmm.Modes().size() && "Invalid number of observations.");
    int numObservations = (int)observations.size();
    int numModes = (int)gmm.Modes().size();
    for (int o = 0; o < numObservations; ++o) {
        for (int idxMode = 0; idxMode < numModes; ++idxMode) {
            double responsibility = exp(gmm.LogResponsibility(observations[o], idxMode));
            m_tmpResponsibilities[idxMode][o] = IsFinite(responsibility) ? responsibility : 0;
        }
    }
}

//----------------------------------------------------------------------------
void AC::GMM::EM::UpdateWeights(GMM3D& gmm)
{
    _ASSERT(m_numTrainingPoints >= gmm.Modes().size() && "Invalid number of observations.");
    int numModes = (int)gmm.Modes().size();
    for (int k = 0; k < numModes; ++k) {
        AC::OnlineMean<double> weight;
        for (int o = 0; o < m_numTrainingPoints; ++o)
            weight.Push(m_tmpResponsibilities[k][o]);
        gmm.Modes(k)->setWeight(std::max(weight.Mean(), c_SafeMinWeight)); // sum_n(p(k|x_n))/N
    }
}

//----------------------------------------------------------------------------
void AC::GMM::EM::UpdateMeans(const std::vector<Vec3>& observations, GMM3D& gmm)
{
    _ASSERT(observations.size() == m_numTrainingPoints && m_numTrainingPoints >= gmm.Modes().size() && "Invalid number of observations.");
    int numModes = (int)gmm.Modes().size();
    AC::OnlineMean<Vec3> sumMean;
    for (int k = 0; k < numModes; ++k) {
        for (int o = 0; o < m_numTrainingPoints; ++o)
            sumMean.Push(observations[o] * m_tmpResponsibilities[k][o]); // sum_n(p(k|x_n)*x_n)/N
                                                                         // In UpdateWeights, we calculate sum_n( p(k|x_n) )/N, so the /N in sumMean cancels this one, and we get:
        gmm.Modes(k)->setMean(sumMean.Mean() / std::max(gmm.Modes(k)->Weight(), c_SafeMinWeight)); // sum_n(p(k|x_n)*x_n)/sum_n(p(k|x_n)) 
        sumMean.Reset(); // Reset for the next use
    }
}

//----------------------------------------------------------------------------
void AC::GMM::EM::UpdateCovariances(const std::vector<Vec3>& observations, GMM3D& gmm)
{
    _ASSERT(observations.size() == m_numTrainingPoints && m_numTrainingPoints >= gmm.Modes().size() && "Invalid number of observations.");
    int numModes = (int)gmm.Modes().size();
    Vec3 centeredObservation;
    Mat3 cov;
    Mat3 centeredObservationOuterProd;
    for (int k = 0; k < numModes; ++k) {
        cov = Mat3::Zero();
        for (int o = 0; o < m_numTrainingPoints; ++o) {
            centeredObservation = observations[o] - gmm.Modes(k)->Mean();
            centeredObservationOuterProd = centeredObservation * centeredObservation.transpose();
            cov += m_tmpResponsibilities[k][o] * centeredObservationOuterProd;
        }
        // In UpdateWeights, we calculate sum_n( p(k|x_n) )/N, and we need sum_n( p(k|x_n) ) in the denominator below
        cov /= m_numTrainingPoints * std::max(gmm.Modes(k)->Weight(), c_SafeMinWeight); // so we divide by sum_n(p(k|x_n))/N * N

        gmm.Modes(k)->setCovariance(cov);
    }
}

//----------------------------------------------------------------------------
bool AC::GMM::EM::Process(const std::vector<Vec3>& observations, GMM3D& gmm)
{
    _ASSERT(m_numTrainingPoints > 0 && m_numTrainingPoints > gmm.Modes().size() && "Invalid number of observations.");
    int numIterations = 0;
    double OldLikelihood;
    double NewLikelihood = -std::numeric_limits<double>::max();
    do {
        // E-Step
        UpdateResponsibilities(observations, gmm);

        // M-Step
        UpdateWeights(gmm);
        UpdateMeans(observations, gmm);
        UpdateCovariances(observations, gmm);

        // Update GMM likelihood (stopping condition)
        OldLikelihood = NewLikelihood;
        NewLikelihood = gmm.LogLikelihood(observations);
    } while (abs((NewLikelihood - OldLikelihood) / OldLikelihood) > m_tolerance && numIterations++ < m_maxIterations);

    // Return true if converged
    return numIterations < m_maxIterations;
}
