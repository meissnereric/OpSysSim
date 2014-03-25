#ifndef READYSET_H
#define READYSET_H
#include "Task.h"
#include <memory>
#include <algorithm>
#include <vector>
#include <queue>

class System;


class ReadySet
{
public:
    ReadySet() {}
    virtual std::shared_ptr<Task> next()=0;
    virtual void add(std::shared_ptr<Task>)=0;
    std::deque<std::shared_ptr<Task>> rs;
    double maxCpuRSSize;

protected:
private:
};

class FIFO : public ReadySet
{
public:
    FIFO() {}
    std::shared_ptr<Task> next();
    void add(std::shared_ptr<Task> task);
protected:
private:

};

class ShortestTask : public ReadySet
{
public:
    ShortestTask() {}
    std::shared_ptr<Task> next();
    void add(std::shared_ptr<Task> task);
protected:
private:
};

class ApproxShortestTask : public ReadySet
{
public:
    ApproxShortestTask() {}
    std::shared_ptr<Task> next();
    void add(std::shared_ptr<Task> task);
protected:
private:
};

class RoundRobin : public ReadySet
{
public:
    RoundRobin() {}
    std::shared_ptr<Task> next();
    void add(std::shared_ptr<Task> task);
protected:
private:
};


#endif // READYSET_H
