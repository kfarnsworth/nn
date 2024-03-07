#pragma once

#include <string>
#include <vector>

class DataSet {
public:
    DataSet() {};
    ~DataSet() {};
    
    DataSet(std::vector<double> &in, std::vector<double> out)
    { 
        input = in;
        output = out;
    }
    std::vector<double> input;
    std::vector<double> output;
};

class TrainingData {
public:
    TrainingData();
    ~TrainingData();
    bool OpenData(const std::string filename);
    bool GetFirstDataSet(DataSet &set);
    bool GetNextDataSet(DataSet &set);
    size_t GetInputCount() { return inputCount; };
    size_t GetOutputCount() { return outputCount; };

private:
    std::string dirPrefix;
    size_t setIndex;
    size_t inputCount;
    std::string dataFilename;
    std::ifstream dataFh;
    size_t dataEntrySize;
    size_t dataEntries;
    int threshold;
    std::string inputType;
    size_t outputCount;
    std::vector<std::string> outputSet;
    std::vector<DataSet> dataSets;

    bool ParseDataInput(std::string inputStr, std::vector<double> &input);
    bool ParseDataInput(std::vector<unsigned char> inputBytes, std::vector<double> &input);
    bool ParseDataOutput(std::string outputStr, std::vector<double> &output);
    bool ParseDataOutput(unsigned char byte, std::vector<double> &output);
};