#ifndef BURST_H
#define BURST_H

class Burst
{
    public:
        Burst(double dur, int dev){
            duration = dur;
            timeLeft = dur;
            device = dev;
        }

        double getDuration(){return duration;}
        int getDevice(){return device;}
        double getTimeLeft(){return timeLeft;}

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
};

#endif // BURST_H
