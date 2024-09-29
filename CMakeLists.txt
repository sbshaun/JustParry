cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the project name
project(JustParry)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

# adapted from SimpleGL
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(IS_OS_MAC 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(IS_OS_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(IS_OS_WINDOWS 1)
else()
  message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif()

if (IS_OS_MAC)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
  # 2024-09-24 - added for M-series Mac's
  include_directories(/opt/homebrew/include)
  link_directories(/opt/homebrew/lib)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_include_directories(${PROJECT_NAME} PUBLIC external/gl3w)

# Find OpenGL
find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
   target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
   target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY})
endif()

set(glm_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glm/cmake/glm) # if necessary
find_package(glm REQUIRED)

# GLFW  could be precompiled (on windows) or installed by a package manager (on OSX and Linux)
if (IS_OS_LINUX OR IS_OS_MAC)
    # Try to find packages rather than to use the precompiled ones
    # Since we're on OSX or Linux, we can just use pkgconfig.
    find_package(PkgConfig REQUIRED)

    pkg_search_module(GLFW REQUIRED glfw3)

    # Link Frameworks on OSX
    if (IS_OS_MAC)
       find_library(COCOA_LIBRARY Cocoa)
       find_library(CF_LIBRARY CoreFoundation)
       target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY} ${CF_LIBRARY})
    endif()

    elseif (IS_OS_WINDOWS)
        set(GLFW_FOUND TRUE)

        set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")

        if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
            set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3dll-x64.lib")

            set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3-x64.dll")
        else()
            set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3dll-x86.lib")

            set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3-x86.dll")
     endif()

    # Copy and rename dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll")
       
endif()


#END SimpleGL adaptation


# Add Gainput submodule

include_directories(${CMAKE_SOURCE_DIR}/external/gainput/lib/include)
add_subdirectory(external/gainput)
# Find GLEW
find_package(GLEW REQUIRED)

# Link the Gainput, GLFW, and GLEW libraries
target_link_libraries(JustParry PUBLIC gainput glfw GLEW::GLEW)