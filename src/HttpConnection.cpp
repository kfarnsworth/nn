
#include <string>
#include "HttpConnection.h"
#include "httplib.h"

static httplib::Server m_server;

HttpConnection::HttpConnection(const char *path, uint16_t port) : m_port(port), m_handler(nullptr)
{
    m_server.Post(path, [&](const httplib::Request& req, httplib::Response& res) {
        if (m_handler != nullptr)
        {
            std::string responseStr = m_handler(req.body);
            res.set_content(responseStr.c_str(), "application/json");
        }
        else
        {
            res.set_content("{ \"result\":-1, \"error\":\"no callback\"}", "application/json");
        }
    });
}

HttpConnection::~HttpConnection()
{
    Stop();
}
void HttpConnection::SetHandler(std::function<std::string(const std::string &)> handler)
{
    m_handler = handler;
}

void HttpConnection::HttpThread(void *data)
{
    (void)data;
    m_server.listen("0.0.0.0", m_port);
}

void HttpConnection::Start()
{
    m_httpThread = std::thread(&HttpConnection::HttpThread, this, nullptr);
}

void HttpConnection::Stop()
{
    if (m_httpThread.joinable())
    {
        m_server.stop();
        m_httpThread.detach();
    }
}
