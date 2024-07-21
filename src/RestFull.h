#pragma once

#include <cstdint>
#include "HttpConnection.h"
#include "Network.h"
#include "Training.h"
#include "TrainingData.h"
#include "json.hpp"

class RestFull {
public:
    static constexpr uint16_t REST_PORT = 4569;

    RestFull(Network &network, TrainingData &trainingData);
    ~RestFull();
    void Start();
    void Stop();
    void SetCommandParser(std::function<std::string(const std::string &)> handler);

private:
    Network &m_network;
    TrainingData &m_trainingData;
    Training::TrainerPtr m_trainer;
    HttpConnection m_httpConnection;

    std::string RequestCallback(const std::string &request);
    int CommandInterpreter(const std::string &cmd, std::string &errStr,
                           const nlohmann::json &data, nlohmann::json &info);
};
