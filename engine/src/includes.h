#ifndef INCLUDES_H
#define INCLUDES_H

// Libs
#include <Windows.h>
#include <Windowsx.h>
#include <xinput.h>
#include <glad/glad.h>
#include <GL/wglext.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../libs/lua/src/lua.hpp"
#include "../libs/lua/src/lualib.h"
#include "../libs/lua/src/lauxlib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <map>


// HEADERS files
#include "defines.h"
#include "input.h"
#include "shader.h"
#include "texture.h"
#include "renderer.h"

// CPP files
#include "input.cpp"
#include "shader.cpp"
#include "texture.cpp"
#include "renderer.cpp"

#endif
