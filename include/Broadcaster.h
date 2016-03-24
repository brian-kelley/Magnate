#ifndef __BROADCASTER_H__
#define __BROADCASTER_H__

#include <vector>
#include <algorithm>
#include <string>

//Listener is the type that implements the Callback
//Msg is the type that the Broadcaster sends out
template<typename Msg>
class Broadcaster
{
public:
    typedef void (*MessageCallback) (void*, const Msg&);    //signature of Callback function
    void addListener(void* lis, MessageCallback func)       //Add a Callback function to act on lis
    {
        LisEntry le(lis, func);
        if(std::find(callbacks.begin(), callbacks.end(), le) == callbacks.end())
            callbacks.push_back(le);
    }
    void removeListener(void* lis, MessageCallback func)
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
        LisEntry(void* l, MessageCallback f)
        {
            lis = l;
            func = f;
        }
        bool operator==(const LisEntry& r)
        {
            return lis == r.lis && func == r.func;
        }
        void* lis;
        MessageCallback func;
    };
    std::vector<LisEntry> callbacks;
};

#endif
