
#include "RestFull.h"

RestFull::RestFull()
    : m_httpConnection("/nn", REST_PORT)
{

}

RestFull::~RestFull()
{
}

void RestFull::Start()
{
    m_httpConnection.Start();
}

void RestFull::Stop()
{
    m_httpConnection.Stop();
}
