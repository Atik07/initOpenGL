#pragma once

#include <GLEW/glew.h>

// kinda non-intuitive to include the ASSERT macro in the renderer 
// since we are going to use this every where not just in the renderer

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__));

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);