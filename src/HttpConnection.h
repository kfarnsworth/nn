#pragma once

#include <functional>
#include <thread>

class HttpConnection {
public:
    HttpConnection(const char *path="/nn", uint16_t port=5511);
    ~HttpConnection();
    void SetHandler(std::function<std::string(const std::string &)> handler);
    void Start();
    void Stop();

private:
    uint16_t m_port;
    std::thread m_httpThread;
    std::function<std::string(const std::string &)> m_handler;

    void HttpThread(void *);
};
