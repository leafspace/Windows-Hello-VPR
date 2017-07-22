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
#include "random_generator.h"

//----------------------------------------------------------------------------
template <typename Vec>
AC::KMeans<Vec>::KMeans(int numMeans) : m_numMeans(numMeans), m_centroids(numMeans), m_avgDistancesPerCentroid(numMeans), m_tmpCentroidMeans(numMeans), m_maxIterations(100)
{
    m_centroids.resize(numMeans);
}

//----------------------------------------------------------------------------
template <typename Vec>
int AC::KMeans<Vec>::Process(const std::vector<Vec>& observations, int numRestarts, std::vector<int>& assignments)
{
    if (observations.size() < numCentroids())
        return false;

    this->m_centroids.resize(m_numMeans);
    for (int i = 0; i < int(this->m_centroids.size()); i++)
        this->m_centroids[i] = observations[0];

    m_numTrainingPoints = (int)observations.size();
    RandomGenerator subsetGenerator;
    std::vector<Vec> bestCentroids(numCentroids(), observations[0]);
    double bestDistance = std::numeric_limits<double>::max();
    subsetGenerator.setLimits(0, (int)observations.size() - 1);
    std::vector<int> initialCentroids(numCentroids());

    // Ensure the assingments and observations have the same size
    assignments.resize(observations.size());

    for (int i = 0; i < numRestarts; ++i)
    {
        // Choose initial centroids
        subsetGenerator.NonRepeatingSubset(initialCentroids);
        for (int j = 0; j < numCentroids(); ++j)
            Centroids(j) = observations[initialCentroids[j]];

        int assignmentChanges = 1;
        double avgDistance = 0;
        int numIterations = 0;
        // Process
        while (assignmentChanges && numIterations < m_maxIterations)
        {
            // E-Step
            avgDistance = ClosestCentroids(observations, assignments, assignmentChanges);

            // M-Step
            UpdateCentroids(observations, assignments);

            numIterations++;
        }
        // Check if this restart is better than the previous ones
        if (avgDistance < bestDistance)
        {
            bestCentroids = Centroids();
            bestDistance = avgDistance;
        }
    }
    // Update KMeans centroids with the best centroids we found
    Centroids() = bestCentroids;
    // Recompute best assignments corresponding to best centroids
    int numAssignmentChanges = 0;
    ClosestCentroids(observations, assignments, numAssignmentChanges);
    return numAssignmentChanges;
}

//----------------------------------------------------------------------------
template <typename Vec>
double AC::KMeans<Vec>::ClosestCentroid(const Vec& observation, int& assignment)
{
    double bestDistance = std::numeric_limits<double>::max();
    // Compute distance to each centroid and pick the lowest. We could use a kd-tree here if we had many centroids.
    for (int k = 0; k < numCentroids(); ++k)
    {
        double distance = (observation - Centroids(k)).norm();
        if (distance < bestDistance) {
            assignment = k;
            bestDistance = distance;
        }
    }
    return bestDistance;
}

//----------------------------------------------------------------------------
template <typename Vec>
double AC::KMeans<Vec>::ClosestCentroids(const std::vector<Vec>& observations, std::vector<int>& assignments, int& numAssignmentChanges)
{
    _ASSERT(observations.size() == assignments.size() && L"Vectors must be the same size");
    _ASSERT(m_avgDistancesPerCentroid.size() == numCentroids() && L"Invalid vector size");

    // Clean up centroid statistics
    for (auto& avgDistance : m_avgDistancesPerCentroid)
        avgDistance.Reset();

    numAssignmentChanges = 0;
    int oldAssignment;

    for (int i = 0; i < observations.size(); ++i)
    {
        // Choose nearest centroid
        oldAssignment = assignments[i];
        double distance = ClosestCentroid(observations[i], assignments[i]);

        // Keep track of assignment changes 
        if (assignments[i] != oldAssignment)
            numAssignmentChanges++;

        // Update centroid statistics (online mean)
        m_avgDistancesPerCentroid[assignments[i]].Push(distance);
    }

    // Compute average variance for each centroid (i.e., our metric to choose one assignment over another)
    double avgVariance = 0;
    for (int i = 0; i < numCentroids(); ++i)
        avgVariance += m_avgDistancesPerCentroid[i].Variance();

    return avgVariance;
}

//----------------------------------------------------------------------------
template <typename Vec>
void AC::KMeans<Vec>::UpdateCentroids(const std::vector<Vec>& observations, const std::vector<int>& assignments)
{
    // Clean centroids first
    for (auto& centroid : m_tmpCentroidMeans)
        centroid.Reset();

    // Online computation of mean: http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    for (int i = 0; i < (int)observations.size(); ++i)
        m_tmpCentroidMeans[assignments[i]].Push(observations[i]);

    // Copy centroids to its permanent storage
    for (int k = 0; k < numCentroids(); ++k)
    {
        if (m_tmpCentroidMeans[k].NumSamples() != 0)
            Centroids(k) = m_tmpCentroidMeans[k].Mean();
    }
}