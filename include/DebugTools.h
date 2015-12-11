#ifndef __DEBUG_TOOLS_H__
#define __DEBUG_TOOLS_H__

#include <iostream>
#include <ctime>
#include <cmath>

//Debug assert (segfault if condition not true)
#define DBASSERT(cond) if(!(cond)) throw std::runtime_error(#cond " not true!");
#define GLERR {auto errcode = glGetError(); if(errcode != GL_NO_ERROR) {std::cout << errcode << std::endl; throw runtime_error("GL error!");}}
#define PRINT(msg) {std::cout << msg << std::endl;}
#define ASSERT_EQ(ex1, ex2) {DBASSERT(fabs((ex1) - (ex2)) < 1e-3)}
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

#endif