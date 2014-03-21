#include "ReadySet.h"

inline bool operator<(const std::shared_ptr<Task> &a, const std::shared_ptr<Task> &b){
    return a->bursts[a->nextBurstIndex].getDuration() < b->bursts[b->nextBurstIndex].getDuration();
}

struct ApproxCompare{
    bool operator()(const std::shared_ptr<Task> &a, const std::shared_ptr<Task> &b){
        return a->averageCpuBurstLength < b->averageCpuBurstLength;
    }
};

void FIFO::add(std::shared_ptr<Task> task){
    rs.push_back(task);
    if(rs.size() > maxCpuRSSize)
         maxCpuRSSize = rs.size();
}


std::shared_ptr<Task> FIFO::next(){
    if(rs.size() > 0){
        auto temp = rs.front();
        rs.pop_front();
        return temp;
    }
    else
        return NULL;
}

void ShortestTask::add(std::shared_ptr<Task> task){
    rs.push_back(task);
    if(rs.size() > 1)
        std::sort(rs.begin(), rs.end(), ApproxCompare());
    if(rs.size() > maxCpuRSSize)
         maxCpuRSSize = rs.size();
}


std::shared_ptr<Task> ShortestTask::next(){
    if(rs.size() > 0){
        auto temp = rs.front();
        rs.pop_front();
        return temp;
    }
    else
        return NULL;
}

void ApproxShortestTask::add(std::shared_ptr<Task> const task){
    if(task->nextBurstIndex > 0){ // other than the first burst
            if((int)task->nextBurstIndex % 2 == 0){
                task->averageCpuBurstLength =0;
                int burstsSoFar = (task->nextBurstIndex) / 2 + 1;
                for(int i =0; i <= burstsSoFar; i+=2){
                    task->averageCpuBurstLength += task->bursts.at(i).getDuration();
                }
                task->averageCpuBurstLength /= burstsSoFar;
            }
            else{
                task->averageIoBurstLength =0;
                int burstsSoFar = (task->nextBurstIndex)/2 + 1;
                for(int i =0; i < burstsSoFar; i+=2){
                    task->averageIoBurstLength += task->bursts.at(i).getDuration();
                }
                task->averageIoBurstLength /= burstsSoFar;
            }
    }
    else{ // first burst
        task->averageCpuBurstLength = task->bursts.at(0).getDuration();
    }
    rs.push_back(task);
    if(rs.size() > 1)
        std::sort(rs.begin(), rs.end(), ApproxCompare());
    if(rs.size() > maxCpuRSSize)
         maxCpuRSSize = rs.size();
}


std::shared_ptr<Task> ApproxShortestTask::next(){
    if(rs.size() > 0){
        auto temp = rs.front();
        rs.pop_front();
        return temp;
    }
    else
        return NULL;
}

std::shared_ptr<Task> RoundRobin::next(){

    if(rs.size() > 0){
        auto temp = rs.front();
        rs.pop_front();
        return temp;
    }
    else
        return NULL;
}

void RoundRobin::add(std::shared_ptr<Task> task){
    rs.push_back(task);
    if(rs.size() > maxCpuRSSize)
         maxCpuRSSize = rs.size();
}

