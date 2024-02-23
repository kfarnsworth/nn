#include "Network.h"
#include "json.hpp"

Network::Network()
{
}

Network::~Network()
{
    Clear();
}

void Network::CreateNetwork(std::ifstream &fs)
{
    nlohmann::json data = nlohmann::json::parse(fs);
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        int numNodes = layers[i]["nodes"];
        int bias = layers[i]["bias"];
        Add(numNodes, bias);
    }
}

void Network::SaveNetwork(std::ofstream &fs)
{
    nlohmann::json data;
    nlohmann::json layers;
    for (size_t layerIx=0; layerIx<LayerCount(); layerIx++)
    {
        nlohmann::json layer;
        size_t nodeCount = GetNodeCount(layerIx);
        nlohmann::json nodes;
        for (size_t nodeIx=0; nodeIx<nodeCount; nodeIx++)
        {
            std::vector<double> weights;
            double bias;
            GetNodeWeights(layerIx, nodeIx, weights);
            GetNodeBias(layerIx, nodeIx, bias);
            nlohmann::json w_array(weights);
            nlohmann::json node;
            node["bias"] = bias;
            node["weights"] = weights;
            nodes.push_back(node);
        }
        layer["count"] = nodeCount;
        layer["nodes"] = nodes;
        layers.push_back(layer);
    }
    data["layers"] = layers;
    fs << std::setw(4) << data << std::endl
;}

void Network::LoadNetwork(std::ifstream &fs)
{
    Clear();
    nlohmann::json data = nlohmann::json::parse(fs);
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        auto layer = layers[i];
        size_t nodeCount = layer["count"];
        auto nodes = layer["nodes"];
        Add(nodeCount);
        for (size_t n=0; n<nodes.size(); n++)
        {
            auto node = nodes[n];
            double bias = node["bias"];
            std::vector<double> weights = node["weights"];
            for (size_t j=0; j<nodeCount; j++)
            {
                SetNodeBias(i, j, bias);
                SetNodeWeights(i, j, weights);
            }
        }
    }
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