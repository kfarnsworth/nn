#include <iostream>
#include <fstream>
#include "Network.h"
#include "json.hpp"

static Network network;

static void create_network(std::ifstream &fs)
{
    nlohmann::json data = nlohmann::json::parse(fs);
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        int numNodes = layers[i]["nodes"];
        int bias = layers[i]["bias"];
        network.Add(numNodes, bias);
    }
}

static void save_network(std::ofstream &fs)
{
    nlohmann::json data;
    nlohmann::json layers;
    for (size_t layerIx=0; layerIx<network.LayerCount(); layerIx++)
    {
        nlohmann::json layer;
        size_t nodeCount = network.GetNodeCount(layerIx);
        nlohmann::json nodes;
        for (size_t nodeIx=0; nodeIx<nodeCount; nodeIx++)
        {
            std::vector<double> weights;
            double bias;
            network.GetNodeWeights(layerIx, nodeIx, weights);
            network.GetNodeBias(layerIx, nodeIx, bias);
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
    fs << std::setw(4) << data << std::endl;
}

static void load_network(std::ifstream &fs)
{
    network.Clear();
    nlohmann::json data = nlohmann::json::parse(fs);
    auto layers = data["layers"];
    for (size_t i=0; i<layers.size(); i++)
    {
        auto layer = layers[i];
        size_t nodeCount = layer["count"];
        auto nodes = layer["nodes"];
        network.Add(nodeCount);
        for (size_t n=0; n<nodes.size(); n++)
        {
            auto node = nodes[n];
            double bias = node["bias"];
            std::vector<double> weights = node["weights"];
            for (size_t j=0; j<nodeCount; j++)
            {
                network.SetNodeBias(i, j, bias);
                network.SetNodeWeights(i, j, weights);
            }
        }
    }
}

static void show_outputs(std::vector<double> &outputs)
{
    std::cout << "Got outputs: " << outputs.size() << std::endl;
    for (double v : outputs)
    {
        std::cout << " " << v;
    }
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    if (argc <= 1)
    {
        std::cerr << "Usage: " << argv[0] << " <neural-network-config.json>" << std::endl;
        exit(1);
    }
    std::string inputName(argv[1]);
    std::ifstream fs(inputName, std::ifstream::in);
    if (!fs.is_open())
    {
        std::cerr << "Can't open config file " << argv[1] << std::endl;
        exit(2);
    }
    create_network(fs);
    fs.close();

    if (network.LayerCount() == 0)
    {
        std::cerr << "Error reading config file " << argv[1] << std::endl;
        exit(3);
    }

    std::vector<double> inputs(network.NumInputs(),0);

    char c;
    do
    {
        std::cout << std::endl;
        std::cout << "1. Evaluate Now" << std::endl;
        std::cout << "2. Change Input" << std::endl;
        std::cout << "s. Save Network" << std::endl;
        std::cout << "l. Load Network" << std::endl;
        std::cout << "q. Quit" << std::endl;
        std::cout << "> ";
        c = getchar();
        if (c == '1')
        {
            network.Measure(inputs);
            std::vector<double> &outputs = network.GetOutputs();
            show_outputs(outputs);
        }
        else if (c == '2')
        {
            inputs[0] += 0.3;
        }
        else if (c == 's' || c == 'S')
        {
            std::string saveFileName;
            std::cout << "Enter network save filename> ";
            std::cin >> saveFileName;
            if (saveFileName.size() > 0)
            {
                if (saveFileName.size() < 6 || 
                    saveFileName.substr(saveFileName.size()-5).compare(".json") != 0)
                {
                    saveFileName += ".json";
                }
                std::ofstream fs(saveFileName, std::ofstream::out);
                if (fs.is_open())
                {
                    save_network(fs);
                    fs.close();
                }
                else
                {
                    std::cerr << "Can't save network file '" << saveFileName << "'" << std::endl;
                }
            }
        }
        else if (c == 'l' || c == 'L')
        {
            std::string loadFileName;
            std::cout << "Enter network load filename> ";
            std::cin >> loadFileName;
            if (loadFileName.size() > 0)
            {
                if (loadFileName.size() < 6 || 
                    loadFileName.substr(loadFileName.size()-5).compare(".json") != 0)
                {
                    loadFileName += ".json";
                }
                std::ifstream fs(loadFileName, std::ifstream::in);
                if (fs.is_open())
                {
                    load_network(fs);
                    fs.close();
                }
                else
                {
                    std::cerr << "Can't open network file " << loadFileName << std::endl;
                }
            }
        }
    } while (c != 'q' && c != 'Q');

    return 0;
}