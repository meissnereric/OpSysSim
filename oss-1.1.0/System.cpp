#ifndef SYSTEM_CPP
#define SYSTEM_CPP
#include <cassert>
#include "System.h"
std::shared_ptr<System>  System::pInst;
std::default_random_engine System::generation;
double System::quantumTime;

std::shared_ptr<System> System::instance()
{
    if(! pInst)
        pInst = std::shared_ptr<System>(new System(Fifo, FifoM));
    return pInst;
}

void System::clearSystem(typeOfReadySet t, double quant, typeOfMemory mem)
{
    pInst = std::shared_ptr<System>(new System(t, mem));
    if(quant > 0)
        pInst->quantumTime = quant;
    else
        pInst->quantumTime = MAXLONG;
    //generation = *(new std::default_random_engine );
}

void System::nextEvent()
{

    auto event = eq.top();
    currTime = event->time;
    eq.pop();

    if( event != NULL )
    {
        auto t = event->type;
        if(t == START )
        {
            auto e = std::dynamic_pointer_cast<StartTask> (event);
            e->nextProcess();
        }
        else if(t == CPU )
        {
            auto e = std::dynamic_pointer_cast<CPUDone> (event);
            e->nextProcess();
        }
        else if(t == IO )
        {
            auto e = std::dynamic_pointer_cast<IODone> (event );
            e->nextProcess();
        }
        else if(t == END)
        {
            endTime = currTime;
            std::dynamic_pointer_cast<EndSim> (event )->nextProcess();
            return;
        }
    }
    else
    {
        assert(true);
    }
}

void System::add(std::shared_ptr<Task> task)
{
    if(task->nextBurstType == -1)  //if CPU
    {
        CpuReadySet->add(task);
    }
    else  // if IO
    {
        IOReadySet.at(task->nextBurstType)->add(task);
    }
}

void System::runNext()
{
    if(idleCPU > 0)
    {
        auto next = CpuReadySet->next();
        if(next != NULL )
        {
            auto startTime = next->startTime;
            runCPU(next);
        }
    }
    for(int i =0; i < numIO; ++i)
    {
        auto next2 = IOReadySet.at(i)->next();
        if(next2 != NULL)
        {
            if(idleDevices[next2->nextBurstType])
                runIO(next2);
            else
                IOReadySet.at(i)->add(next2);
        }
    }
}

double System::doMemory(int pgnum)
{
    if(memory->inPages(pgnum))
        return 0;
    else
    {
        memory->add(pgnum);
        stats.numFaults++;
        return faultCost;
    }
}

void System::runCPU(std::shared_ptr<Task> t)
{
    --idleCPU;
    std::shared_ptr<Event> e;
    auto pagefault = doMemory( t->bursts.at(t->nextBurstIndex).getPage() );
    if(t->bursts.at(t->nextBurstIndex).getTimeLeft() < quantumTime)
    {
        e = std::dynamic_pointer_cast<Event>( std::make_shared<CPUDone> (t, currTime + t->bursts.at(t->nextBurstIndex).getTimeLeft() + contextSwitch + pagefault, true) );
        t->updateNB();
    }
    else
    {
        e = std::dynamic_pointer_cast<Event>( std::make_shared<CPUDone> (t, currTime + quantumTime + contextSwitch + pagefault, false) );
        t->bursts.at(t->nextBurstIndex).decrementTimeLeft(quantumTime);
    }
    if(e)
    {
        auto timeOfe = e->time;
        eq.push(e);
    }
    else
        assert(true);
}

void System::runIO(std::shared_ptr<Task> t)
{
    idleDevices[t->nextBurstType] = false;
    std::shared_ptr<Event> e = std::dynamic_pointer_cast<Event>( std::make_shared <IODone> (t, currTime + t->bursts.at( t->nextBurstIndex).getDuration() + contextSwitch, t->nextBurstType) );
    auto timeOfe = e->time;
    eq.push(e);
    t->updateNB();
}

void Statistics::indivCpuIoAverages()
{

    //CPU utilization update
    double avCpu = std::accumulate(cpuUtilizations.begin(), cpuUtilizations.end(),0);
    avCpu = avCpu / (double)(System::instance()->endTime * System::instance()->maxCPU);
    averageCpuUtilization = avCpu;

    //IO utilizations update
    for(int i =0; i< System::instance()->numIO; ++i)
    {
        avCpu = std::accumulate(ioUtilizations.at(i).begin(), ioUtilizations.at(i).end(),0);
        avCpu = avCpu / (double)(System::instance()->endTime * System::instance()->numIO);
        averageIoUtilization.push_back(avCpu);
    }

    //update throughput
    averageThroughput = numTasksRun / System::instance()->endTime;
    averageFaults = numFaults;

}

void Statistics::updateFinalAverages()
{
    //cpuUtilizations
    mean(cpuUtilizations, averageCpuUtilization);
    std_dev(cpuUtilizations, stdDevCpuUtilization);

    auto rTmin = std::min_element(cpuUtilizations.begin(), cpuUtilizations.end());
    auto place = std::distance(cpuUtilizations.begin(), rTmin);
    if(place >=0)
        minCpuUtilization = cpuUtilizations.at(place);

    auto rTmax = std::max_element(cpuUtilizations.begin(), cpuUtilizations.end());
    place = std::distance(cpuUtilizations.begin(), rTmax);
    if(place >=0)
        maxCpuUtilization = cpuUtilizations.at(place);

    //Throughputs
    mean(throughputs, averageThroughput);
    std_dev(throughputs, stdDevThroughput);

    rTmin = std::min_element(throughputs.begin(), throughputs.end());
    place = std::distance(throughputs.begin(), rTmin);
    if(place >=0)
        minThroughput = throughputs.at(place);

    rTmax = std::max_element(throughputs.begin(), throughputs.end());
    place = std::distance(throughputs.begin(), rTmax);
    if(place >=0)
        maxThroughput = throughputs.at(place);

    //Faults
    mean(faults, averageFaults);
    std_dev(faults, stdDevFaults);

    rTmin = std::min_element(faults.begin(), faults.end());
    place = std::distance(faults.begin(), rTmin);
    if(place >=0)
        minFaults = faults.at(place);

    rTmax = std::max_element(faults.begin(), faults.end());
    place = std::distance(faults.begin(), rTmax);
    if(place >=0)
        maxFaults = faults.at(place);


    for(int i =0; i < ioUtilizations.size(); ++i)
    {
        //IO Updates
        mean(ioUtilizations.at(i), averageIoUtilization.at(i));
        std_dev(ioUtilizations.at(i), stdDevIoUtilization.at(i));

        rTmin = std::min_element(ioUtilizations.at(i).begin(), ioUtilizations.at(i).end());
        place = std::distance(ioUtilizations.at(i).begin(), rTmin);
        if(place >=0)
            minIoUtilization.at(i)= ioUtilizations.at(i).at(place);

        rTmax = std::max_element(ioUtilizations.at(i).begin(), ioUtilizations.at(i).end());
        place = std::distance(ioUtilizations.at(i).begin(), rTmax);
        if(place >=0)
            maxIoUtilization.at(i) = ioUtilizations.at(i).at(place);
    }
}

void Statistics::updateAverages()
{
    if(System::instance()->getMaxCpuSize() > maxCpuRSSize)
        maxCpuRSSize = System::instance()->getMaxCpuSize();
    //response times
    mean(responseTimes, averageResponseTime);
    std_dev(responseTimes, stdDevResponseTime);

    auto rTmin = std::min_element(responseTimes.begin(), responseTimes.end());
    auto place = std::distance(responseTimes.begin(), rTmin);
    if(place >=0)
        minResponseTime = responseTimes.at(place);

    auto rTmax = std::max_element(responseTimes.begin(), responseTimes.end());
    place = std::distance(responseTimes.begin(), rTmax);
    if(place >=0)
        maxResponseTime = responseTimes.at(place);

    //latencies
    mean(latencies, averageLatency);
    std_dev(latencies, stdDevLatency);

    rTmin = std::min_element(latencies.begin(), latencies.end());
    place = std::distance(latencies.begin(), rTmin);
    if(place >=0)
        minLatency = latencies.at(place);

    rTmax = std::max_element(latencies.begin(), latencies.end());
    place = std::distance(latencies.begin(), rTmax);
    if(place >=0)
        maxLatency = latencies.at(place);



    //throughput
    if(throughputs.size() < 1)
    {
        indivCpuIoAverages();
    }
    else
    {
        updateFinalAverages();
    }

}

void System::initEQ(double cpus, double ios, double context, double mix, double freq, int mp, int pgrng, double fc)
{
    idleCPU = cpus;
    maxCPU = cpus;
    numIO = ios;
    contextSwitch = context;
    taskMix = mix;
    frequencyOfTasks = freq;
    maxPages = mp;
    pageRange = pgrng;
    (*memory).max_pages = mp;
    faultCost = fc;

    endTime = 2000;
    int numberOfTasks = endTime / freq;

    IOReadySet.resize(numIO);
    idleDevices.resize(numIO);
    stats.ioUtilizations.resize(numIO);
    stats.maxPages = maxPages;


    for(int i = 0; i < numIO; ++i)
    {
        idleDevices.at(i) = true;
        IOReadySet.at(i) = std::make_shared<FIFO>();
    }

    int t = 0;
    std::uniform_int_distribution<int> distribution(1, 30);
    auto randomInt = std::bind(distribution, generation);

    std::uniform_int_distribution<int> distribution0_100(1, 100);

    std::normal_distribution<double> distributionSmall(3,1);
    auto randomRealSmall = std::bind (distributionSmall, generation);
    std::normal_distribution<double> distributionLarge(7,2);
    auto randomRealLarge = std::bind (distributionLarge, generation);

    std::uniform_int_distribution<int> distributionPages(1, pageRange);
    auto randomPageStart = std::bind(distributionPages, generation);
    std::normal_distribution<double> distributionCloseness(0, (float)(pageRange) / 6.0);
    auto randomPageShift = std::bind (distributionCloseness, generation);



    for(int i = 0; i < numberOfTasks; ++i)
    {
        int whichSet = distribution0_100(generation);
        int numBurst = randomInt() % 12 + 2;
        int device  = randomInt() % (int)numIO;

        int pageBase = randomPageStart();
        int pageNumber = pageBase + (int) randomPageShift();
        if(pageNumber < 0) pageNumber = 0;
        if (pageNumber >= pageRange)
            pageNumber = pageRange - 1;

        std::vector<Burst> b;

        if(whichSet >= (taskMix * 100) )  //CPU bound
        {
            b.push_back(Burst(randomRealLarge(), -1, pageNumber));
            for(int i =0; i < numBurst; ++i)
            {

                int pageNumber = pageBase + (int) randomPageShift();
                //std::cout << "pageBase: " << pageBase << " pageNumber: " << pageNumber << std::endl;
                if (pageNumber < 0)
                    pageNumber = 0;
                if (pageNumber >= pageRange)
                    pageNumber = pageRange - 1;

                if(device >=0)
                {
                    double burstTime = randomRealSmall();
                    b.push_back(Burst(burstTime, device, pageNumber));
                    device = -1;
                }
                else
                {
                    double burstTime = randomRealLarge();
                    b.push_back(Burst(burstTime, device, pageNumber));
                    device = randomInt() % (int)numIO;
                }
            }
            b.push_back(Burst(randomRealLarge(), -1, pageNumber));
        }
        else  // IOBound
        {
            b.push_back(Burst(randomRealSmall(), -1, pageNumber));
            for(int i =0; i < numBurst; ++i)
            {

                int pageNumber = pageBase + (int) randomPageShift();
                //std::cout << "pageBase: " << pageBase << " pageNumber: " << pageNumber << std::endl;
                if(pageNumber < 0)
                    pageNumber = 0;
                if(pageNumber >= pageRange)
                    pageNumber = pageRange - 1;

                if(device >=0)
                {
                    double burstTime = randomRealLarge();
                    b.push_back(Burst(burstTime, device, pageNumber));
                    device = -1;
                }
                else
                {
                    double burstTime = randomRealSmall();
                    b.push_back(Burst(burstTime, device, pageNumber));
                    device = randomInt() % (int)numIO;
                }
            }
            b.push_back(Burst(randomRealSmall(), -1, pageNumber));
        }

        //make a task, then an StartTask event, then an Event and push it into eq
        auto task = std::make_shared<Task>(t,b);
        auto e = std::make_shared <StartTask> ( task );
        eq.push( std::dynamic_pointer_cast<Event> (e )  );
        t += (frequencyOfTasks + randomInt() % 3 );
    }
    eq.push( std::dynamic_pointer_cast<Event> (std::make_shared <EndSim> (endTime) ) );
}
#endif // SYSTEM_CPP
