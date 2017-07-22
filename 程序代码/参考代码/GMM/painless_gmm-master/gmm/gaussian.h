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
#ifndef __GAUSSIAN_H__
#define __GAUSSIAN_H__

#include "math_utils.h"

namespace AC
{
    static const double c_SafeDeterminantWithoutUnderflow = 1e-50; // minimum determinant without underflow in Gaussian evaluation
    static const double c_SafeMatrixRCOND = 1e-10; // Minimum value for us to consider that a covariance matrix is badly conditioned
    static const double c_SafeCovarianceFactor = 1e-10; // Factor added to the diagonal elements of an ill-conditioned covariance matrix to make it well-conditioned.

    template <typename Vec, typename Mat, int Dims>
    class GaussianDistribution
    {
    public:
        typedef std::shared_ptr<GaussianDistribution<Vec, Mat, Dims>> SP;

        /// <summary> Default constructor (initializes with mean 0 and unit spherical covariance). </summary>
        GaussianDistribution();

        /// <summary> Constructor. </summary>
        /// <param name="mean">     The mean. </param>
        /// <param name="variance"> The variance (spherical covariance matrix with 'variance' scaling). </param>
        GaussianDistribution(const Vec& mean, double variance, double weight = 1);

        // Copy constructor
        GaussianDistribution(const GaussianDistribution<Vec, Mat, Dims>& rhs);

        // swap contents
        void swap(GaussianDistribution<Vec, Mat, Dims>& rhs);

        // Operator=
        GaussianDistribution<Vec, Mat, Dims>& operator=(GaussianDistribution<Vec, Mat, Dims> rhs);

        /// <summary> Evaluates Gaussian distribution at the given observation point. </summary>
        /// <param name="observation"> The observation. </param>
        /// <returns> The value of the gaussian pdf(observation).  </returns>
        double Evaluate(const Vec& observation);

        /// <summary> Evaluates log Gaussian distribution at the given observation point. </summary>
        /// <param name="observation"> The observation. </param>
        /// <returns> The value of log (gaussian pdf(observation)). </returns>
        double EvaluateLog(const Vec& observation);

        /// <summary> Reinitialize Gaussian distribution with some mean and variance (spherical covariance matrix)  </summary>
        /// <param name="mean">     The mean. </param>
        /// <param name="variance"> The variance. We build a spherical covariance matrix cov with cov[i,i] = variance. </param>
        /// <param name="weight">   (optional) the weight of this Gaussian distribution. </param>
        void Reinitialize(const Vec& mean, double variance, double weight = 1);

        /// <summary> Rescale Gaussian (mean and covariance matrix) with the given scaling factors. The end result is that, after scaling Evaluate(obs) will transform into Evaluate(scaling * obs). </summary>
        /// <param name="scalingFactors"> The scaling factors. </param>
        void Rescale(const Vec& scalingFactors);

        const Mat& Covariance() const { return m_covariance; }
        const Mat& InvCovariance() const { return m_invCovariance; }
        void setCovariance(const Mat& cov);

        const Vec& Mean() const { return m_mean; }
        Vec& Mean() { return m_mean; }
        void setMean(const Vec& mean) { m_mean = mean; }

        double Weight() const { return m_weight; }
        double LogWeight() const { return m_logWeight; }
        void setWeight(double w) { m_weight = w; m_logWeight = log(w); }

        /// <summary> Enable/disable underflow protection. If enabled, we reshape/resize the covariance matrix to diagonal when it is ill-conditioned. 
        ///           WARNING: This factor may change the shape/size of your covariance matrix. Keep it in mind! </summary>
        void EnableUnderflowProtection() { m_underflowProtection = true; }
        void DisableUnderflowProtection() { m_underflowProtection = false; }
        bool UnderflowProtection() const { return m_underflowProtection; }

        const int Dimensions() const { return Dims; }

    private:
        Mat m_covariance; // Covariance matrix
        Mat m_invCovariance; // Inverse Covariance matrix
        Vec m_mean; // Mean of the distribution
        double m_logNormFactor; // Normalization factor in log scale. That is: -log (1/(2*PI)^(M/2)) - log (det(Cov)^(1/2))
        double m_weight; // Weight of distribution
        double m_logWeight; // log(Weight) of distribution
        bool m_underflowProtection; // Use underflow protection in covariance matrix.
                                    /* Temporaries */
        Vec m_tmpCenteredObservation; // Temporary vector to avoid lots of constructor calls
        Vec m_tmpProdOutput; // Temporary vector to avoid lots of constructor calls
    };
    typedef GaussianDistribution<Vec3, Mat3, 3> GaussianDistribution3D;

#include "gaussian.inl"
}

#endif
