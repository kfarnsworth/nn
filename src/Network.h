#pragma once

#include <fstream>
#include <mutex>
#include "NetworkLayer.h"

class Network {
public:
    Network();
    virtual ~Network();
    void CreateNetwork(std::ifstream &fs);
    void SaveNetwork(std::ofstream &fs);
    void LoadNetwork(std::ifstream &fs);
    void Add(int numNodes, int numInputs, double bias=-1.0);
    void Clear();
    void SetNodeWeights(int layerIx, int nodeIx, const std::vector<double> &weights);
    void GetNodeWeights(int layerIx, int nodeIx, std::vector<double> &weights);
    void SetNodeBias(int layerIx, int nodeIx, const double bias);
    void GetNodeBias(int layerIx, int nodeIx, double &bias);
    int GetNodeCount(int layerIx);
    size_t NumInputs(int layerIx);
    std::vector<double> &GetOutputs(int layerIx);
    std::vector<double> &GetOutputDerivatives(int layerIx);
    void Measure(const std::vector<double> &inputs);
    std::vector<double> &GetOutputs();
    int LayerCount() { return m_layers.size(); };
    size_t NumInputs()
    {
        return m_numInputs;
    };
    size_t NumOutputs()
    {
        if (LayerCount()>0) 
            return m_layers.back().NumNodes();
        return 0;
    };
    void SetTrainingState(bool state) { m_isTraining = state; }
    bool IsTraining() { return m_isTraining; }
    
    void GetInputState(std::vector<double> &inputs);
    void GetOutputState(int layerIx, std::vector<double> &outputs);
    void GetBiasState(int layerIx, std::vector<double> &biases);
    void GetWeightsState(int layerIx, std::vector<std::vector<double>> &weightsPerNode);
    static void NetworkFiles(std::vector<std::string> &list, const std::string dir=DEFAULT_NETWORK_DIRECTORY);
    static const char *NetworkDirectory() { return DEFAULT_NETWORK_DIRECTORY; };

private:
    int m_numInputs;
    std::vector<NetworkLayer>   m_layers;
    bool m_isTraining;
    std::vector<double> stateInputs;
    mutable std::mutex m_MeasureMutex;
    static constexpr char DEFAULT_NETWORK_DIRECTORY[] = "networks";
};