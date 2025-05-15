#pragma once

#include <string>
#include <vector>

class DataSet {
public:
    DataSet() {};
    ~DataSet() {};

    DataSet(std::vector<double> &in, std::vector<double> &out)
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
    bool GetFirstDataSet(DataSet &set, int outIx=-1);
    bool GetNextDataSet(DataSet &set, int outIx=-1);
    size_t GetInputCount() { return inputCount; };
    size_t GetOutputCount() { return outputCount; };
    bool GetDataSet(int index, DataSet &set);
    void Clear() { dataSets.clear(); outputSet.clear(); dataEntries=0; dataFh.close(); };
    bool IsEmpty() { return dataEntries == 0; }
    static void TrainingDataFiles(std::vector<std::string> &list, const std::string dir=DEFAULT_TRAINING_DIRECTORY);
    static const char *TrainingDirectory() { return DEFAULT_TRAINING_DIRECTORY; };
    std::string GetTrainingFilename() { return m_trainingFilename; }
    std::vector<std::string> GetOutputParamsSet();

private:
    std::string dirPrefix;
    size_t setIndex;
    size_t inputCount;
    std::string dataFilename;
    std::ifstream dataFh;
    size_t dataEntrySize;
    size_t dataEntries;
    int threshold;
    bool inverted;
    std::string inputType;
    size_t outputCount;
    std::vector<std::string> outputSet;
    std::vector<DataSet> dataSets;
    std::string m_trainingFilename;

    static constexpr char DEFAULT_TRAINING_DIRECTORY[] = "training";

    bool ParseDataInput(std::string inputStr, std::vector<double> &input);
    bool ParseDataInput(std::vector<unsigned char> inputBytes, std::vector<double> &input);
    bool ParseDataOutput(std::string outputStr, std::vector<double> &output);
    bool ParseDataOutput(unsigned char byte, std::vector<double> &output);
};
