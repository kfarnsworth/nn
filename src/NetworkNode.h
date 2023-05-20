#include <vector>
#include <valarray>

class NetworkNode {

public:
    NetworkNode(const std::vector<double> &weights, double bias);
    ~NetworkNode();
    double Measure(const std::vector<double> &inputs);
    double Measure(std::vector<NetworkNode> &inputNodes);
    void SetBias(double bias);
    double GetBias();
    void SetWeights(const std::vector<double> &weights);
    void GetWeights(std::vector<double> &weights);
    double GetOutput() { return m_value; };

private:
    double m_value;
    size_t m_numInputs;
    std::vector<double> m_weights;
    double m_bias;

    double Sigmoid( double x )
    {
        return 1.0 / ( 1.0 + std::exp( -x ) );
    }
};
