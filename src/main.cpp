#include <iostream>
#include <iomanip>
#include <fstream>
#include "Network.h"
#include "TrainingData.h"

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
    network.CreateNetwork(fs);
    fs.close();

    if (network.LayerCount() == 0)
    {
        std::cerr << "Error reading config file " << argv[1] << std::endl;
        exit(3);
    }

    char c;
    do
    {
        std::cout << std::endl;
        std::cout << "e. Evaluate Data Set" << std::endl;
        std::cout << "s. Save Network" << std::endl;
        std::cout << "l. Load Network" << std::endl;
        std::cout << "t. Load Training Set" << std::endl;
        std::cout << "q. Quit" << std::endl;
        std::cout << "> ";
        while ((c = getchar()) == '\n' || c == EOF);
        if (c == 'e' || c == 'E')
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
                        std::cout << "Training data set empty!" << std::endl;
                    else
                        std::cout << "Reset to first data set" << std::endl;
                }
                if (dataSet.input.size() > 0)
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
            std::string trainingFileName;
            std::cout << "Enter training data set filename> ";
            std::cin >> trainingFileName;
            if (trainingFileName.size() > 0)
            {
                if (trainingFileName.size() < 6 || 
                    trainingFileName.substr(trainingFileName.size()-5).compare(".json") != 0)
                {
                    trainingFileName += ".json";
                }
                trainingData.OpenData(trainingFileName);
                if (trainingData.GetInputCount() != network.NumInputs() ||
                    trainingData.GetOutputCount() != network.NumOutputs())
                {
                    std::cout << "NOTE: Training data set does not match network: "
                              << "network(in:" << network.NumInputs() << ",out:" << network.NumOutputs()
                              << ") dataset(in:" << trainingData.GetInputCount() << ",out:"
                              << trainingData.GetOutputCount() << ")" << std::endl;
                }
            }
        }
    } while (c != 'q' && c != 'Q');

    return 0;
}