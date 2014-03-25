#ifndef MEMORYSCHEDULER_H
#define MEMORYSCHEDULER_H
#include <vector>
#include <memory>
#include <algorithm>

struct Page
{
    int num;
    bool sc;
    Page(int n) : num(n)
    {
        sc = false;
    }
};

inline bool operator==(const Page& a, const Page& b)
{
    if (a.num == b.num)
        return true;
    else
        return false;
}


class MemoryScheduler
{
public:
    MemoryScheduler() {}
    int max_pages;
    virtual bool inPages(int pgnum) =0;
    virtual void add(int pgnum) =0;
protected:
    std::vector<Page> pages;
private:
};

class FIFOMem : public MemoryScheduler
{
public:
    FIFOMem() {}
    bool inPages(int pgnum)
    {
        Page p = Page(pgnum);
        auto found = std::find(pages.begin(), pages.end(), p );
        if ( found != pages.end() )
            return true;
        else
            return false;
    }
    void add(int pgnum)
    {
        Page p = Page(pgnum);
        if(pages.size() < max_pages)
            pages.push_back(p);
        else
        {
            std::rotate(pages.begin(), pages.begin()+1, pages.end());
            pages.pop_back();
            pages.push_back(p);
        }
    }
protected:
private:

};

class SecondChance : public MemoryScheduler
{
public:
    SecondChance() {}
    bool inPages(int pgnum)
    {
        Page p = Page(pgnum);
        auto found = std::find(pages.begin(), pages.end(), p );
        if ( found != pages.end() )
        {
                found->sc = true;
                return true;
        }
        else
            return false;
    }

    void add(int pgnum)
    {
        Page p = Page(pgnum);
        if(pages.size() < max_pages)
            pages.push_back(p);
        else
        {
            bool pushed = false;
            while(!pushed)
            {
                if( !(pages.begin()->sc) )
                {
                    std::rotate(pages.begin(), pages.begin()+1, pages.end());
                    pages.pop_back();
                    pages.push_back(p);
                    pushed = true;
                }
                else{
                    pages.begin()->sc = false;
                    std::rotate(pages.begin(), pages.begin()+1, pages.end());
                }


            }
        }
    }
protected:
private:

};

#endif // MEMORYSCHEDULER_H
