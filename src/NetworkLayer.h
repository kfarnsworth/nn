
#include <vector>
#include "NetworkNode.h"

class NetworkLayer {

public:
    NetworkLayer(int numNodes, double bias);
    ~NetworkLayer();

    std::vector<double> &GetOutputs();
    double Cost(const std::vector<double> expectedOutputs);
    const std::vector<NetworkNode> &GetNodes();
    void SetNodeWeights(size_t nodeIx, const std::vector<double> &weights);
    void GetNodeWeights(size_t nodeIx, std::vector<double> &weights);
    void SetNodeBias(size_t nodeIx, const double bias);
    void GetNodeBias(size_t nodeIx, double &bias);
    void Measure(const std::vector<double> &outputs);
    void Measure(std::vector<NetworkNode> &inputNodes);
    size_t NumNodes() { return m_nodes.size(); };

    std::vector<double> m_outputs;

private:
    std::vector<NetworkNode> m_nodes;

    void CreateWeights(size_t numNodes, std::vector<double> &weights);
};