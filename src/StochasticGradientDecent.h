#pragma once

#include <vector>
#include "Training.h"
#include "Network.h"
#include "TrainingData.h"

class StochasticGradientDecent : public Training
{
public:
    StochasticGradientDecent(Network &network);
    ~StochasticGradientDecent();
    std::string GetType() { return "SGD"; }

protected:
    void TrainBatch(Network &network, std::vector<DataSet> &batchDataSet);

private:
    void BackPropagate(Network &network, DataSet &dataSet,
        std::vector<std::vector<double>> &gradBias,
        std::vector<std::vector<std::vector<double>>> &gradWeights);
};
