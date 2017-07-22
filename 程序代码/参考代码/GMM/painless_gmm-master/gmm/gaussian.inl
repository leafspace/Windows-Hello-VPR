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
#define _USE_MATH_DEFINES
#include <math.h>

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
AC::GaussianDistribution<Vec, Mat, Dims>::GaussianDistribution() : m_underflowProtection(true)
{
    Reinitialize(Vec::Zero() /* zero mean */, 1.0 /* Unit variance */, 1.0 /* Unit weight */);
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
AC::GaussianDistribution<Vec, Mat, Dims>::GaussianDistribution(const Vec& mean, double variance, double weight) : m_underflowProtection(true)
{
    Reinitialize(mean, variance, weight);
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
AC::GaussianDistribution<Vec, Mat, Dims>::GaussianDistribution(const GaussianDistribution<Vec, Mat, Dims>& rhs) :
    m_weight(rhs.m_weight),
    m_logWeight(rhs.m_logWeight),
    m_logNormFactor(rhs.m_logNormFactor),
    m_mean(rhs.m_mean),
    m_covariance(rhs.m_covariance),
    m_invCovariance(rhs.m_invCovariance),
    m_underflowProtection(rhs.m_underflowProtection)
{
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
void AC::GaussianDistribution<Vec, Mat, Dims>::swap(GaussianDistribution<Vec, Mat, Dims>& rhs)
{
    using std::swap;
    swap(m_weight, rhs.m_weight);
    swap(m_logWeight, rhs.m_logWeight);
    swap(m_logNormFactor, rhs.m_logNormFactor);
    swap(m_mean, rhs.m_mean);
    swap(m_covariance, rhs.m_covariance);
    swap(m_invCovariance, rhs.m_invCovariance);
    swap(m_underflowProtection, rhs.m_underflowProtection);
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
GaussianDistribution<Vec, Mat, Dims>& AC::GaussianDistribution<Vec, Mat, Dims>::operator=(GaussianDistribution<Vec, Mat, Dims> rhs)
{
    swap(rhs);
    return *this;
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
void AC::GaussianDistribution<Vec, Mat, Dims>::setCovariance(const Mat& cov)
{
    m_covariance = cov;

    double determinant;
    if (UnderflowProtection()) {
        // A very small determinant leads to an invalid logNormFactor. If that happens, reset the covariance matrix to a diagonal one.
        determinant = m_covariance.determinant();
        if (determinant < c_SafeDeterminantWithoutUnderflow) {
            m_covariance.setIdentity();
            m_covariance *= c_SafeCovarianceFactor;
        }

        // Is this covariance matrix degenerate? If RCOND is close to zero, it means that cov is ill-conditioned (close to degenerate).
        if (RCOND(m_covariance) < c_SafeMatrixRCOND) {
            // Make cov = cov + eye(Dims)*SomeSmallValue to make it better conditioned, even if it's inaccurate.
            for (int i = 0; i < Dims; ++i)
                m_covariance(i, i) += c_SafeCovarianceFactor;
        }
    }

    determinant = m_covariance.determinant();
    m_invCovariance = m_covariance.inverse();
    m_logNormFactor = -0.5*log(pow(M_PI * 2, Dims) * determinant);
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
double AC::GaussianDistribution<Vec, Mat, Dims>::EvaluateLog(const Vec& observation)
{
    double exponentialTerm = ((observation - Mean()).transpose() * InvCovariance() * (observation - Mean()));
    return m_logNormFactor - 0.5 * exponentialTerm;
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
double AC::GaussianDistribution<Vec, Mat, Dims>::Evaluate(const Vec& observation)
{
    return exp(EvaluateLog(observation));
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
void AC::GaussianDistribution<Vec, Mat, Dims>::Reinitialize(const Vec& mean, double variance, double weight)
{
    m_mean = mean;
    Mat cov;
    cov.setIdentity();
    cov *= UnderflowProtection() ? std::max(variance, c_SafeCovarianceFactor) : variance;
    setCovariance(cov);
    setWeight(weight);
}

//----------------------------------------------------------------------------
template <typename Vec, typename Mat, int Dims>
void AC::GaussianDistribution<Vec, Mat, Dims>::Rescale(const Vec& scalingFactors)
{
    Mat scalingMat;
    scalingMat.setZero();
    for (int i = 0; i < scalingFactors.size(); ++i) {
        _ASSERT(!UnderflowProtection() || (UnderflowProtection() && scalingFactors[i] > c_SafeCovarianceFactor && "Invalid Scaling Factor (should be > 0)"));
        scalingMat(i, i) = scalingFactors[i] > c_SafeCovarianceFactor ? 1 / scalingFactors[i] : 1;
    }

    m_mean = scalingMat * m_mean;

    // Rescale covariance matrix: newCov = scaling * oldCov * scaling
    m_covariance = scalingMat * (m_covariance * scalingMat);
    setCovariance(m_covariance); // Update invCovariance and the gaussian normalization factors
}
