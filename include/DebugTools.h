#ifndef __DEBUG_TOOLS_H__
#define __DEBUG_TOOLS_H__

#include <iostream>
#include <ctime>
#include <cmath>

//Global debug switch
#define MAGNATE_DEBUG

#ifdef MAGNATE_DEBUG
//Debug assert (segfault if condition not true)
#define DBASSERT(cond) if(!(cond)) throw std::runtime_error(#cond " not true!");
#define GLERR {auto errcode = glGetError(); if(errcode != GL_NO_ERROR) {throw runtime_error("GL error!");}}
#define PRINT(msg) {std::cout << msg << std::endl;}
#define ASSERT_EQ(ex1, ex2) {DBASSERT(fabs((ex1) - (ex2)) < 1e-3)}
#define PRINTVAR(a) {std::cout << #a << " = " << a << endl;};
#define TIMEIT(func) \
{ \
    clock_t _start = clock(); \
    func; \
    int n = printf("%s", #func); \
    for(int i = 0; i < 60 - n; i++) \
        printf("."); \
    double sec = double(clock() - _start) / CLOCKS_PER_SEC; \
    printf("%.3f sec.\n", sec); \
}
#else   //provide empty definitinos for the macros

#define DBASSERT(cond)
#define GLERR
#define PRINT(msg)
#define ASSERT_EQ(ex1, ex2)
#define TIMEIT(func) func;  //need to call the function, but don't do timing/output
#define PRINTVAR(a)

#endif

#endif
