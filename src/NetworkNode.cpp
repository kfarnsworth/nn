
#include "NetworkNode.h"

NetworkNode::NetworkNode(const std::vector<double> &weights, double bias)
{
    m_numInputs = weights.size();
    m_weights = weights;
    m_bias = bias;
    m_value = 0;
}

NetworkNode::~NetworkNode() 
{   
}

double NetworkNode::Measure(const std::vector<double> &inputs)
{
    double total = 0.0;
    for (size_t i=0; i<inputs.size(); i++)
    {
        total += m_weights[i] * inputs[i];
    }
    total += m_bias;
    return Sigmoid(total);
}

double NetworkNode::Measure(std::vector<NetworkNode> &inputNodes)
{
    double total = 0.0;
    for (size_t i=0; i<inputNodes.size() && i<m_weights.size(); i++)
    {
        total += m_weights[i] * inputNodes[i].GetOutput();
    }
    total += m_bias;
    return Sigmoid(total);
}

void NetworkNode::SetBias(double bias)
{
    m_bias = bias;
}

double NetworkNode::GetBias()
{
    return m_bias;
}

void NetworkNode::SetWeights(const std::vector<double> &weights)
{
    m_weights = weights;
}

void NetworkNode::GetWeights(std::vector<double> &weights)
{
    weights = m_weights;
}
