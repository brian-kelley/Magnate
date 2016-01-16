#ifndef __BROADCASTER_H__
#define __BROADCASTER_H__

#include "DebugTools.h"
#include <iostream>
#include <vector>
#include <algorithm>

//Listener is the type that implements the callback
//Msg is the type that the Broadcaster sends out
template<typename Msg>
class Broadcaster
{
public:
    typedef void (*callback) (void*, const Msg&);    //signature of callback function
    void addListener(void* lis, callback func)       //Add a callback function to act on lis
    {
        LisEntry le(lis, func);
        if(std::find(callbacks.begin(), callbacks.end(), le) == callbacks.end())
            callbacks.push_back(le);
    }
    void removeListener(void* lis, callback func)
    {
        LisEntry le(lis, func);
        auto it = find(callbacks.begin(), callbacks.end(), le);
        if(it != callbacks.end())
            callbacks.erase(it);
    }
    void send(const Msg& m)
    {
        for(LisEntry lisEntry : callbacks)
            lisEntry.func(lisEntry.lis, m);
    }
private:
    struct LisEntry
    {
        LisEntry(void* l, callback f)
        {
            lis = l;
            func = f;
        }
        bool operator==(const LisEntry& r)
        {
            return lis == r.lis && func == r.func;
        }
        void* lis;
        callback func;
    };
    std::vector<LisEntry> callbacks;
};

#endif