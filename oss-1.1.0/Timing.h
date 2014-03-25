#ifndef TIMING_H
#define TIMING_H
#include <vector> //std::vector
#include <functional>   // std::bind
#include <algorithm> // for_each
#include <numeric> //accumulate
#include <math.h> // sqrt
#include <ctime> //time_t

template<class C, class T>
void mean(C container, T& mean);

template<class C, class T>
void std_dev(C container, T& stdDev);

template<typename F>
time_t timeFunc(F f);

//takes in a function and how many times you want to run it, then returns the average and stdDev time it took
template<typename F>
void average_time(F func, time_t &timeAverage, time_t &timeStdDev, int numInvokes);

template<class T>
struct diffSquaredSum
{
    diffSquaredSum(T loc_mean)
    {
        sum = 0;
        mean = loc_mean;
    }
    void operator() (T num)
    {
        T temp = (mean - num) * (mean - num);
        sum += temp;
    };
    T sum;
    T mean;
};

//uses accumulate to sum the container, then divides by the size of the container
template<class C, class T>
void mean(C container, T& mean)
{
    double cnt = container.size();
    double total = std::accumulate(container.begin(), container.end(), 0.0);
    if(cnt != 0)
        mean = double(total) / (double)cnt;
    else
        mean = total;
}

//Uses a few previously defined functions to find the std_dev of a container of things
template<class C, class T>
void std_dev(C container, T& stdDev)
{
    T Mean;
    mean(container, Mean); // find the mean
    diffSquaredSum<T> sum = std::for_each(container.begin(), container.end(), diffSquaredSum<T>(Mean));
    double cnt = container.size();
    stdDev = sqrt(sum.sum / cnt);
}
//pretty much just endClock - startClock after the function has run.
template<typename F>
time_t timeFunc(F f)
{
    auto t1 = clock();
    f();
    auto t2 = clock();
    auto time = t2 - t1;
    return time;

}

//takes in a function and how many times you want to run it, then returns the average and stdDev time it took
template<typename F>
void average_time(F func, time_t &timeAverage, time_t &timeStdDev, int numInvokes)
{
    std::vector<time_t> times(numInvokes);
    std::generate(times.begin(), times.end(), [&](){return timeFunc(func) ;});
    mean(times, timeAverage);
    std_dev(times, timeStdDev);
}


#endif // TIMING_H
