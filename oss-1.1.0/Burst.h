#ifndef BURST_H
#define BURST_H

class Burst
{
    public:
        Burst(double dur, int dev, int pg){
            duration = dur;
            timeLeft = dur;
            device = dev;
            pgnum = pg;
        }

        double getDuration(){return duration;}
        int getDevice(){return device;}
        double getTimeLeft(){return timeLeft;}
        int getPage(){return pgnum;}

        void setDuration(double d){duration = d;}
        void setDevice(int d){device = d;}
        void decrementTimeLeft(double t){
            timeLeft -= t;
        }

    protected:
    private:
        double timeLeft;
        double duration;
        int device;
        int pgnum;
};

#endif // BURST_H
