#include <random>
#include "NetworkLayer.h"

NetworkLayer::NetworkLayer(int numNodes, double bias)
{
    std::vector<double> weights;
    CreateWeights(numNodes, weights);
    for (size_t i=0; i<numNodes && i<weights.size(); i++)
    {
        m_nodes.emplace_back(weights, bias);
    }
    m_outputs.resize(numNodes);
}

NetworkLayer::~NetworkLayer()
{
    m_nodes.clear();
}

void NetworkLayer::SetNodeWeights(size_t nodeIx, const std::vector<double> &weights)
{
    if (nodeIx >= NumNodes())
        return;
    m_nodes[nodeIx].SetWeights(weights);
}

void NetworkLayer::GetNodeWeights(size_t nodeIx, std::vector<double> &weights)
{
    if (nodeIx >= NumNodes())
        return;
    m_nodes[nodeIx].GetWeights(weights);
}

void NetworkLayer::SetNodeBias(size_t nodeIx, const double bias)
{
    if (nodeIx >= NumNodes())
        return;
    m_nodes[nodeIx].SetBias(bias);
}

void NetworkLayer::GetNodeBias(size_t nodeIx, double &bias)
{
    if (nodeIx >= NumNodes())
        return;
    bias = m_nodes[nodeIx].GetBias();
}

void NetworkLayer::Measure(const std::vector<double> &outputs)
{
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputs[i] = m_nodes[i].Measure(outputs);
    }
}

void NetworkLayer::Measure(std::vector<NetworkNode> &inputNodes)
{
    for(size_t i=0; i<m_nodes.size(); i++)
    {
        m_outputs[i] = m_nodes[i].Measure(inputNodes);
    }
}

std::vector<double> &NetworkLayer::GetOutputs()
{
    return m_outputs;
}

const std::vector<NetworkNode> &NetworkLayer::GetNodes()
{
    return m_nodes;
}

void NetworkLayer::CreateWeights(size_t numNodes, std::vector<double> &weights)
{
    std::random_device rd;
    std::mt19937 generator( rd() );

    double const distributionRangeHalfWidth = ( 2.4 / numNodes );
    double const standardDeviation = distributionRangeHalfWidth * 2 / 6;
    std::normal_distribution<> normalDistribution( 0, standardDeviation );

    // Set weights to normally distributed random values between [-2.4 / numInputs, 2.4 / numInputs]
    for ( size_t hiddenIdx = 0; hiddenIdx < numNodes; hiddenIdx++ )
    {
        double const weight = normalDistribution( generator );
        weights.push_back(weight);
    }
}