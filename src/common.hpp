#pragma once

// standard libs
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <sstream>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// glt
#define GLT_IMPLEMENTATION
#include <GLText.h>

// sdl
#include <SDL.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../external/project_path.hpp"
using namespace glm;

bool gl_has_errors();