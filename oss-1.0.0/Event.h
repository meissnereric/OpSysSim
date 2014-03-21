#ifndef EVENT_H
#define EVENT_H
#include <memory>
#include <queue>
#include "Task.h"
#include "System.h"

class Event
{
public:
    virtual ~Event();
    Event();
    double time;
    void nextProcess(){
        process();
    }
protected:
    virtual void process();
    void addToCPU(std::shared_ptr<Task> t);
    void addToIO(std::shared_ptr<Task> t);
    std::shared_ptr<int> numCPUs;
    std::shared_ptr<std::vector<bool>> IODevices;
    typedef std::priority_queue<std::vector<std::shared_ptr<Event>>> EventQueue;
    std::shared_ptr<EventQueue> eq;
    std::shared_ptr<double> currTime;

private:
};

class StartTask : public Event
{
public:
    ~StartTask(){}
    StartTask(std::shared_ptr<Task> t)
    {
        task = task;
    }
    void process();
protected:
private:
    std::shared_ptr<Task> task;

};

class EndSim : public Event
{
public:
    EndSim(int endTime)
    {
        time = endTime;
    }
    void process();
protected:
private:

};

class CPUDone : public Event
{
public:
    CPUDone(std::shared_ptr<Task> nt, double st)
    {
        time = st;
        task = nt;
    }
    void process();
protected:
private:
    std::shared_ptr<Task> task;

};

class IODone : public Event
{
public:
    IODone(std::shared_ptr<Task> nt, double st)
    {
        time = st;
        task = nt;
    }
    void process();
protected:
private:
    std::shared_ptr<Task> task;

};


#endif // EVENT_H
