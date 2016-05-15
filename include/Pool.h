#include <vector>
#include <stdexcept>
#include "DebugTools.h"

using std::vector;
using std::runtime_error;

/*  Simple fixed-size pool allocator for fast
 *  allocation of individual objects.
 *  Allows for int as a pointer (index into pool)
 *  and STL-style iteration.
 */

template<typename T>
class Pool
{
    friend struct iterator;
    public:
        Pool(int capacity = 64);
        ~Pool();
        T& operator[](int index);
        int alloc();
        int alloc(T val);
        void allocIndex(int index);
        void dealloc(int index);
        void dealloc(T* ptr);
        void clear();
        struct iterator
        {
            int loc;
            Pool& owner;
            iterator operator++(int);   //postfix increment requires arg for no reason
            T& operator*();
            T* operator->();
            bool operator==(const iterator& rhs);
            bool operator!=(const iterator& rhs);
        };
        bool isAllocated(int index);
        iterator begin();
        iterator end();
        int size;
        int capacity;
    private:
        vector<bool> allocMap;
        vector<int> freeList;
        T* data;
        int top;
};

template<typename T>
Pool<T>::Pool(int capacity) : allocMap(capacity, false)
{
    this->capacity = capacity;
    size = 0;
    top = 0;
    freeList.reserve(100);
    data = (T*) malloc(capacity * sizeof(T));
}

template<typename T>
Pool<T>::~Pool()
{
    //call destructor on all allocated objects
    for(auto it = begin(); it != end(); it++)
    {
        it->~T();
    }
    free(data);
}

template<typename T>
T& Pool<T>::operator[](int index)
{
#ifdef MAGNATE_DEBUG
    if(index < 0 || index >= capacity || !allocMap[index])
    {   
        throw runtime_error("Illegal pool access!");
    }
#endif
    return data[index];
}

template<typename T>
int Pool<T>::alloc()
{
    //Prioritize filling gaps to minimize fragmentation
    int loc = -1;
    if(freeList.size())
    {
        loc = freeList.back();
        freeList.pop_back();
    }
    else if(top < capacity)
    {
        loc = top;
        top++;
    }
#ifdef MAGNATE_DEBUG
    if(loc == -1)
        throw runtime_error("Pool allocation failed!");
#endif
    allocMap[loc] = true;
    //intialize the location with default constructor (required for RAII to work)
    new (&data[loc]) T;
    size++;
    return loc;
}

template<typename T>
int Pool<T>::alloc(T val)
{
    int loc = alloc();
    data[loc] = val;
    return loc;
}

template<typename T>
void Pool<T>::allocIndex(int index)
{
    allocMap[index] = true;
    size++;
    new(&data[index]) T;
}

template<typename T>
void Pool<T>::dealloc(int index)
{
    data[index].~T();
    allocMap[index] = false;
    if(index != top - 1)
    {
        freeList.push_back(index);
    }
    else
    {
        top--;
    }
    size--;
}

template<typename T>
void Pool<T>::dealloc(T* ptr)
{
    ptrdiff_t offset = (ptrdiff_t) ptr;
    ptrdiff_t base = (ptrdiff_t) data;
    PRINT("offset: " << offset);
    PRINT("base: " << base);
    dealloc((offset - base) / sizeof(T));
}

template<typename T>
void Pool<T>::clear()
{
    for(size_t i = 0; i < allocMap.size(); i++)
    {
        allocMap[i] = false;
    }
    freeList.clear();
    size = 0;
    top = 0;
}
    
template<typename T>
typename Pool<T>::iterator Pool<T>::iterator::operator++(int)
{
    do
    {
        loc++;
    }
    while(!owner.allocMap[loc] && loc < owner.capacity);
    //returned iterator will be at next allocated position, or end()
    return *this;
}

template<typename T>
T& Pool<T>::iterator::operator*()
{
#ifdef MAGNATE_DEBUG
    if(!owner.allocMap[loc])
    {
        throw runtime_error("Attempted to deref pool iter at unallocated position.");
    }
#endif
    return owner.data[loc];
}

template<typename T>
T* Pool<T>::iterator::operator->()
{
#ifdef MAGNATE_DEBUG
    if(!owner.allocMap[loc])
    {
        throw runtime_error("Attempted to deref pool iter at unallocated position.");
    }
#endif
    return &owner.data[loc];
}

template<typename T>
typename Pool<T>::iterator Pool<T>::begin()
{
    Pool<T>::iterator it = {0, *this};
    while(!allocMap[it.loc] && it.loc < capacity)
    {
        it.loc++;
    }
    return it;
}

template<typename T>
typename Pool<T>::iterator Pool<T>::end()
{
    return {capacity, *this};
}

template<typename T>
bool Pool<T>::iterator::operator==(const Pool<T>::iterator& rhs)
{
    //iterators must be for same pool, and must 
    return &owner == &rhs.owner && loc == rhs.loc;
}

template<typename T>
bool Pool<T>::iterator::operator!=(const Pool<T>::iterator& rhs)
{
    return !(*this == rhs);
}

template<typename T>
bool Pool<T>::isAllocated(int index)
{
    return allocMap[index];
}
