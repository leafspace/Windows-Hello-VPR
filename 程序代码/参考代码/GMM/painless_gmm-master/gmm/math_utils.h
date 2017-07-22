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
#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

//----------------------------------------------------------------------------
// Use Eigen by default (you will need to add it to your path)
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/StdVector> // Necessary to use aligned Eigen::Vector2f inside an std::vector 

namespace AC
{
    typedef Eigen::Vector2d Vec2;
    typedef Eigen::Vector3d Vec3;
    typedef Eigen::Matrix2d Mat2;
    typedef Eigen::Matrix3d Mat3;

    // General version
    template <typename T>
    inline void SetZero(T& val)
    {
        val = 0;
    }

    // Specialized version for Eigen
    inline void SetZero(Vec3 vec)
    {
        vec.setZero();
    }

    // Utility to compute the mean of a list of values incrementally (more accurate), as in: http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    template <typename T>
    class OnlineMean {
    public:
        OnlineMean() : m_numSamples(0)
        {
            SetZero(m_currentMean);
        }

        /// <summary> Push new value through the online mean. </summary>
        /// <remarks> Alcollet, 7/30/2013. </remarks>
        /// <param name="value"> The value to push. </param>
        /// <returns> The updated mean. </returns>
        const T& Push(const T& value)
        {
            ++m_numSamples;
            T delta = value - m_currentMean;
            m_currentMean = m_currentMean + delta / float(m_numSamples);
            return m_currentMean;
        }

        /// <summary> Return the current mean value. </summary>
        /// <returns> The current mean value. </returns>
        const T& Mean() const
        {
            _ASSERT(m_numSamples != 0 && "Mean() is ill-defined since there are no elements pushed yet");
            return m_currentMean;
        }

        /// <summary> Resets this object (to zero mean, zero elements). </summary>
        void Reset()
        {
            m_numSamples = 0;
            SetZero(m_currentMean);
        }

        /// <summary> Return the current number of elements. </summary>
        /// <returns> The total number of elements. </returns>
        int NumSamples()
        {
            return static_cast<int>(m_numSamples);
        }

    private:
        size_t m_numSamples;
        T m_currentMean;
    };

    //----------------------------------------------------------------------------
    /// <summary> Compute mean and variance in a single loop using the online approach described in:
    /// Donald E. Knuth (1998). The Art of Computer Programming, volume 2: Seminumerical Algorithms, 3rd edn., p. 232. Boston: Addison-Wesley.
    /// Also see: http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance </summary>
    template <typename T>
    class OnlineMeanVariance
    {
    public:
        OnlineMeanVariance()
            : m_numSamples(0)
            , m_currentMean(0)
            , m_currentMeanSq(0)
            , m_currentVariance(0)
        {}

        // Add a new value
        void Push(T value)
        {
            ++m_numSamples;
            T diff = value - m_currentMean;
            m_currentMean += diff / static_cast<float>(m_numSamples);
            m_currentMeanSq += diff * (value - m_currentMean);
            if (m_numSamples > 1)
                m_currentVariance = m_currentMeanSq / (static_cast<float>(m_numSamples - 1));
        }

        // Reset calculations
        void Reset()
        {
            m_numSamples = 0;
            SetZero(m_currentMean);
            SetZero(m_currentMeanSq);
            SetZero(m_currentVariance);
        }

        // Get current number of samples
        size_t NumSamples() { return m_numSamples; }

        // Get current mean
        T Mean() { return m_currentMean; }

        // Get current variance
        T Variance() { return m_currentVariance; }

    private:
        size_t m_numSamples;
        T m_currentMeanSq;
        T m_currentMean;
        T m_currentVariance;
    };

    //----------------------------------------------------------------------------
    static const double c_SafeMinVariance = 1e-10; // Minimum variance we tolerate without being ill-conditioned 
    static const double c_SafeMinWeight = 1e-20; // Minimum weight we consider valid

    //----------------------------------------------------------------------------
    // Normalize each dimension independently in all observations so that they have zero mean and unit variance. 
    template <typename Vec>
    void WhitenObservations(std::vector<Vec>& observations, Vec& whiteningFactors, double SafeMinVariance = c_SafeMinVariance)
    {
        // Compute variance for each channel and store their inverses in whiteningFactors
        OnlineMeanVariance<double> onlineVariance;
        for (int b = 0; b < whiteningFactors.Size(); ++b) {
            for (auto& observation : observations)
                onlineVariance.Push((double)observation[b]);
            whiteningFactors[b] = onlineVariance.Variance() > SafeMinVariance ? 1 / onlineVariance.Variance() : 1; // If variance is too small, don't invert
            onlineVariance.Reset();
        }

        // Divide each observation by its variance
        for (int b = 0; b < whiteningFactors.size(); ++b) {
            for (auto& observation : observations)
                observation[b] *= whiteningFactors[b];
        }
    }

    //----------------------------------------------------------------------------
    /// <summary> Compute Norm_1 on a matrix. This corresponds to the maximum sum of column values over all columns.  </summary>
    /// <typeparam name="typename Mat"> Type of the typename matrix. </typeparam>
    /// <param name="M"> Matrix for which to calculate the Norm_1 </param>
    /// <returns> Norm_1(M) </returns>
    template <typename Mat>
    double Norm1(const Mat& m)
    {
        // This is the conceptual code that should be executed
        /*
        double maxSumCols = 0;
        for (int j = 0; j < m.cols(); ++j) {
            double sumCol = 0;
            // Sum of all absolute values for one column
            for (int i = 0; i < m.rows(); ++i)
                sumCol += (double)abs(m(i, j));
            // Maximum sum of column values over all columns
            if (sumCol > maxSumCols)
                maxSumCols = sumCol;
        }
        return maxSumCols;
        */
        // This is the equivalent code in Eigen. Alternatively, we could also use Eigen::lpNorm<1>(m);
        return m.colwise().sum().maxCoeff(); 
    }

    //----------------------------------------------------------------------------
    /// <summary> Compute the reciprocal condition number of matrix M. This value should be close to 1.0 for well conditioned matrices. </summary>
    /// <typeparam name="typename Mat"> Type of the typename matrix. Should have ".inverse()" function. </typeparam>
    /// <param name="m"> The const Mat&amp; to process. </param>
    /// <returns> . </returns>
    template <typename Mat>
    double RCOND(const Mat& m)
    {
        return 1 / (Norm1(m) * Norm1(m.inverse()));
    }

    //----------------------------------------------------------------------------
    /// <summary> Query if 'arg' is finite (not infinite or NaN) </summary>
    template<typename T> bool IsFinite(T arg)
    {
        return arg == arg &&
            arg != std::numeric_limits<T>::infinity() &&
            arg != -std::numeric_limits<T>::infinity();
    }
}
#endif