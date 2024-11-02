#pragma once

// standard libs
#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <memory>
// #include <bits/unique_ptr.h>


#include "constants.hpp"

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// glt
#define GLT_IMPLEMENTATION
#include <GLText.h>

// sdl - Need the SDL_MAIN_HANDLED macro to prevent LNK error
#define SDL_MAIN_HANDLED
#include <SDL.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace glm;

//path stuff
#include "../external/project_path.hpp"
inline std::string asset_path() { return std::string(PROJECT_SOURCE_DIR) + "assets"; };
inline std::string textures_path(const std::string& name) {return asset_path() + "/textures/" + std::string(name);};
inline std::string mesh_path(const std::string& name) {return asset_path() + "/meshes/" + std::string(name);};

bool gl_has_errors();