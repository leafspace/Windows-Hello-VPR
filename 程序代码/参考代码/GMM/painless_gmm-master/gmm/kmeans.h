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
#ifndef __KMEANS_H__
#define __KMEANS_H__

#include <memory>
#include <vector>

namespace AC
{
    template <typename Vec>
    class KMeans {
    public:
        typedef std::shared_ptr<KMeans<Vec>> SP;

        /// <summary> Constructor. </summary>
        /// <param name="numMeans"> Number of centroids to use in k-means (parameter k). </param>
        KMeans(int numMeans);

        /// <summary> Compute K-Means algorithm on a set of observations with N random restarts. The resulting centroids are available through the function Centroids(). </summary>
        /// <param name="observations"> N-vector of D-dimensional observations. </param>
        /// <param name="restarts"> Number of times to restart the algorithm (with random initialization). </param>
        /// <param name="assignments">  [out] N-vector of point assignments. Assignment[i] = C --> means that observation[i] is clustered with centroid C. </param>
        /// <returns> Number of assignment changes at the last iteration (or 0 if the algorithm converged). </returns>
        int Process(const std::vector<Vec>& observations, int restarts, std::vector<int>& assignments);

        /// <summary> Compute closest centroid for a given observation. </summary>
        /// <param name="observation"> The observation. </param>
        /// <param name="assignment">  [out] The closest centroid to this observation. </param>
        /// <returns> The distance from the closest centroid to the observation. </returns>
        double ClosestCentroid(const Vec& observation, int& assignment);

        /// <summary> Compute closest centroids for a set of observations.  </summary>
        /// <param name="observation"> The set of observations. </param>
        /// <param name="assignments"> [out] The computed assignments. </param>
        /// <param name="numAssignmentChanges"> [out] Number of assignment changes from the previous iteration of KMeans. </param>
        /// <returns> The average distance from an observation to its centroid. </returns>
        double ClosestCentroids(const std::vector<Vec>& observation, std::vector<int>& assignments, int& numAssignmentChanges);

        /// <summary> Updates the centroids given a set of observations and assignments. </summary>
        /// <param name="observations"> The observations. </param>
        /// <param name="assignments">  The assignments. </param>
        void UpdateCentroids(const std::vector<Vec>& observations, const std::vector<int>& assignments);

        // Get centroids
        std::vector<Vec>& Centroids() { return m_centroids; }
        const std::vector<Vec>& Centroids() const { return m_centroids; }

        // Get centroid(k)
        Vec& Centroids(int k) { return m_centroids[k]; }
        const Vec& Centroids(int k) const { return m_centroids[k]; }

        // Number of centroids (parameter k)
        int numCentroids() { return m_numMeans; }

        // Maximum number of iterations
        void setMaxIterations(int maxIterations) { m_maxIterations = maxIterations; }

        // Average distance of points to this centroid
        double AvgDistance(int k) { return m_avgDistancesPerCentroid[k].Mean(); }
        double AvgVariance(int k) { return m_avgDistancesPerCentroid[k].Variance(); }

        // Ratio of points that belong to centroid k
        double CentroidAssignmentRatio(int k) { return (double)m_avgDistancesPerCentroid[k].NumSamples() / (double)m_numTrainingPoints; }

        // Returns number of samples in centroid k
        size_t NumSamplesAssignedToCentroid(int k) { return m_avgDistancesPerCentroid[k].NumSamples(); }

    private:
        int m_maxIterations; // Maximum number of iterations in KMeans (should not be necessary, in theory, but just in case...)
        int m_numMeans; // Parameter K in k-means
        std::vector<Vec> m_centroids; // K-vector of centroids
        std::vector<OnlineMeanVariance<double>> m_avgDistancesPerCentroid; // K-vector containing the average distance to each centroid
        std::vector<OnlineMean<Vec>> m_tmpCentroidMeans; // K-Vector with helper classes to compute centroids
        int m_numTrainingPoints; // Number of observations used in training
    };

    typedef KMeans<Vec2> KMeans2D;
    typedef KMeans<Vec3> KMeans3D;

    /// <summary> Basic test for KMeans. Generate a set of noisy observations around a set of centroids, run KMeans, and compare the centroids and assignments. </summary>
    /// <returns> true if passed the test </returns>
    bool TestKMeans3D();
}

#include "KMeans.inl"

#endif
