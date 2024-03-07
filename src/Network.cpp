#include "Network.h"
#include "json.hpp"

Network::Network() : m_numInputs(0)
{
}

Network::~Network()
{
    Clear();
}

void Network::CreateNetwork(std::ifstream &fs)
{
    nlohmann::json data = nlohmann::json::parse(fs);
    m_numInputs = data["inputs"];
    int inputs = m_numInputs;
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        int numNodes = layers[i]["nodes"];
        int bias = layers[i]["bias"];
        Add(numNodes, inputs, bias);
        inputs = numNodes;
    }
}

void Network::SaveNetwork(std::ofstream &fs)
{
    nlohmann::json data;
    nlohmann::json layers;
    data["inputs"] = m_numInputs;
    for (int layerIx=0; layerIx<LayerCount(); layerIx++)
    {
        nlohmann::json layer;
        int nodeCount = GetNodeCount(layerIx);
        nlohmann::json nodes;
        for (int nodeIx=0; nodeIx<nodeCount; nodeIx++)
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
    m_numInputs = data["inputs"];
    int inputs = m_numInputs;
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        auto layer = layers[i];
        size_t nodeCount = layer["count"];
        auto nodes = layer["nodes"];
        Add(nodeCount, inputs);
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
        inputs = nodeCount;
    }
}

void Network::Add(int numNodes, int numInputs, double bias)
{
    m_layers.emplace_back(numNodes, numInputs, bias);
}

void Network::Clear()
{
    m_layers.clear();
}

void Network::SetNodeWeights(int layerIx, int nodeIx, const std::vector<double> &weights)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    m_layers[layerIx].SetNodeWeights(nodeIx, weights);
}

void Network::GetNodeWeights(int layerIx, int nodeIx, std::vector<double> &weights)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    m_layers[layerIx].GetNodeWeights(nodeIx, weights);
}

void Network::SetNodeBias(int layerIx, int nodeIx, const double bias)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    m_layers[layerIx].SetNodeBias(nodeIx, bias);
}

void Network::GetNodeBias(int layerIx, int nodeIx, double &bias)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    m_layers[layerIx].GetNodeBias(nodeIx, bias);
}

int Network::GetNodeCount(int layerIx)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    return m_layers[layerIx].NumNodes();
};

size_t Network::NumInputs(int layerIx)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    return m_layers[layerIx].NumInputs();
}

std::vector<double> &Network::GetOutputs(int layerIx)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    return m_layers[layerIx].GetOutputs();
}

std::vector<double> &Network::GetOutputDerivatives(int layerIx)
{
    if (layerIx >= LayerCount()) throw std::runtime_error("layer index out of range");
    return m_layers[layerIx].GetOutputDerivatives();
}

void Network::Measure(const std::vector<double> &inputs)
{
    std::vector<double> lastInputs = inputs;
    for (size_t i = 0; i<m_layers.size(); i++)
    {
        m_layers[i].Measure(lastInputs);
        lastInputs = m_layers[i].GetOutputs();
    }
}

std::vector<double> &Network::GetOutputs()
{
    return m_layers.back().GetOutputs();
}
