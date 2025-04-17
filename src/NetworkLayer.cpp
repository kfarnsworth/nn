#include <iostream>
#include <random>
#include "NetworkLayer.h"
#include "Threader.hpp"

NetworkLayer::NetworkLayer(size_t numNodes, size_t numInputs, double bias)
{
    std::vector<double> weights;
    CreateWeights(numInputs, weights);
    m_numInputs = numInputs;
    for (size_t i=0; i<numNodes; i++)
    {
        m_nodes.emplace_back(weights, bias);
    }
    m_outputs.resize(numNodes);
    m_outputDerivatives.resize(numNodes);
}

NetworkLayer::~NetworkLayer()
{
    m_nodes.clear();
}

void NetworkLayer::SetNodeWeights(int nodeIx, const std::vector<double> &weights)
{
    if (nodeIx >= NumNodes()) throw std::runtime_error("node index out of range");
    m_nodes[nodeIx].SetWeights(weights);
}

void NetworkLayer::GetNodeWeights(int nodeIx, std::vector<double> &weights)
{
    if (nodeIx >= NumNodes()) throw std::runtime_error("node index out of range");
    m_nodes[nodeIx].GetWeights(weights);
}

void NetworkLayer::SetNodeBias(int nodeIx, const double bias)
{
    if (nodeIx >= NumNodes()) throw std::runtime_error("node index out of range");
    m_nodes[nodeIx].SetBias(bias);
}

void NetworkLayer::GetNodeBias(int nodeIx, double &bias)
{
    if (nodeIx >= NumNodes()) throw std::runtime_error("node index out of range");
    bias = m_nodes[nodeIx].GetBias();
}

//#define TEST_WITH_THREADS
#ifndef TEST_WITH_THREADS
void NetworkLayer::Measure(const std::vector<double> &outputs)
{
    StartTimer();
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputs[i] = m_nodes[i].Measure(outputs);
    }

    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputDerivatives[i] = m_nodes[i].GetOutputDerivative();
    }
    StopTimer();
}
#else
void NetworkLayer::Measure(const std::vector<double> &outputs)
{
    StartTimer();
    Threader threader(m_nodes.size());
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        auto func = std::bind(static_cast<double(NetworkNode::*)(const std::vector<double> &)>(&NetworkNode::Measure), &m_nodes[i], std::placeholders::_1);
        threader.start(i, func, outputs);
        //m_outputs[i] = m_nodes[i].Measure(outputs);
    }
    threader.join();

    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputs[i] = threader.result(i);
        m_outputDerivatives[i] = m_nodes[i].GetOutputDerivative();
    }
    StopTimer();
}
#endif

void NetworkLayer::Measure(std::vector<NetworkNode> &inputNodes)
{
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputs[i] = m_nodes[i].Measure(inputNodes);
        m_outputDerivatives[i] = m_nodes[i].GetOutputDerivative();
    }
}

std::vector<double> &NetworkLayer::GetOutputs()
{
    return m_outputs;
}

std::vector<double> &NetworkLayer::GetOutputDerivatives()
{
    return m_outputDerivatives;
}

const std::vector<NetworkNode> &NetworkLayer::GetNodes()
{
    return m_nodes;
}

double NetworkLayer::Cost(const std::vector<double> expectedOutputs)
{
    if (expectedOutputs.size() != m_nodes.size())
    {
        std::cout << "Can't do cost because expectedOutputs is not the same as nodes." << std::endl;
        return 0.0;
    }
    double cost = 0.0;
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        cost += m_nodes[i].NodeCost(expectedOutputs[i]);
    }
    return cost;
}

void NetworkLayer::CreateWeights(size_t numInputs, std::vector<double> &weights)
{
    std::random_device rd;
    std::mt19937 generator( rd() );

    double const distributionRangeHalfWidth = ( 2.4 / numInputs );
    double const standardDeviation = distributionRangeHalfWidth * 2 / 6;
    std::normal_distribution<> normalDistribution( 0, standardDeviation );

    // Set weights to normally distributed random values between [-2.4 / numInputs, 2.4 / numInputs]
    for ( size_t hiddenIdx = 0; hiddenIdx < numInputs; hiddenIdx++ )
    {
        double const weight = normalDistribution( generator );
        weights.push_back(weight);
    }
}
