cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/external/)

# Set the project name
project(JustParry)
file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp)

# adapted from SimpleGL
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find OS
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(IS_OS_MAC 1)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(IS_OS_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(IS_OS_WINDOWS 1)
else()
  message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif()

# Generate the shader folder location to the header
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/external/project_path.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/external/project_path.hpp")

if (IS_OS_MAC)
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
  # 2024-09-24 - added for M-series Mac's
  if(EXISTS "/opt/homebrew")
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
  endif()
endif()

add_executable(${PROJECT_NAME} ${SOURCES}  "src/linearinterp.cpp" "src/state/game.cpp" "src/state/game.hpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})

# External header-only libs
target_include_directories(${PROJECT_NAME} PUBLIC external/stb_image/)
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
    pkg_search_module(SDL2 REQUIRED sdl2)
    pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)

    # check compilation for this
    pkg_search_module(FREETYPE REQUIRED freetype2)

    # Link Frameworks on OSX
    if (IS_OS_MAC)
       find_library(COCOA_LIBRARY Cocoa)
       find_library(CF_LIBRARY CoreFoundation)
       target_link_libraries(${PROJECT_NAME} PUBLIC ${COCOA_LIBRARY} ${CF_LIBRARY})
    endif()
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2MIXER_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2MIXER_LIBRARIES})

elseif (IS_OS_WINDOWS)
    set(GLFW_FOUND TRUE)
    set(SDL2_FOUND TRUE)

    set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/include/SDL")

    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3dll-x64.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2-x64.lib")
	    set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2_mixer-x64.lib")

        set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3-x64.dll")
        set(SDL2_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2-x64.dll")
	    set(SDL2MIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2_mixer-x64.dll")
    else()
        set(GLFW_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3dll-x86.lib")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2-x86.lib")
        set(SDL2MIXER_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2_mixer-x86.lib")

        set(GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib/glfw3-x86.dll")
        set(SDL2_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2-x86.dll")
	    set(SDL2MIXER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/external/sdl/lib/SDL2_mixer-x86.dll")
    endif()

    set (FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include")
    set (FREETYPE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/release static/vs2015-2022/win64/freetype.lib")

    # Copy and rename dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2MIXER_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2_mixer.dll"
    )

endif()

if (NOT GLFW_FOUND OR NOT SDL2_FOUND)
   if (NOT GLFW_FOUND)
      message(FATAL_ERROR "Can't find GLFW." )
   else ()
      message(FATAL_ERROR "Can't find SDL2." )
   endif()
endif()

find_package(Freetype REQUIRED)

if(TARGET Freetype AND NOT TARGET Freetype::Freetype)
     add_library(Freetype::Freetype ALIAS freetype) # target freetype is defined by freetype-targets.cmake
     # might need to add freetype to global scope if cmake errors here
     # alternativly if the above does not work for you you can use
     # add_library(Freetype::Freetype INTERFACE IMPORTED)
     # target_link_libraries(Freetype::Freetype INTERFACE freetype)
endif()
 
if(NOT TARGET Freetype::Freetype)
     # insert error here
     # or create the target correctly (see cmakes newer FindFreetype.cmake)
     message(FATAL_ERROR "Can't find FreeType (fonts)." )
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include")

target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBRARIES} glm::glm ${FREETYPE_LIBRARY})

if (NOT SDL2_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
endif()

if (NOT SDL2MIXER_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2MIXER_LIBRARIES})
endif()

# Needed to add this
if(IS_OS_LINUX)
  target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${CMAKE_DL_LIBS})
endif()

#END SimpleGL adaptation
