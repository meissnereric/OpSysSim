#include "System.h"

void CPUDone::process()
{
    //std::cout <<"CPU->(" <<System::instance()->idleCPU << "++) " ;

    //increment utilization of resource
    if(done){
        double runningTime = task->bursts.at(task->nextBurstIndex - 1).getDuration() -  System::instance()->contextSwitch;//nbI - 1 is the CPU task that was just on this CPU
        System::instance()->stats.cpuUtilizations.push_back(runningTime);
    }
    else{
        //double runningTime = task->bursts.at(task->nextBurstIndex).getDuration() - task->bursts.at(task->nextBurstIndex ).getTimeLeft() - System::instance()->contextSwitch;//nbI - 1 is the CPU task that was just on this CPU
        //System::instance()->stats.cpuUtilizations.push_back(runningTime);
    }


    //free the resource
    System::instance()->idleCPU++;

    //if nbI >= size, then this task is done
    // so update its latency and run runNext() for the newly freed CPU
    if(task->nextBurstIndex >= task->bursts.size()){
        System::instance()->stats.numTasksRun++;
        task->latency = time - task->startTime;
        System::instance()->stats.latencies.push_back(task->latency);
    }
    else{
        System::instance()->add(task);
    }
    System::instance()->runNext();
}

void IODone::process()
{
    //std::cout <<"IO->";

    //increment utilization of resource
    double runningTime = task->bursts.at(task->nextBurstIndex -1).getDuration() - System::instance()->contextSwitch;//nbI - 1 is the CPU task that was just on this CPU
    System::instance()->stats.ioUtilizations.at(whichIOFinished).push_back(runningTime);

    //free up the resource its been using
    System::instance()->idleDevices[whichIOFinished] = true;

    //record response time
    if(task->responseTime < 0){
        task->responseTime = (time - task->startTime);
        System::instance()->stats.responseTimes.push_back(task->responseTime);
    }

    //add and do the next burst in this tasks list
    //as well as check the rest of the resources for newly freed ones
    System::instance()->add(task);
    System::instance()->runNext();
}

void StartTask::process()
{
    //std::cout <<"Start->";
    System::instance()->add(task);
    System::instance()->runNext();
}

void EndSim::process(){
    System::instance()->notDone = false;
    System::instance()->stats.updateAverages();

}

inline bool operator<(const std::shared_ptr<Event> &a, const std::shared_ptr<Event> &b)
{
    return a->time < b->time;
}
inline bool operator>(const std::shared_ptr<Event> &a, const std::shared_ptr<Event> &b)
{
    std::cout << a << " " << b << std::endl;
    return a->time > b->time;
}
