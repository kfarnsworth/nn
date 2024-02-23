#include <fstream>
#include "NetworkLayer.h"

class Network {
public:
    Network();
    virtual ~Network();
    void CreateNetwork(std::ifstream &fs);
    void SaveNetwork(std::ofstream &fs);
    void LoadNetwork(std::ifstream &fs);
    void Add(int numNodes, double bias=-1.0);
    void Clear();
    void SetNodeWeights(size_t layerIx, size_t nodeIx, const std::vector<double> &weights);
    void GetNodeWeights(size_t layerIx, size_t nodeIx, std::vector<double> &weights);
    void SetNodeBias(size_t layerIx, size_t nodeIx, const double bias);
    void GetNodeBias(size_t layerIx, size_t nodeIx, double &bias);
    size_t GetNodeCount(size_t layerIx);
    void Measure(std::vector<double> &inputs);
    std::vector<double> &GetOutputs();
    size_t LayerCount() { return m_layers.size(); };
    size_t NumInputs()
    {
        if (LayerCount()>0) 
            return m_layers.front().NumNodes();
        return 0;
    };
    size_t NumOutputs()
    {
        if (LayerCount()>0) 
            return m_layers.back().NumNodes();
        return 0;
    };

private:
    std::vector<NetworkLayer>   m_layers;
};