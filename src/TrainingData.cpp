#include <iostream>
#include <fstream>
#include <filesystem>
#include <stb/stb_image.h>
#include "TrainingData.h"
#include "json.hpp"

TrainingData::TrainingData()
{
    setIndex = 0;
    threshold = 1;
}

TrainingData::~TrainingData()
{
    dataFh.close();
}

bool TrainingData::GetFirstDataSet(DataSet &set, int outIx)
{
    setIndex = 0;
    if (inputType == "binary")
    {
        dataFh.seekg (0, dataFh.beg);
    }
    return GetNextDataSet(set, outIx);
}

bool TrainingData::GetNextDataSet(DataSet &set, int outIx)
{
    if (setIndex >= dataEntries)
        return false;
    if (outIx >= 0 && outIx > (int)outputSet.size())
        return false;
    if (inputType == "binary")
    {
        char *buf = new char[dataEntrySize];
        bool getNext = false;
        do
        {
            dataFh.read(buf, dataEntrySize);
            if (outIx >= 0)
            {
                std::string byteStr = std::to_string(buf[0]);
                getNext = (byteStr.compare(outputSet[outIx]) != 0);
                if (getNext)
                {
                    setIndex++;
                    if (setIndex >= dataEntries)
                    {
                        delete[] buf;
                        return false;
                    }
                }
            }
        }
        while (getNext);
        std::vector<unsigned char> inData(buf+1, buf + dataEntrySize);
        std::vector<double> inputEntry, outputEntry;
        if (!ParseDataInput(inData, inputEntry) ||
            !ParseDataOutput(buf[0], outputEntry))
        {
            delete[] buf;
            return false;
        }
        set = DataSet(inputEntry, outputEntry);
        delete[] buf;
    }
    else
    {
        bool getNext = false;
        do
        {
            set = dataSets[setIndex];
            if (outIx >= 0)
            {
                std::vector<double> expOut;
                if (!ParseDataOutput(outputSet[outIx], expOut))
                    return false;
                getNext = !std::equal(set.output.begin(), set.output.end(), expOut.begin());
                if (getNext)
                {
                    setIndex++;
                    if (setIndex >= dataEntries)
                    {
                        return false;
                    }
                }
            }
        } while(getNext);
    }
    setIndex++;
    return true;
}

bool TrainingData::GetDataSet(int index, DataSet &set)
{
    if (inputType == "binary")
    {
        if (index >= (int)dataEntries)
            return false;
        char *buf = new char[dataEntrySize];
        dataFh.seekg (dataEntrySize*index, dataFh.beg);
        dataFh.read(buf, dataEntrySize);
        std::vector<unsigned char> inData(buf+1, buf + dataEntrySize);
        std::vector<double> inputEntry, outputEntry;
        if (!ParseDataInput(inData, inputEntry) ||
            !ParseDataOutput(buf[0], outputEntry))
        {
            delete[] buf;
            return false;
        }
        set = DataSet(inputEntry, outputEntry);
        delete[] buf;
    }
    else
    {
        if (index >= (int)dataSets.size())
            return false;
        set = dataSets[index];
    }
    setIndex = index+1;
    return true;
}

bool TrainingData::OpenData(const std::string filename)
{
    std::ifstream fs(filename, std::ifstream::in);
    if (fs.is_open())
    {
        dataSets.clear();
        setIndex = 0;
        size_t pos = filename.find_last_of('/');
        if (pos != std::string::npos)
            dirPrefix = filename.substr(0, pos);
        else
            dirPrefix = "";
        nlohmann::json trainingInfo = nlohmann::json::parse(fs);
        inputCount = trainingInfo["inputs"];
        outputCount = trainingInfo["outputs"];
        auto input_params = trainingInfo["input_params"];
        auto output_params = trainingInfo["output_params"];
        inputType = input_params["type"];
        if (inputType == "image")
        {
            size_t rows = input_params["width"];
            size_t cols = input_params["height"];
            inputCount = rows * cols;
            threshold = input_params["threshold"];
        }
        else if (inputType == "decimal")
        {
        }
        else if (inputType == "binary")
        {
            dataFilename = input_params["file"];
            if (!dirPrefix.empty())
                dataFilename = dirPrefix + "/" + dataFilename;
            if (dataFh.is_open())
                dataFh.close();
            dataFh.open(dataFilename, std::ifstream::in | std::ifstream::binary);
            if (!dataFh.is_open())
            {
                std::cerr << "Unable to open binary file: '" << dataFilename << "'" << std::endl;
                return false;
            }
            dataFh.seekg (0, dataFh.end);
            int fileSize = dataFh.tellg();
            if (fileSize <= 0)
            {
                std::cerr << "Invalid size for binary file: '" << dataFilename << "': " << fileSize << std::endl;
                dataFh.close();
                return false;
            }
            dataFh.seekg (0, dataFh.beg);
            dataEntrySize = inputCount + 1;
            dataEntries = (size_t)fileSize / dataEntrySize;
        }
        else
        {
            std::cerr << "Unknown input type: '" << inputType << "'" << std::endl;
            return false;
        }
        auto set = output_params["set"];
        outputCount = set.size();
        for (auto out : set)
        {
            outputSet.push_back(out);
        }
        auto sets = trainingInfo["data"];
        if (sets.size() > 0)
        {
            for (size_t i=0; i<sets.size(); i++)
            {
                auto data = sets[i];
                std::vector<double> inputEntry, outputEntry;
                std::string inputStr = data["input"];
                std::string outputStr = data["output"];
                if (ParseDataInput(inputStr, inputEntry) &&
                    ParseDataOutput(outputStr, outputEntry))
                {
                    dataSets.emplace_back(inputEntry, outputEntry);
                }
                else
                {
                    std::cerr << "Failed to parse data!" << std::endl;
                    return false;
                }
            }
            dataEntries = dataSets.size();
        }
        std::cout << "Training Data Loaded:" << std::endl;
        std::cout << "       File: " << filename << std::endl;
        std::cout << "       Type: " << inputType << std::endl;
        std::cout << "    Samples: " << dataEntries << std::endl;
        std::cout << "     Inputs: " << inputCount << std::endl;
        std::cout << "    Outputs: " << outputCount << std::endl;
        if (pos == std::string::npos)
            m_trainingFilename = filename;
        else
            m_trainingFilename = filename.substr(pos+1);
        return true;
    }
    else
    {
        std::cerr << "Can't load training file '" << filename << "'" << std::endl;
    }
    return false;
}

bool TrainingData::ParseDataInput(std::string inputStr, std::vector<double> &input)
{
    input.clear();
    if (inputType == "image")
    {
        if (!dirPrefix.empty())
            inputStr = dirPrefix + "/" + inputStr;
        int x,y,comp;
        uint8_t* img = stbi_load(inputStr.c_str(), &x, &y, &comp, 0);
        if (img == NULL)
        {
            std::cerr << "Unable to read image file: '" << inputStr << "'" << std::endl;
            return false;
        }
        if ((x * y) != (int)inputCount)
        {
            std::cerr << "Input data set entry not correct size: '" << inputStr << "'" << std::endl;
            return false;
        }
        for (int i=0; i<x; i++)
        {
            for (int j=0; j<y; j++)
            {
                uint8_t v = *(img + (i*x) + j);
                if (v < threshold)
                    v = 0;
                input.push_back((v * 1.0) / 255);
            }
        }
        return true;
    }
    if (inputType == "decimal")
    {
        size_t pos1 = 0;
        while (pos1 < inputStr.size())
        {
            size_t pos2 = inputStr.find(' ', pos1);
            std::string s = inputStr.substr(pos1, pos2-pos1);
            uint8_t v = static_cast<uint8_t>(atol(s.c_str()));
            if (v < threshold)
                v = 0;
            input.push_back((v * 1.0) / 255);
            if (pos2 == std::string::npos)
                break;
            pos1 = pos2+1;
        }
        return true;
    }
    return false;
}

bool TrainingData::ParseDataInput(std::vector<unsigned char> inputBytes, std::vector<double> &input)
{
    input.clear();
    if (inputType == "binary")
    {
        for (unsigned char v : inputBytes)
        {
            if (v < threshold)
                v = 0;
            input.push_back((v * 1.0) / 255);
        }
        return true;
    }
    return false;
}

bool TrainingData::ParseDataOutput(std::string outputStr, std::vector<double> &output)
{
    output.clear();
    output.resize(outputCount, 0.0);
    for (size_t i=0; i<outputSet.size(); i++)
    {
        if (outputStr.compare(outputSet[i]) == 0)
        {
            output[i] = 1.0;
            return true;
        }
    }
    std::cerr << "Output data set does not contain: '" << outputStr << "'" << std::endl;
    return false;
}

bool TrainingData::ParseDataOutput(unsigned char byte, std::vector<double> &output)
{
    output.clear();
    output.resize(outputCount, 0.0);
    std::string byteStr = std::to_string(byte);
    for (size_t i=0; i<outputSet.size(); i++)
    {
        if (byteStr.compare(outputSet[i]) == 0)
        {
            output[i] = 1.0;
            return true;
        }
    }
    std::cerr << "Output data set does not contain: '" << byteStr << "'" << std::endl;
    return false;
}

void TrainingData::TrainingDataFiles(std::vector<std::string> &list, const std::string dir)
{
    list.clear();
    for (const auto & file : std::filesystem::directory_iterator(dir))
    {
        if (file.path().extension() == ".json")
        {
            list.push_back(file.path().filename());
        }
    }
}
