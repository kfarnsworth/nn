
#include <thread>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sched.h>

class Threader {
  public:
    Threader(size_t maxThreads) : m_numThreads(0), m_maxThreads(maxThreads)
    {
        m_results = std::vector<double>(maxThreads, 0.0);
        m_threads.resize(maxThreads);
        m_numCores = std::thread::hardware_concurrency();
        m_nextCore = 0;
    }

    void start(size_t ix,
               std::function<double(const std::vector<double> &)> func,
               const std::vector<double> &inputs)
    {
        if (ix != m_numThreads) throw std::runtime_error("threader start index not the next expected count");
        m_numThreads++;
        if (m_numThreads > m_maxThreads) throw std::runtime_error("threader start called too many times");
        m_threads[ix]  = std::thread([this, ix, func, inputs] {
            SetAffinity();
            m_results[ix] = func(inputs);
        });
    }

    void join()
    {
        for (auto &thread: m_threads) {
            thread.join();
        }
    }

    double result(size_t ix)
    {
        if (ix > m_numThreads) std::runtime_error("threader result index out of range");
        return m_results[ix];
    }

  private:
    unsigned int m_numCores;
    unsigned int m_nextCore;
    size_t  m_numThreads;
    size_t  m_maxThreads;
    std::vector<std::thread> m_threads;
    std::vector<double> m_results;

    void SetAffinity()
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(m_nextCore, &cpuset); // Bind to CPU core
        pthread_t current_thread = pthread_self();
        if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0) {
            std::cerr << "Failed to set thread affinity for core " << (int)m_nextCore << std::endl;
        }
        m_nextCore = (m_nextCore+1) % m_numCores;
    }
};
