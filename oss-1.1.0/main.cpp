#include <iostream>
#include <random>
#include <fstream>
#include "System.h"
#include "ReadySet.h"



void runSimulation(double numCPUs, double numIO, double contextCost, double taskMix, double freqOfJobCreation,std::vector<Statistics>& totals, int maxPages, int pageRange, double faultCost, typeOfMemory mem, typeOfReadySet type, double quant = -1){
        System::clearSystem(type, quant, mem);
        System::instance()->initEQ(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, maxPages, pageRange, faultCost);

        while(System::instance()->notDone){
                System::instance()->nextEvent();
        }
        System::instance()->notDone = true;
        Statistics temp = System::instance()->stats;
        totals.push_back(temp);
}

bool isZero(int i){
    if(i==0)
        return true;
    return false;
}

void runStats(std::vector<Statistics> totals, std::ostream & out){
    Statistics finalTotals;

    for(auto &total: totals){
        finalTotals.cpuUtilizations.emplace_back(total.averageCpuUtilization);
        finalTotals.responseTimes.emplace_back(total.averageResponseTime);
        finalTotals.latencies.emplace_back(total.averageLatency);
        finalTotals.throughputs.emplace_back(total.averageThroughput);
        finalTotals.faults.emplace_back(total.averageFaults);
        finalTotals.numTasksRun++;

        finalTotals.ioUtilizations.resize(total.averageIoUtilization.size());
        finalTotals.maxIoUtilization.resize(total.averageIoUtilization.size());
        finalTotals.minIoUtilization.resize(total.averageIoUtilization.size());
        finalTotals.averageIoUtilization.resize(total.averageIoUtilization.size());
        finalTotals.stdDevIoUtilization.resize(total.averageIoUtilization.size());

        for(int i =0; i < total.averageIoUtilization.size() ; i++)
            finalTotals.ioUtilizations.at(i).emplace_back(total.averageIoUtilization.at(i));
    }

    finalTotals.updateAverages();

    finalTotals.printForAnalysis(out);
}

int main(int argc, char** argv)
{
    std::vector<double> parameters;
    for(int i=0; i < argc; ++i){
        parameters.push_back(std::atof(argv[i]) );
    }

    auto numCPUs = parameters.at(1);
    auto numIO = parameters.at(2);
    auto contextCost = parameters.at(3);
    auto taskMix = parameters.at(4);
    auto freqOfJobCreation = parameters.at(5);
    auto quantumTime = parameters.at(6);
    auto maxPages = parameters.at(7);
    auto pageRange = parameters.at(8);
    auto faultCost = parameters.at(9);

    std::vector<Statistics> fifoTotals, astTotals, stTotals, rrTotals;
    std::ofstream fifoFile, scFile, stFile, rrFile;
    fifoFile.open("maxPages_vs_faults_fifo.txt");
    scFile.open("maxPages_vs_faults_sc.txt");
    //stFile.open("context_vs_throughput_st.txt");
    //rrFile.open("context_vs_throughput_rr.txt");

    double numberOfRuns = 10;
    for(double maxPages = 1; maxPages < 11; maxPages += 1 ){
        for(int i = 0; i < numberOfRuns; ++i){
            runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, fifoTotals, maxPages, pageRange, faultCost, FifoM,  AST);
            runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, astTotals, maxPages, pageRange, faultCost, SC,  AST);
        }
        runStats(fifoTotals, fifoFile);
        runStats(astTotals, scFile);
        //std::cout <<"maxPages: " << maxPages << "\t numFaults: " << astTotals.at(maxPages+1).averageFaults << "\t " << astTotals.at(0).stdDevFaults << std::endl;
        fifoTotals.clear();
        astTotals.clear();
        //runStats(stTotals, stFile);
        //runStats(rrTotals, rrFile);
    }

    /*for(int i = 0; i < numberOfRuns; ++i){
        runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, fifoTotals, Fifo);
        runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, astTotals, AST);
        runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, stTotals, ST);
        runSimulation(numCPUs, numIO, contextCost, taskMix, freqOfJobCreation, rrTotals, RR, quantumTime);
    }
    runStats(fifoTotals);
    std::cout << std::endl;
    runStats(astTotals);
    std::cout << std::endl;
    runStats(stTotals);
    std::cout << std::endl;
    runStats(rrTotals);
    std::cout << std::endl;
*/

    return 0;
}

