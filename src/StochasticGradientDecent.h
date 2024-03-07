#pragma once

#include <vector>
#include "Network.h"
#include "TrainingData.h"

class StochasticGradientDecent
{
public:
    StochasticGradientDecent(double learningRate=1.0);
    ~StochasticGradientDecent();

    void Train(Network &network, TrainingData &trainingData, int batchSize, int numBatches=0);
    void TrainTest(Network &network, DataSet &dataSet, int batchSize, int numBatches=0);

private:
    void TrainBatch(Network &network, std::vector<DataSet> &batchDataSet);
    void BackPropagate(Network &network, DataSet &dataSet, 
        std::vector<std::vector<double>> &gradBias,
        std::vector<std::vector<std::vector<double>>> &gradWeights);

    double m_learningRate;
};