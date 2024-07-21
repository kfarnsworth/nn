#pragma once

#include <map>
#include <memory>
#include <thread>
#include "Network.h"
#include "TrainingData.h"

class Training {

public:
    static std::shared_ptr<Training> GetTrainer(Network &network, std::string type="SGD");
    Training(Network &network);
    virtual ~Training();

    typedef std::shared_ptr<Training> TrainerPtr;

    void Train(TrainingData &trainingData, int batchSize, int numBatches=0, int trainingOuputIndex=-1);
    void TrainTest(TrainingData &trainingData, int dataSetIndex, int batchSize, int numBatches=0);
    virtual void SetLearningRate(double learningRate) {};
    virtual double GetLearningRate() { return 0; }
    void Stop() { StopThread(); }
    void WaitComplete();
    static void TrainingDataTypes(std::vector<std::string> &list);
    void TrainingProgress(int &batchCount, int &batchTotal, int &totalTime);

protected:
    virtual void TrainBatch(Network &network, std::vector<DataSet> &batchDataSet) = 0;

private:
    static std::map<std::string, std::shared_ptr<Training>> m_trainers;
    void TrainingThread(void *data);
    void StartThread(TrainingData &trainingData);
    void StopThread();

    std::thread m_trainingThread;
    Network &m_network;
    bool m_complete;
    bool m_testTraining;
    int m_testDataSetIndex;
    int m_batchSize;
    int m_numBatches;
    int m_trainingOutputIndex;
    int m_batchCount;
    int m_totalTimeSecs;
};
