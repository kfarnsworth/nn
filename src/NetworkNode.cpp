
#include "NetworkNode.h"
#include <numeric>

NetworkNode::NetworkNode(const std::vector<double> &weights, double bias)
{
    m_numInputs = weights.size();
    m_weights = weights;
    m_bias = bias;
    m_weightedSum = 0;
    m_activation = 0;
}

NetworkNode::~NetworkNode()
{
}

double NetworkNode::Measure(const std::vector<double> &inputs)
{
    if (inputs.size() != m_weights.size()) throw std::runtime_error("weights and inputs not same size");
    // get dot product of weight and inputs
    //double total = std::inner_product(m_weights.begin(), m_weights.end(), inputs.begin(), 0.0);
    double total = 0.0;
    for (size_t i=0; i<inputs.size(); i++)
    {
        total += m_weights[i] * inputs[i];
    }
    m_weightedSum = total + m_bias;
    m_activation = Sigmoid(m_weightedSum);
    m_activationDerivative = SigmoidPrime(m_activation);
    return m_activation;
}

double NetworkNode::Measure(std::vector<NetworkNode> &inputNodes)
{
    double total = 0.0;
    for (size_t i=0; i<inputNodes.size() && i<m_weights.size(); i++)
    {
        total += m_weights[i] * inputNodes[i].GetOutput();
    }
    m_weightedSum = total + m_bias;
    m_activation = Sigmoid(m_weightedSum);
    m_activationDerivative = SigmoidPrime(m_activation);
    return m_activation;
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

double NetworkNode::NodeCost(double expectedOutput)
{
    double error = GetOutput() - expectedOutput;
    return error*error;
}
