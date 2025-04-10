#include <iostream>
#include <ctime>
#include "StochasticGradientDecent.h"

StochasticGradientDecent::StochasticGradientDecent(Network &network)
    : Training(network)
{

}

StochasticGradientDecent::~StochasticGradientDecent()
{

}

void StochasticGradientDecent::TrainBatch(Network &network, std::vector<DataSet> &batchDataSet)
{
    std::vector<std::vector<double>> gradBias;
    std::vector<std::vector<std::vector<double>>> gradWeights;
    std::vector<std::vector<double>> biasDeltaLast;
    std::vector<std::vector<std::vector<double>>> weightsDeltaLast;
    double batchLearningRate = m_learningRate / batchDataSet.size();
    double batchMomentum = m_momentum / batchDataSet.size();

    // Set sizes for correction arrays
    gradBias.resize(network.LayerCount());
    gradWeights.resize(network.LayerCount());
    biasDeltaLast.resize(network.LayerCount());
    weightsDeltaLast.resize(network.LayerCount());
    for (int i=0; i<network.LayerCount(); i++)
    {
        gradBias[i].resize(network.GetNodeCount(i), 0.0);
        gradWeights[i].resize(network.GetNodeCount(i));
        biasDeltaLast[i].resize(network.GetNodeCount(i), 0.0);
        weightsDeltaLast[i].resize(network.GetNodeCount(i));
        for (int j=0; j<network.GetNodeCount(i); j++)
        {
            gradWeights[i][j].resize(network.NumInputs(i), 0.0);
            weightsDeltaLast[i][j].resize(network.NumInputs(i), 0.0);
        }
    }
    // train with the batch
    for (DataSet &dataSet: batchDataSet)
    {
        BackPropagate(network, dataSet, gradBias, gradWeights);
    }

    // adjust biases and weights after the batch completes
    for (int i=0; i<network.LayerCount(); i++)
    {
        for (int j=0; j<network.GetNodeCount(i); j++)
        {
            double bias, biasDelta;
            network.GetNodeBias(i, j, bias);
            biasDelta = (batchMomentum * biasDeltaLast[i][j]) -
                            (batchLearningRate * gradBias[i][j]);
            bias += biasDelta;
            biasDeltaLast[i][j] = biasDelta;

            std::vector<double> weights;
            network.GetNodeWeights(i, j, weights);
            for (size_t k=0; k<weights.size(); k++)
            {
                double weightDelta = (batchMomentum * weightsDeltaLast[i][j][k]) -
                                      (batchLearningRate * gradWeights[i][j][k]);
                weights[k] += weightDelta;
                weightsDeltaLast[i][j][k] = weightDelta;
            }

            network.SetNodeBias(i, j, bias);
            network.SetNodeWeights(i, j, weights);
        }
    }
}

void StochasticGradientDecent::BackPropagate(Network &network, DataSet &dataSet,
        std::vector<std::vector<double>> &gradBias,
        std::vector<std::vector<std::vector<double>>> &gradWeights)
{
    // Perform a measurement in the network
    network.Measure(dataSet.input);

    // Work backwards from last layer to the first
    std::vector<double> prevDeltas;
    for (int lix = network.LayerCount()-1; lix >= 0 ; lix--)
    {
        std::vector<double> &outputs = network.GetOutputs(lix);
        std::vector<double> &outputDerivatives = network.GetOutputDerivatives(lix);
        std::vector<double> &inputs = (lix>0) ? network.GetOutputs(lix-1) : dataSet.input;

        std::vector<double> deltas;
        deltas.resize(network.GetNodeCount(lix));

        for (int nix=0; nix<network.GetNodeCount(lix); nix++)
        {
            double delta = 0.0;
            if (lix < network.LayerCount()-1)
            {
                // weights times delta of last layer calculations
                std::vector<double> prevWeights;
                for (size_t k=0; k<prevDeltas.size(); k++)
                {
                    network.GetNodeWeights(lix+1, k, prevWeights);
                    delta += prevDeltas[k] * prevWeights[nix];
                }
            }
            else
            {   // compare network input to network output
                delta = outputs[nix] - dataSet.output[nix];
            }
            delta *= outputDerivatives[nix];
            gradBias[lix][nix] += delta;
            for (size_t j=0; j<network.NumInputs(lix); j++)
            {
                gradWeights[lix][nix][j] += delta * inputs[nix];
            }
            deltas[nix] = delta;
        }
        prevDeltas = deltas;
    }
}
