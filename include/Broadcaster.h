#ifndef __BROADCASTER_H__
#define __BROADCASTER_H__

#include "Constants.h"
#include "DebugTools.h"
#include <iostream>
#include <vector>
#include <algorithm>

template<typename Msg>
class Broadcaster
{
public:
    typedef void callback(const Msg& m);
    void addListener(callback rec)
    {
        if(std::find(callbacks.begin(), callbacks.end(), rec) != callbacks.end())
            callbacks.push_back(rec);
    }
    void removeListener(callback rec)
    {
        for(auto it = callbacks.begin(); it != callbacks.end();)
        {
            if(*it == rec)
            {
                callbacks.erase(it);
                return;
            }
        }
    }
    void send(const Msg& m)
    {
        for(auto cb : callbacks)
            rec(m);
    }
private:
    std::vector<callback> callbacks;
};

#endif