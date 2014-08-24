#pragma once

#ifndef __APPLE__
#    include "generated/flextGL.h"
#else
#    define GLFW_INCLUDE_GLCOREARB
#    define flextInit
#    include "GLFW/glfw3.h"
#endif