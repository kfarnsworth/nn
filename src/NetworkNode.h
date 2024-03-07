#pragma once

#include <vector>
#include <valarray>
#include <cmath>
#include <exception>
#include <stdexcept>

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
    double GetOutput() { return m_activation; };
    double NodeCost(double expectedOutput);
    double GetWeightedSum() { return m_weightedSum; }
    double GetOutputDerivative() { return m_activationDerivative; }

private:
    double m_weightedSum;
    double m_activation;
    double m_activationDerivative;
    size_t m_numInputs;
    std::vector<double> m_weights;
    double m_bias;

    double Sigmoid( double x )
    {
        if (std::isnan(x)) throw std::runtime_error("Sigmoid input NaN");
        double value = 1.0 / ( 1.0 + std::exp( -x ) );
        if (std::isnan(value)) throw std::runtime_error("Sigmoid output NaN");
        return value;
    }

    double SigmoidPrime( double xSigmoid )
    {
        double value = xSigmoid*(1 - xSigmoid);
        return value;
    }
};
