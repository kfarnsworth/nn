#include <exception>
#include "RestFull.h"

RestFull::RestFull(Network &network, TrainingData &trainingData)
    : m_network(network), m_trainingData(trainingData),
      m_trainer(nullptr), m_httpConnection("/nn", REST_PORT)
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
    returnData["response"] = response;
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
    if (!cmd.compare("getStatus"))
    {
        info["networkLoaded"] = m_network.LayerCount() > 0;
        info["isTraining"] = m_network.IsTraining();
        nlohmann::json inputs;
        std::vector<double> networkInputs;
        m_network.GetInputState(networkInputs);
        info["inputs"] = networkInputs;
        nlohmann::json activations;
        for (int layerIx=0; layerIx<m_network.LayerCount(); layerIx++)
        {
            nlohmann::json activationLayer;
            std::vector<double> layerActivations;
            m_network.GetOutputState(layerIx, layerActivations);
            activationLayer = layerActivations;
            activations.push_back(activationLayer);
        }
        info["activations"] = activations;
        std::vector<std::string> trainingFiles;
        TrainingData::TrainingDataFiles(trainingFiles);
        info["trainingFiles"] = trainingFiles;
        std::vector<std::string> trainingTypes;
        Training::TrainingDataTypes(trainingTypes);
        info["trainingTypes"] = trainingTypes;
        std::vector<std::string> networkFiles;
        Network::NetworkFiles(networkFiles);
        info["networkFiles"] = networkFiles;
        nlohmann::json trainingSettings;
        trainingSettings["file"] = m_trainingData.GetTrainingFilename();
        trainingSettings["type"] = m_trainer ? m_trainer->GetType() : "";
        trainingSettings["rate"] = m_trainer ? m_trainer->GetLearningRate() : Training::LEARNING_RATE_DEFAULT;
        trainingSettings["momentum"] = m_trainer ? m_trainer->GetMomentum() : Training::MOMENTUM_DEFAULT;
        trainingSettings["batchSize"] = m_trainer ? m_trainer->GetBatchSize() : Training::BATCH_SIZE_DEFAULT;
        trainingSettings["batchCount"] = m_trainer ? m_trainer->GetBatchCount() : Training::BATCH_COUNT_DEFAULT;
        info["trainingSettings"] = trainingSettings;
        errStr = "ok";
        return 0;
    }
    if (!cmd.compare("getNetwork") || !cmd.compare("loadNetwork"))
    {
        if (!cmd.compare("loadNetwork"))
        {
            if (!data.is_object())
            {
                errStr = "data object is missing";
                return -1;
            }
            if (!data.contains("filename") || !data["filename"].is_string())
            {
                errStr = "filename missing";
                return -1;
            }
            std::string loadFileName(Network::NetworkDirectory());
            loadFileName += "/";
            loadFileName += data["filename"];
            std::ifstream fs(loadFileName, std::ifstream::in);
            if (!fs.is_open())
            {
                errStr = "filename can't be opened";
                return -1;
            }
            m_network.LoadNetwork(fs);
            fs.close();
        }
        else
        {
            if (m_network.LayerCount() == 0)
            {
                errStr = "network not set";
                return -1;
            }
        }
        info["inputs"] = m_network.NumInputs();
        info["outputs"] = m_network.NumOutputs();
        nlohmann::json layers;
        for (int layerIx=0; layerIx<m_network.LayerCount(); layerIx++)
        {
            nlohmann::json layer;
            nlohmann::json nodes;
            nlohmann::json node;
            std::vector<double> biases;
            std::vector<std::vector<double>> weightsPerNode;
            m_network.GetBiasState(layerIx, biases);
            m_network.GetWeightsState(layerIx, weightsPerNode);
            for (size_t nodeIx=0; nodeIx<biases.size(); nodeIx++)
            {
                node["bias"] = biases[nodeIx];
                node["weights"] = weightsPerNode[nodeIx];
                nodes.push_back(node);
            }
            layer["nodes"] = nodes;
            layers.push_back(layer);
        }
        info["layers"] = layers;
        errStr = "ok";
        return 0;
    }
    if (!cmd.compare("saveNetwork"))
    {
        if (m_network.LayerCount() == 0)
        {
            errStr = "network is empty";
            return -1;
        }
        if (!data.is_object())
        {
            errStr = "data object is missing";
            return -1;
        }
        if (!data.contains("filename") || !data["filename"].is_string())
        {
            errStr = "filename missing";
            return -1;
        }
        std::string saveFileName(data["filename"]);
        if (saveFileName.empty())
        {
            errStr = "filename invalid";
            return -1;
        }
        if (saveFileName.size() < 6 ||
            saveFileName.substr(saveFileName.size()-5).compare(".json") != 0)
        {
            saveFileName += ".json";
        }
        saveFileName.insert(0, "/");
        saveFileName.insert(0, Network::NetworkDirectory());
        std::ofstream fs(saveFileName, std::ofstream::out);
        if (fs.is_open())
        {
            m_network.SaveNetwork(fs);
            fs.close();
        }
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
        for (size_t i=0; i<layers.size(); i++)
        {
            auto layer = layers[i];
            if (!layer.contains("nodes") || !layer["nodes"].is_array())
            {
                errStr = "invalid layer info";
                return -1;
            }
            auto nodes = layer["nodes"];
            int prevNodeCnt = -1;
            for (size_t j=0; j<nodes.size(); j++)
            {
                auto node = nodes[j];
                if (!node.contains("weights") || !node["weights"].is_array() ||
                    !node.contains("bias") || !node["bias"].is_number())
                {
                    errStr = "invalid node info";
                    return -1;
                }
                auto weights = node["weights"];
                int numInputs = weights.size();
                if (i == 0 && inputs != numInputs)
                {
                    errStr = "first node has non-matching input counts";
                    return -1;
                }
                // full mesh: outputs of previous layer is num inputs on next layer
                else if (j > 0 && prevNodeCnt != numInputs)
                {
                    errStr = "node input count not matching previous node count";
                    return -1;
                }
                prevNodeCnt = numInputs;
            }
            if (i == layers.size() - 1 && outputs != (int)nodes.size())
            {
                errStr = "node count in last layer does not match outputs";
                return -1;
            }
        }
        m_network.Clear();
        int numInputs = inputs;
        for (size_t i=0; i<layers.size(); i++)
        {
            auto layer = layers[i];
            auto nodes = layer["nodes"];
            int numNodes = nodes.size();
            m_network.Add(numNodes, numInputs);
            numInputs = numNodes;
            for (size_t j=0; j<nodes.size(); j++)
            {
                auto node = nodes[j];
                auto weights = node["weights"];
                int bias = node["bias"];
                m_network.SetNodeWeights(i, j, weights);
                m_network.SetNodeBias(i, j, bias);
            }
        }
        errStr = "ok";
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
        info["output"] = outputVector;
        errStr = "ok";
        return 0;
    }
    if (!cmd.compare("startTraining"))
    {
        if (m_network.IsTraining())
        {
            errStr = "Network already training!";
            return -1;
        }
        if (!data.is_object())
        {
            errStr = "data object is missing";
            return -1;
        }
        if (!data.contains("filename") || !data["filename"].is_string())
        {
            errStr = "filename missing or invalid";
            return -1;
        }
        std::string trainingFileName(data["filename"]);
        if (trainingFileName.size() < 6 ||
            trainingFileName.substr(trainingFileName.size()-5).compare(".json") != 0)
        {
            trainingFileName += ".json";
        }
        trainingFileName.insert(0, "/");
        trainingFileName.insert(0, TrainingData::TrainingDirectory());
        if (m_trainingData.OpenData(trainingFileName))
        {
            if (m_trainingData.GetInputCount() != m_network.NumInputs() ||
                m_trainingData.GetOutputCount() != m_network.NumOutputs())
            {
                errStr = "Training data set does not match network!";
                return -1;
            }
        }
        else
        {
            errStr = "Unable to load training data";
            return -1;
        }
        if (!data.contains("type") || !data["type"].is_string())
        {
            errStr = "type missing or invalid";
            return -1;
        }
        auto type = data["type"];
        if (!data.contains("rate") || !data["rate"].is_number())
        {
            errStr = "rate missing or invalid";
            return -1;
        }
        auto rate = data["rate"];
        if (!data.contains("momentum") || !data["momentum"].is_number())
        {
            errStr = "momentum missing or invalid";
            return -1;
        }
        auto momentum = data["momentum"];
        if (!data.contains("batchSize") || !data["batchSize"].is_number())
        {
            errStr = "batchSize missing or invalid";
            return -1;
        }
        auto batchSize = data["batchSize"];
        if (!data.contains("batchCount") || !data["batchCount"].is_number())
        {
            errStr = "batchCount missing or invalid";
            return -1;
        }
        auto batchCount = data["batchCount"];
        if (!data.contains("outputType") || !data["outputType"].is_number())
        {
            errStr = "outputType missing or invalid";
            return -1;
        }
        auto outputType = data["outputType"];
        m_trainer = Training::GetTrainer(m_network, type);
        m_trainer->SetLearningRate(rate);
        m_trainer->SetMomentum(momentum);
        m_trainer->Train(m_trainingData, batchSize, batchCount, outputType);
        info["set"] = m_trainingData.GetOutputParamsSet();
        errStr = "training started";
        return 0;
    }
    if (!cmd.compare("stopTraining"))
    {
        if (!m_network.IsTraining())
        {
            errStr = "Network is not training!";
            return -1;
        }
        m_trainer->Stop();
        errStr = "Training stopped";
        return 0;
    }
    if (!cmd.compare("getTrainingState"))
    {
        info["isTraining"] = m_network.IsTraining();
        std::vector<double> networkInputs;
        m_network.GetInputState(networkInputs);
        info["inputs"] = networkInputs;
        nlohmann::json activations;
        for (int layerIx=0; layerIx<m_network.LayerCount(); layerIx++)
        {
            nlohmann::json activationLayer;
            std::vector<double> layerActivations;
            m_network.GetOutputState(layerIx, layerActivations);
            activationLayer = layerActivations;
            activations.push_back(activationLayer);
        }
        info["activations"] = activations;
        int batchCount, batchTotal, totalTime;
        m_trainer->TrainingProgress(batchCount, batchTotal, totalTime);
        info["batchCount"] = batchCount;
        info["batchTotal"] = batchTotal;
        info["totalTime"] = totalTime;
        errStr = "ok";
        return 0;
    }
    errStr = "Unknown command";
    return -1;

}
