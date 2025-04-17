#pragma once

#include <vector>
#include "NetworkNode.h"
#include "Stats.hpp"

class NetworkLayer : public Stats {

public:
    NetworkLayer(size_t numNodes, size_t numInputs, double bias);
    ~NetworkLayer();

    std::vector<double> &GetOutputs();
    std::vector<double> &GetOutputDerivatives();
    double Cost(const std::vector<double> expectedOutputs);
    const std::vector<NetworkNode> &GetNodes();
    void SetNodeWeights(int nodeIx, const std::vector<double> &weights);
    void GetNodeWeights(int nodeIx, std::vector<double> &weights);
    void SetNodeBias(int nodeIx, const double bias);
    void GetNodeBias(int nodeIx, double &bias);
    void Measure(const std::vector<double> &outputs);
    void Measure(std::vector<NetworkNode> &inputNodes);
    int NumNodes() { return m_nodes.size(); };
    int NumInputs() { return m_numInputs; };
    void DumpLayerStats(size_t ix) {
        std::cout << "Layer " << ix << std::endl;
        DumpStats();
    }

    std::vector<double> m_outputs;
    std::vector<double> m_outputDerivatives;

private:
    std::vector<NetworkNode> m_nodes;
    size_t m_numInputs;

    void CreateWeights(size_t numInputs, std::vector<double> &weights);
};
