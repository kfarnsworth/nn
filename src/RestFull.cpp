#include <exception>
#include "RestFull.h"

RestFull::RestFull(Network &network)
    : m_network(network), m_httpConnection("/nn", REST_PORT)
{
}

RestFull::~RestFull()
{
}

std::string RestFull::RequestCallback(const std::string &request)
{
    std::string cmd;
    nlohmann::json data;
    nlohmann::json info;
    int err;
    std::string errStr;

    try {
        nlohmann::json requestData = nlohmann::json::parse(request);
        cmd = requestData["cmd"];
        data = requestData["data"];
        err = CommandInterpreter(cmd, errStr, data, info);
    } catch(std::exception& ex) {
        err = -1;
        errStr = "Exception:" + std::string(ex.what());
    }

    nlohmann::json returnData;
    nlohmann::json response;
    returnData["cmd"] = cmd;
    response["result"] = err;
    response["error"] = errStr;
    response["info"] = info;
    returnData["result"] = response;
    return returnData.dump();
}

void RestFull::Start()
{
    m_httpConnection.Start();
    auto myCallback = std::bind(&RestFull::RequestCallback, this, std::placeholders::_1);
    m_httpConnection.SetHandler(myCallback);
}

void RestFull::Stop()
{
    m_httpConnection.SetHandler(nullptr);
    m_httpConnection.Stop();
}

int RestFull::CommandInterpreter(const std::string &cmd, std::string &errStr, 
    const nlohmann::json &data, nlohmann::json &info)
{
    if (!cmd.compare("getNetwork"))
    {
        info["inputs"] = m_network.NumInputs();
        info["outputs"] = m_network.NumOutputs();
        nlohmann::json layers;
        for (int layerIx=0; layerIx<m_network.LayerCount(); layerIx++)
        {
            nlohmann::json layer;
            layer["nodes"] = m_network.GetNodeCount(layerIx);
            layers.push_back(layer);
        }
        info["layers"] = layers;
        errStr = "ok";
        return 0;
    }
    if (!cmd.compare("setNetwork"))
    {
        if (m_network.IsTraining())
        {
            errStr = "Network busy training!";
            return -1;
        }
        if (!data.is_object())
        {
            errStr = "data object is missing";
            return -1;
        }
        if (!data.contains("inputs") || !data["inputs"].is_number())
        {
            errStr = "number of inputs missing";
            return -1;
        }
        if (!data.contains("outputs") || !data["outputs"].is_number())
        {
            errStr = "number of outputs missing";
            return -1;
        }
        int inputs = data["inputs"];
        int outputs = data["outputs"];
        if (!data.contains("layers") || !data["layers"].is_array())
        {
            errStr = "layer array missing or incorrect";
            return -1;
        }
        auto layers = data["layers"];
        if (layers.size() == 0)
        {
            errStr = "no layers defined";
            return -1;
        }
        m_network.Clear();
        for (size_t i=0; i<layers.size(); i++)
        {
            auto layer = layers[i];
            if (!layer.contains("nodes") || !layer["nodes"].is_number() ||
                !layer.contains("defaultBias") || !layer["defaultBias"].is_number())
            {
                errStr = "invalid layer info";
                return -1;
            }
            int nodeCount = layer["nodes"];
            int defaultBias = layer["defaultBias"];
            m_network.Add(nodeCount, inputs, defaultBias);
            inputs = nodeCount; // full mesh: outputs of previous layer is num inputs on next layer
        }
        if (m_network.NumOutputs() != (size_t)outputs)
        {
            errStr = "warning: number of declared outputs does not equal numbers of nodes in last layer";
        }
        else
        {
            errStr = "ok";
        }
        return 0;
    }
    if (!cmd.compare("getMeasurement"))
    {
        if (m_network.IsTraining())
        {
            errStr = "Network busy training!";
            return -1;
        }
        if (!data.is_object())
        {
            errStr = "data object is missing";
            return -1;
        }
        if (!data.contains("input") || !data["input"].is_array())
        {
            errStr = "input array missing or invalid";
            return -1;
        }
        auto input = data["input"];
        if (input.size() != m_network.NumInputs())
        {
            errStr = "input array size does not match network inputs size";
            return -1;
        }
        double divisor = 1.0;
        if (data.contains("divisor"))
        {
            if (!data["divisor"].is_number())
            {
                errStr = "divisor input must be a number";
                return -1;
            }
            divisor = data["divisor"];
        }
        std::vector<double> inputVector;
        for (size_t i=0; i<input.size(); i++)
        {
            if (!input[i].is_number())
            {
                errStr = "input is not a number";
                return -1;
            }
            double value = input[i];
            inputVector.push_back(value / divisor);
        }
        m_network.Measure(inputVector);
        std::vector<double> outputVector = m_network.GetOutputs();
        nlohmann::json output;
        info["output"] = outputVector;
        errStr = "ok";
        return 0;
    }
    errStr = "Unknown command";
    return -1;

}

