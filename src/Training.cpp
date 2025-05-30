#include <iostream>
#include <ctime>
#include <thread>
#include "Training.h"
#include "StochasticGradientDecent.h"

std::map<std::string, std::shared_ptr<Training>> Training::m_trainers;

Training::Training(Network &network) : m_learningRate(LEARNING_RATE_DEFAULT),
                                       m_momentum(MOMENTUM_DEFAULT),
                                       m_network(network),
                                       m_batchSize(BATCH_SIZE_DEFAULT),
                                       m_numBatches(BATCH_COUNT_DEFAULT)
{

}

Training::~Training()
{
    StopThread();
}

std::shared_ptr<Training> Training::GetTrainer(Network &network, std::string type)
{
    if (m_trainers.find(type) != m_trainers.end())
        return m_trainers[type];

    if (type == "SGD")
    {
        m_trainers[type] = std::make_shared<StochasticGradientDecent>(network);
        return m_trainers[type];
    }
    return nullptr;
}

void Training::TrainingDataTypes(std::vector<std::string> &list)
{
    list.clear();
    list.push_back("SGD");
}

void Training::TrainingProgress(int &batchCount, int &batchTotal, int &totalTime)
{
    batchCount = m_batchCount;
    batchTotal = m_numBatches;
    totalTime = m_totalTimeSecs;
}

void Training::WaitComplete()
{
    m_trainingThread.join();
}

void Training::Train(TrainingData &trainingData, int batchSize, int numBatches, int trainingOuputIndex)
{
    m_testTraining = false;
    m_batchSize = batchSize;
    m_numBatches = numBatches;
    m_trainingOutputIndex = trainingOuputIndex;
    m_complete = false;
    StartThread(trainingData);
}

void Training::TrainTest(TrainingData &trainingData, int dataSetIndex, int batchSize, int numBatches)
{
    m_testTraining = true;
    m_testDataSetIndex = dataSetIndex;
    m_trainingOutputIndex = -1;
    m_batchSize = batchSize;
    m_numBatches = numBatches;
    m_complete = false;
    StartThread(trainingData);
}

void Training::StartThread(TrainingData &trainingData)
{
    StopThread();
    m_trainingThread = std::thread(&Training::TrainingThread, this, &trainingData);
}

void Training::StopThread()
{
    if (m_trainingThread.joinable())
    {
        m_complete = true;
        m_trainingThread.detach();
    }
}

void Training::TrainingThread(void *data)
{
    TrainingData *t = (TrainingData *)data;

    m_batchCount = 0;
    int setCount = 0;
    std::vector<DataSet> dataSets;
    m_complete = false;

    dataSets.resize(m_batchSize);
    if (!m_testTraining)
    {
        if (!t->GetFirstDataSet(dataSets[setCount], m_trainingOutputIndex))
        {
            std::cerr << "ERROR: training data input" << std::endl;
            m_complete = true;
        }
        m_network.SetTrainingState(true);
        setCount++;
    }
    else
    {
        for (int i=0; i<m_batchSize; i++)
        {
            if (!t->GetDataSet(m_testDataSetIndex, dataSets[i]))
            {
                std::cerr << "ERROR: training data input" << std::endl;
                m_complete = true;
                break;
            }
        }
    }

    m_totalTimeSecs = 0;
    while (!m_complete)
    {
        StartTimer();
        if (!m_testTraining)
        {
            setCount = 0;
            while (setCount < m_batchSize)
            {
                if (!t->GetNextDataSet(dataSets[setCount], m_trainingOutputIndex))
                {
                    if (!t->GetFirstDataSet(dataSets[setCount], m_trainingOutputIndex))
                    {
                        std::cerr << "ERROR: training data no more" << std::endl;
                        m_complete = true;
                        break;
                    }
                }
                setCount++;
            }
        }

        TrainBatch(m_network, dataSets);

        m_batchCount++;
        StopTimer();
        m_totalTimeSecs = (int)(GetTotalTimeMsecs() / 1000);

        if (m_numBatches != 0)
        {
            std::cout << "Batch " << m_batchCount << " of " << m_numBatches << " complete.         \r" << std::flush;
            if (m_batchCount >= m_numBatches)
            {
                m_complete = true;
            }
        }
        else
        {
            std::cout << "Batch " << m_batchCount << "complete.         \r" << std::flush;
        }
    }

    std::cout << std::endl;
    std::cout << "Completed in " << std::fixed << std::setprecision(3) << GetTotalTimeMsecs()/1000.0 << " secs" << std::endl;
    m_network.DumpNetworkStats();
    m_network.SetTrainingState(false);
}
