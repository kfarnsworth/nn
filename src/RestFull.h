#pragma once

#include <cstdint>
#include "HttpConnection.h"

class RestFull {
public:
    static constexpr uint16_t REST_PORT = 4569;

    RestFull();
    ~RestFull();
    void Start();
    void Stop();

private:
    HttpConnection m_httpConnection;
};