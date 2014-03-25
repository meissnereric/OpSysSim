#ifndef TASK_H
#define TASK_H
#include <vector>
#include <iostream>
#include "Burst.h"


class Task
{
    public:
        Task(double start, std::vector<Burst> b){
            startTime = start;
            bursts = b;
            latency = responseTime = -1;
            nextBurstType = -1;
            nextBurstIndex = 0;
            if(nextBurstIndex < b.size()){
                if(nextBurstType >= 0)
                    averageIoBurstLength = b.at(nextBurstIndex).getDuration();
                else{
                    averageCpuBurstLength =0;
                    for(int i =0; i < b.size(); i+=2)
                        averageCpuBurstLength += b.at(i).getDuration();
                    averageCpuBurstLength += b.at(b.size() -1 ).getDuration();
                    averageCpuBurstLength /= ( (b.size() / 2) + 1);
                }
            }

        }
        void updateNB(){
            nextBurstIndex++;
            auto t = bursts.size();
            if(nextBurstIndex < t)
                nextBurstType = bursts[nextBurstIndex].getDevice();
        }
        double startTime;
        std::vector<Burst> bursts;
        double latency;
        double responseTime;
        int nextBurstIndex;
        int nextBurstType; // -1 indicates a cpu, 0+ indicates which IO device
        double averageCpuBurstLength;
        double averageIoBurstLength;
    protected:
    private:
};


#endif // TASK_H
