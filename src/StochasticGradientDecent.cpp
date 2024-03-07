#include <iostream>
#include "StochasticGradientDecent.h"

StochasticGradientDecent::StochasticGradientDecent(double learningRate)
    : m_learningRate(learningRate)
{
    
}

StochasticGradientDecent::~StochasticGradientDecent()
{

}

void StochasticGradientDecent::TrainTest(Network &network, DataSet &dataSet, int batchSize, int numBatches)
{
    int batchCount = 0;
    std::vector<DataSet> dataSets;

    for (int i=0; i<batchSize; i++)
    {
        dataSets.push_back(dataSet);    // same data set
    }

    while (batchCount < numBatches)
    {
        TrainBatch(network, dataSets);
        batchCount++;
        std::cout << "Batch " << batchCount << " complete." << std::endl;
    }
}

void StochasticGradientDecent::Train(Network &network, TrainingData &trainingData, int batchSize, int numBatches)
{
    int batchCount = 0;
    int setCount = 0;
    std::vector<DataSet> dataSets;
    bool complete = false;

    dataSets.resize(batchSize);
    if (!trainingData.GetFirstDataSet(dataSets[setCount]))
    {
        std::cerr << "ERROR: training data input" << std::endl;
        return;
    }
    setCount++;
    do
    {
        while (setCount < batchSize)
        {
            if (!trainingData.GetNextDataSet(dataSets[setCount]))
            {
                setCount--;
                dataSets.resize(setCount);
                complete = true;
                break;
            }
            setCount++;
        }
        TrainBatch(network, dataSets);
        setCount = 0;
        batchCount++;
        if (numBatches != 0 && batchCount >= numBatches)
            break;
        std::cout << "Batch " << batchCount << " complete." << std::endl;
    } while (!complete);
}

void StochasticGradientDecent::TrainBatch(Network &network, std::vector<DataSet> &batchDataSet)
{
    std::vector<std::vector<double>> gradBias;
    std::vector<std::vector<std::vector<double>>> gradWeights;

    // Set sizes for correction arrays
    gradBias.resize(network.LayerCount());
    gradWeights.resize(network.LayerCount());
    for (int i=0; i<network.LayerCount(); i++)
    {
        gradBias[i].resize(network.GetNodeCount(i), 0.0);
        gradWeights[i].resize(network.GetNodeCount(i));
        for (int j=0; j<network.GetNodeCount(i); j++)
        {
            gradWeights[i][j].resize(network.NumInputs(i), 0.0);
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
            double bias;
            network.GetNodeBias(i, j, bias);
            bias -= (m_learningRate / batchDataSet.size()) * gradBias[i][j];
            network.SetNodeBias(i, j, bias);

            std::vector<double> weights;
            network.GetNodeWeights(i, j, weights);
            for (size_t k=0; k<weights.size(); k++)
            {
                weights[k] -= (m_learningRate / batchDataSet.size()) * gradWeights[i][j][k];
            }
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