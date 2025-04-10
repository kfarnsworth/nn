#pragma once

#include <map>
#include <memory>
#include <thread>
#include "Network.h"
#include "TrainingData.h"

class Training {

public:
    static std::shared_ptr<Training> GetTrainer(Network &network, std::string type="SGD");
    static constexpr double LEARNING_RATE_DEFAULT = 3.0;
    static constexpr double MOMENTUM_DEFAULT = 0.9;
    static constexpr int BATCH_COUNT_DEFAULT = 1000;
    static constexpr int BATCH_SIZE_DEFAULT = 30;

    Training(Network &network);
    virtual ~Training();

    typedef std::shared_ptr<Training> TrainerPtr;

    void Train(TrainingData &trainingData, int batchSize, int numBatches=0, int trainingOuputIndex=-1);
    void TrainTest(TrainingData &trainingData, int dataSetIndex, int batchSize, int numBatches=0);
    void SetLearningRate(double learningRate) { m_learningRate = learningRate; }
    double GetLearningRate() { return m_learningRate; }
    void SetMomentum(double momentum) { m_momentum = momentum; }
    double GetMomentum() { return m_momentum; }
    int GetBatchCount() { return m_numBatches; }
    int GetBatchSize() { return m_batchSize; }
    virtual std::string GetType() { return ""; }
    void Stop() { StopThread(); }
    void WaitComplete();
    static void TrainingDataTypes(std::vector<std::string> &list);
    void TrainingProgress(int &batchCount, int &batchTotal, int &totalTime);

protected:
    virtual void TrainBatch(Network &network, std::vector<DataSet> &batchDataSet) = 0;

    double m_learningRate;
    double m_momentum;

private:
    static std::map<std::string, std::shared_ptr<Training>> m_trainers;
    void TrainingThread(void *data);
    void StartThread(TrainingData &trainingData);
    void StopThread();

    std::thread m_trainingThread;
    Network &m_network;
    int m_batchSize;
    int m_numBatches;

    bool m_complete;
    bool m_testTraining;
    int m_testDataSetIndex;
    int m_trainingOutputIndex;
    int m_batchCount;
    int m_totalTimeSecs;
};
