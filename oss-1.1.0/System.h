#ifndef SYSTEM_H
#define SYSTEM_H
#include <queue>
#include <iostream>
#include <memory>
#include <random>
#include <iomanip>
#include <time.h>
#include <climits>
#include <windows.h>
#include "MemoryScheduler.h"
#include "ReadySet.h"
#include "Timing.h"

enum typeOfEvent {START, CPU, IO, END};
enum typeOfReadySet {Fifo, ST, AST, RR};
enum typeOfMemory {FifoM, SC};


// each system::instance() will have one of these
// and then after we've run a number of them the average of
// each instance().Statistics will be taken and output
struct Statistics{

    Statistics(){
        minLatency = 0;
        maxLatency = 0;
        averageLatency = 0;
        stdDevLatency = 0;

        minResponseTime = 0;
        maxResponseTime = 0;
        averageResponseTime = 0;
        stdDevResponseTime = 0;

        minCpuUtilization = 0;
        maxCpuUtilization = 0;
        averageCpuUtilization = 0;
        stdDevCpuUtilization = 0;
        maxCpuRSSize = 0;

        numTasksRun = 0;
        numFaults = 0;

        minFaults = 0;
        maxFaults = 0;
        averageFaults = 0;
        stdDevFaults = 0;

        minThroughput = 0;
        maxThroughput = 0;
        averageThroughput = 0;
        stdDevThroughput = 0;
    }

    std::vector<double> latencies;
    double minLatency;
    double maxLatency;
    double averageLatency;
    double stdDevLatency;

    std::vector<double> responseTimes;
    double minResponseTime;
    double maxResponseTime;
    double averageResponseTime;
    double stdDevResponseTime;

    std::vector<double> cpuUtilizations;
    double minCpuUtilization;
    double maxCpuUtilization;
    double averageCpuUtilization;
    double stdDevCpuUtilization;
    double maxCpuRSSize;

    std::vector<std::vector<double>> ioUtilizations;
    std::vector<double> minIoUtilization;
    std::vector<double> maxIoUtilization;
    std::vector<double> averageIoUtilization;
    std::vector<double> stdDevIoUtilization;

    double numTasksRun;
    double numFaults;

    std::vector<double> faults;
    double minFaults;
    double maxFaults;
    double averageFaults;
    double stdDevFaults;



    double faultCost;
    double maxPages;

    std::vector<double> throughputs;
    double minThroughput;
    double maxThroughput;
    double averageThroughput;
    double stdDevThroughput;

    void updateAverages();
    void indivCpuIoAverages();
    void updateFinalAverages();


    void print(std::ostream & out){

        out << std::setprecision(3) << std::fixed;
        out << "\t\tmin \t\tmax \t\taverage \tstd\n";
        out << "Latency\t\t" << minLatency<< "\t\t" << maxLatency<< "\t\t" << averageLatency<< "\t\t" << stdDevLatency<<std::endl;
        out << "Response Time:\t" <<minResponseTime<< "\t\t"<< maxResponseTime<< "\t\t"<< maxResponseTime<< "\t\t"<< stdDevResponseTime<<std::endl;
        out << "Cpu Util:\t" << minCpuUtilization<< "\t\t"<< maxCpuUtilization<< "\t\t"<< averageCpuUtilization<< "\t\t"<< stdDevCpuUtilization<< std::endl;
        for(int i=0; i < averageIoUtilization.size(); i++)
            out << "ioUtil[" << i << "]:\t" << minIoUtilization.at(i) << "\t\t" << maxIoUtilization.at(i) << "\t\t"
                << averageIoUtilization.at(i) << "\t\t" << stdDevIoUtilization.at(i) << std::endl;
        out << std::setprecision(4) << std::fixed;
        out << "Throughput:\t" << minThroughput << "\t\t" << maxThroughput << "\t\t" << averageThroughput << "\t\t" << stdDevThroughput << std::endl;
        out << std::setprecision(0) << std::fixed;
        out << "Faults:\t\t" << minFaults << "\t\t" << maxFaults << "\t\t" << averageFaults << "\t\t" << stdDevFaults << std::endl;
        out << "numTasks:\t\t" << numTasksRun << std::endl;
    }
    void printForAnalysis(std::ostream & out){
        /*
        out << std::setprecision(3) << std::fixed;
        out << "\t" << minLatency<< "\t\t" << maxLatency<< "\t\t" << averageLatency<< "\t\t" << stdDevLatency<<std::endl;
        out << "\t" <<minResponseTime<< "\t\t"<< maxResponseTime<< "\t\t"<< maxResponseTime<< "\t\t"<< stdDevResponseTime<<std::endl;
        out << "\t" << minCpuUtilization<< "\t\t"<< maxCpuUtilization<< "\t\t"<< averageCpuUtilization<< "\t\t"<< stdDevCpuUtilization<< std::endl;
        for(int i=0; i < averageIoUtilization.size(); i++)
            out << "\t" <<minIoUtilization.at(i) << "\t\t" << maxIoUtilization.at(i) << "\t\t"
                << "\t" <<averageIoUtilization.at(i) << "\t\t" << stdDevIoUtilization.at(i) << std::endl;
        out << std::setprecision(4) << std::fixed;
        out << "\t" << minThroughput << "\t\t" << maxThroughput << "\t\t" << averageThroughput << "\t\t" << stdDevThroughput << std::endl;
        out << std::endl;
        */
        out << std::setprecision(0) << std::fixed;
        out << averageFaults << "\t" << stdDevFaults << std::endl;

    }
};

class Event
{
public:
    Event(){}
    double time;
    typeOfEvent type;
    void virtual nextProcess()=0;
protected:
    virtual void process()=0;
    void addToCPU(std::shared_ptr<Task> t);
    void addToIO(std::shared_ptr<Task> t);

private:
};

class StartTask : public Event
{
public:
    StartTask(std::shared_ptr<Task> t):Event(), task(t){
        time = t->startTime;
        type = START;
        }
    void process();
    void nextProcess(){
        process();
    }
    std::shared_ptr<Task> getTask(){return task;}
    std::shared_ptr<Task> task;
protected:
private:

};

class EndSim : public Event
{
public:
    EndSim(int endTime)
    {
        type = END;
        time = endTime;
    }
    void process();
    void nextProcess(){
        process();
    }
protected:
private:

};

class CPUDone : public Event
{
public:
    CPUDone(std::shared_ptr<Task> nt, double st, bool d)
    {
        type = CPU;
        time = st;
        task = nt;
        done = d;
    }
    void process();
    void nextProcess(){
        process();
    }
protected:
private:
    bool done;
    std::shared_ptr<Task> task;

};

class IODone : public Event
{
public:
    IODone(std::shared_ptr<Task> nt, double st, int which)
    {
        whichIOFinished = which;
        type = IO;
        time = st;
        task = nt;
    }
    void process();
    void nextProcess(){
        process();
    }
protected:
private:
    int whichIOFinished;
    std::shared_ptr<Task> task;

};
struct compEvent{
    bool operator() (const std::shared_ptr<Event> &a, const std::shared_ptr<Event> &b) const{
        return a->time > b->time;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Task& t){
    os << "StartTime: " << t.startTime  << " Bursts.size(): " << t.bursts.size() << std::endl;
    os << "nBI: " << t.nextBurstIndex << "\t nbT " << t.nextBurstType << std::endl;
    return os;
}

class System
{
public:
    static void clearSystem(typeOfReadySet, double, typeOfMemory);
    static std::shared_ptr<System> instance();
    static double quantumTime;
    void runNext();
    void add(std::shared_ptr<Task>);
    void runCPU(std::shared_ptr<Task>);
    void runIO(std::shared_ptr<Task>);
    double doMemory(int pgnum);

    void initEQ(double cpus, double ios, double context, double mix, double freq, int mp, int pgrng, double fc);
    std::priority_queue<std::shared_ptr<Event>,std::vector<std::shared_ptr<Event>>, compEvent> eq;
    void nextEvent();
    bool notDone;

    double maxCPU;
    double idleCPU;
    double currTime;
    double endTime;
    double numIO;
    int maxPages;
    int pageRange;
    double faultCost;
    double contextSwitch;
    Statistics stats;

    std::vector<bool> idleDevices;
    double getMaxCpuSize(){return CpuReadySet->maxCpuRSSize;}
private:
    static std::default_random_engine generation;
    std::shared_ptr<ReadySet> CpuReadySet;
    std::shared_ptr<MemoryScheduler> memory;
    std::vector<std::shared_ptr<ReadySet>> IOReadySet;
    static std::shared_ptr<System> pInst;
    std::vector<std::shared_ptr<Task>> tasks;

    double frequencyOfTasks;
    double taskMix;

    System(typeOfReadySet type, typeOfMemory mem){
        if(type  == Fifo)
            CpuReadySet  = std::make_shared<FIFO>();
        if(type == ST)
            CpuReadySet = std::make_shared<ShortestTask>();
        if(type == AST)
            CpuReadySet = std::make_shared<ApproxShortestTask>();
        if(type == RR)
            CpuReadySet = std::make_shared<RoundRobin>();
        if(mem == FifoM)
            memory = std::make_shared<FIFOMem>();
        if(mem == SC)
            memory = std::make_shared<SecondChance>();
    }
};


#endif // SYSTEM_H
