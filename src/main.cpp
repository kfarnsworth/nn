#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include "Network.h"
#include "TrainingData.h"
#include "StochasticGradientDecent.h"

static Network network;
static TrainingData trainingData;

static void show_outputs(std::vector<double> &outputs, std::vector<double> &expOutputs)
{
    std::cout << "Output: " << outputs.size() << std::endl;
    for (size_t i=0; i<outputs.size(); i++)
    {
        std::cout <<  std::setw(8) << std::setprecision(4) << std::fixed << outputs[i];
    }
        std::cout << " | Expected: ";
    for (size_t i=0; i<expOutputs.size(); i++)
    {
        std::cout <<  std::setw(8) << std::setprecision(4) << std::fixed << expOutputs[i];
    }
    std::cout << std::endl;
}

static void load_training(std::string trainingFileName)
{
    if (trainingFileName.size() > 0)
    {
        if (trainingFileName.size() < 6 || 
            trainingFileName.substr(trainingFileName.size()-5).compare(".json") != 0)
        {
            trainingFileName += ".json";
        }
        if (trainingData.OpenData(trainingFileName) && 
            (trainingData.GetInputCount() != network.NumInputs() ||
             trainingData.GetOutputCount() != network.NumOutputs()))
        {
            std::cout << "NOTE: Training data set does not match network: "
                        << "network(in:" << network.NumInputs() << ",out:" << network.NumOutputs()
                        << ") dataset(in:" << trainingData.GetInputCount() << ",out:"
                        << trainingData.GetOutputCount() << ")" << std::endl;
        }
    }
    else
    {
        std::cout << "NOTE: Invalid training file \"" << trainingFileName << "\"" << std::endl;
    }
}

static void show_usage(const char *s)
{
    std::cerr << "Usage: " << s << " <neural-network-config> [options]" << std::endl;
    std::cerr << "  <neural-network-config> - network configuration (required)" << std::endl;
    std::cerr << "  options:" << std::endl;
    std::cerr << "    -t|--training <training-file> - training file input" << std::endl;
    std::cerr << "    -b|--batchcount <int> - number of batches to train with" << std::endl;
    std::cerr << "    -n|--batchsize <int> - size of each batch" << std::endl;
    std::cerr << "    -r|--learningrate <int> - learning rate between 1 and batchsize" << std::endl;
    std::cerr << "    -h|--help - this output" << std::endl;
}

// Terms:
//  - stochastic gradient decsent : run small batches of traning before calculating total cost
//
int main(int argc, const char *argv[])
{
    bool failout = false;
    int i = 1;
    std::string trainingFilename;
    std::string configFilename;
    int trainingBatchSize = 20;
    int trainingMaxBatches = 0;
    int learningRate = 1;

    while (i < argc)
    {
        if (!std::strcmp(argv[i], "-h") || !std::strcmp(argv[i], "--help"))
        {
            failout = true;
            break;    
        }
        else if (!std::strcmp(argv[i], "-t") || !std::strcmp(argv[i], "--training"))
        {
            i++;
            if (i >= argc)
            {
                std::cerr << "ERROR:Missing training file" << std::endl;    
                failout = true;
                break;    
            }
            trainingFilename = std::string(argv[i]);
        }
        else if (!std::strcmp(argv[i], "-b") || !std::strcmp(argv[i], "--batchcount"))
        {
            i++;
            if (i >= argc)
            {
                std::cerr << "ERROR:Missing batch count" << std::endl;
                failout = true;
                break;    
            }
            try {
                trainingMaxBatches = std::stol(argv[i]);
            } catch(...) {
                std::cerr << "ERROR:Invalid batch count" << std::endl;
                failout = true;
                break;    
            }
        }
        else if (!std::strcmp(argv[i], "-n") || !std::strcmp(argv[i], "--batchsize"))
        {
            i++;
            if (i >= argc)
            {
                std::cerr << "ERROR:Missing batch size" << std::endl;
                failout = true;
                break;    
            }
            try {
                trainingBatchSize = std::stol(argv[i]);
            } catch(...) {
                std::cerr << "ERROR:Invalid batch size" << std::endl;
                failout = true;
                break;    
            }
        }
        else if (!std::strcmp(argv[i], "-r") || !std::strcmp(argv[i], "--learningrate"))
        {
            i++;
            if (i >= argc)
            {
                std::cerr << "ERROR:Missing learning rate" << std::endl;
                failout = true;
                break;    
            }
            try {
                learningRate = std::stol(argv[i]);
            } catch(...) {
                std::cerr << "ERROR:Invalid learning rate" << std::endl;
                failout = true;
                break;    
            }
        }
        else if (argv[i][0] == '-')
        {
            std::cerr << "ERROR: Unknown option \"" << argv[i] << "\"" << std::endl;
            failout = true;
            break;
        }
        else
        {
            configFilename = std::string(argv[i]);
        }
        i++;
    }
    if (failout || configFilename.empty())
    {
        show_usage(argv[0]);
        exit(1);
    }

    std::ifstream fs(configFilename, std::ifstream::in);
    if (!fs.is_open())
    {
        std::cerr << "Can't open config file " << configFilename << std::endl;
        exit(2);
    }
    network.CreateNetwork(fs);
    fs.close();

    if (network.LayerCount() == 0)
    {
        std::cerr << "Error reading config file " << configFilename << std::endl;
        exit(3);
    }

    if (!trainingFilename.empty())
    {
        load_training(trainingFilename);
    }

    char c;
    do
    {
        std::cout << std::endl;
        std::cout << "e. Evaluate Data Set" << std::endl;
        std::cout << "a. Train SGD" << std::endl;
        std::cout << "s. Save Network" << std::endl;
        std::cout << "l. Load Network" << std::endl;
        std::cout << "t. Load Training Set" << std::endl;
        std::cout << "q. Quit" << std::endl;
        std::cout << "> ";
        while ((c = getchar()) == '\n' || c == EOF);
        if (c == 'a' || c == 'A')
        {
            StochasticGradientDecent sgd(learningRate);
            sgd.Train(network, trainingData, trainingBatchSize, trainingMaxBatches);
        }
        else if (c == 'e' || c == 'E')
        {
            if (trainingData.GetInputCount() == 0 || trainingData.GetInputCount() != network.NumInputs())
            {
                std::cout << "Input training data set does not match network inputs" << std::endl;
            }
            else if (trainingData.GetOutputCount() == 0 || trainingData.GetOutputCount() != network.NumOutputs())
            {
                std::cout << "Output training data set does not match network outputs" << std::endl;
            }
            else
            {
                DataSet dataSet;
                if (!trainingData.GetNextDataSet(dataSet))
                {
                    if (!trainingData.GetFirstDataSet(dataSet))
                    {
                        std::cout << "Training data set empty!" << std::endl;
                        break;
                    }
                    else
                    {
                        std::cout << "Reset to first data set" << std::endl;
                    }
                }
                if (dataSet.input.size() != 0)
                {
                    network.Measure(dataSet.input);
                    std::vector<double> &outputs = network.GetOutputs();
                    show_outputs(outputs, dataSet.output);
                }
            }
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
                    network.SaveNetwork(fs);
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
                    network.LoadNetwork(fs);
                    fs.close();
                }
                else
                {
                    std::cerr << "Can't open network file " << loadFileName << std::endl;
                }
            }
        }
        else if (c == 't' || c == 'T')
        {
            std::cout << "Enter training data set filename> ";
            std::cin >> trainingFilename;
            load_training(trainingFilename);
        }
    } while (c != 'q' && c != 'Q');

    return 0;
}