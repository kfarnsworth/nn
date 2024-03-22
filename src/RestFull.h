#pragma once

#include <cstdint>
#include "HttpConnection.h"
#include "Network.h"
#include "json.hpp"

class RestFull {
public:
    static constexpr uint16_t REST_PORT = 4569;

    RestFull(Network &network);
    ~RestFull();
    void Start();
    void Stop();
    void SetCommandParser(std::function<std::string(const std::string &)> handler);

private:
    Network &m_network;
    HttpConnection m_httpConnection;
    
    std::string RequestCallback(const std::string &request);
    int CommandInterpreter(const std::string &cmd, std::string &errStr, 
                           const nlohmann::json &data, nlohmann::json &info);
};
