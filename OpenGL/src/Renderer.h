#pragma once
#include<GL/glew.h>
//macro
#define ASSERT(x) if (!(x)) __debugbreak(); //breaks exaclty where error occurs
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)) //says exaclty in which file, function and line the error is


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
