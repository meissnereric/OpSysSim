#ifndef DEVICE_H
#define DEVICE_H


class Device
{
    public:
        static int nextID;
        int id;
        bool open;
        Device(){
            id = nextID;
            open = true;
        }
    protected:
    private:
};
#endif // DEVICE_H
