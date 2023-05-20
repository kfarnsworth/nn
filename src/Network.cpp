#include "Network.h"

Network::Network()
{
}

Network::~Network()
{
    Clear();
}

void Network::Add(int numNodes, double bias)
{
    m_layers.emplace_back(numNodes, bias);
}

void Network::Clear()
{
    m_layers.clear();
}

void Network::SetNodeWeights(size_t layerIx, size_t nodeIx, const std::vector<double> &weights)
{
    if (layerIx >= LayerCount())
        return;
    m_layers[layerIx].SetNodeWeights(nodeIx, weights);
}

void Network::GetNodeWeights(size_t layerIx, size_t nodeIx, std::vector<double> &weights)
{
    if (layerIx >= LayerCount())
        return;
    m_layers[layerIx].GetNodeWeights(nodeIx, weights);
}

void Network::SetNodeBias(size_t layerIx, size_t nodeIx, const double bias)
{
    if (layerIx >= LayerCount())
        return;
    m_layers[layerIx].SetNodeBias(nodeIx, bias);
}

void Network::GetNodeBias(size_t layerIx, size_t nodeIx, double &bias)
{
    if (layerIx >= LayerCount())
        return;
    m_layers[layerIx].GetNodeBias(nodeIx, bias);
}

size_t Network::GetNodeCount(size_t layerIx)
{
    if (layerIx >= LayerCount())
        return 0;
    return m_layers[layerIx].NumNodes();
};

void Network::Measure(std::vector<double> &inputs)
{
    std::vector<double> &lastInputs = inputs;
    for (NetworkLayer &layer : m_layers)
    {
        layer.Measure(lastInputs);
        lastInputs = layer.GetOutputs();
    }
}

std::vector<double> &Network::GetOutputs()
{
    return m_layers.back().GetOutputs();
}