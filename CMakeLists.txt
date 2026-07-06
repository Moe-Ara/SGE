cmake_minimum_required(VERSION 3.16)

# Try to auto-detect vcpkg toolchain if the user didn't provide one.
# NOTE: CMAKE_TOOLCHAIN_FILE must be set before project() to affect dependency resolution.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE PATH "Vcpkg toolchain file")
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "Vcpkg toolchain file")
  else()
    message(STATUS "vcpkg toolchain file not auto-detected. To use vcpkg, configure with:")
    message(STATUS "  cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
    message(STATUS "Or set the environment variable VCPKG_ROOT to your vcpkg root.")
  endif()
endif()

project(SGE VERSION 0.1.0 LANGUAGES CXX)

# Defaults and policy
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect sources (main.cpp is at repo root)
file(GLOB_RECURSE SGE_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)
file(GLOB_RECURSE SGE_HDR CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(SGE
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    ${SGE_SRC}
    ${SGE_HDR}
)

# Add project root to include path so existing includes like
# #include "src/Core/Application.h" resolve correctly.
target_include_directories(SGE PRIVATE "${CMAKE_SOURCE_DIR}")

# ---------- OpenGL (robust) ----------
find_package(OpenGL REQUIRED)
if(TARGET OpenGL::GL)
  set(_OPENGL_LINK_TARGET OpenGL::GL)
else()
  if(DEFINED OpenGL_GL_LIBRARY)
    set(_OPENGL_LINK_TARGET ${OpenGL_GL_LIBRARY})
  elseif(DEFINED OpenGL_GL_LIBRARIES)
    set(_OPENGL_LINK_TARGET ${OpenGL_GL_LIBRARIES})
  else()
    if(WIN32)
      set(_OPENGL_LINK_TARGET opengl32)
    elseif(APPLE)
      find_library(_OPENGL_FOUND_LIBRARY OpenGL)
      set(_OPENGL_LINK_TARGET ${_OPENGL_FOUND_LIBRARY})
    else()
      set(_OPENGL_LINK_TARGET GL)
    endif()
  endif()
endif()

if(NOT _OPENGL_LINK_TARGET)
  message(FATAL_ERROR "Failed to determine OpenGL link target. Ensure OpenGL is available.")
else()
  message(STATUS "Using OpenGL link target: ${_OPENGL_LINK_TARGET}")
endif()

# ---------- GLFW ----------
# Prefer a CONFIG package (vcpkg) but fall back to a Find module or manual locates.
set(_GLFW_TARGET "")
find_package(glfw3 CONFIG QUIET)
if(TARGET glfw)
  set(_GLFW_TARGET glfw)
elseif(TARGET glfw3::glfw)
  set(_GLFW_TARGET glfw3::glfw)
elseif(TARGET glfw::glfw)
  set(_GLFW_TARGET glfw::glfw)
else()
  find_package(GLFW3 QUIET)
  if(TARGET glfw)
    set(_GLFW_TARGET glfw)
  endif()
endif()

if(NOT _GLFW_TARGET)
  # Try system find as fallback
  find_path(_GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h PATH_SUFFIXES include)
  find_library(_GLFW_LIBRARY NAMES glfw3 glfw PATH_SUFFIXES lib)
  if(_GLFW_INCLUDE_DIR AND _GLFW_LIBRARY)
    message(STATUS "Found GLFW headers in ${_GLFW_INCLUDE_DIR}, library ${_GLFW_LIBRARY}")
    target_include_directories(SGE PRIVATE ${_GLFW_INCLUDE_DIR})
    set(_GLFW_TARGET ${_GLFW_LIBRARY})
  else()
    message(STATUS "GLFW not found. Install via vcpkg: 'vcpkg install glfw3' or your OS package manager.")
    message(FATAL_ERROR "Could not locate GLFW (glfw3). Configure CMake with -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake if using vcpkg.")
  endif()
else()
  message(STATUS "Using GLFW target: ${_GLFW_TARGET}")
endif()

# ---------- GLEW ----------
set(_GLEW_TARGET "")
# try config targets (vcpkg)
find_package(glew CONFIG QUIET)
if(TARGET glew::glew)
  set(_GLEW_TARGET glew::glew)
else()
  # try FindGLEW
  find_package(GLEW QUIET)
  if(TARGET GLEW::GLEW)
    set(_GLEW_TARGET GLEW::GLEW)
  endif()
endif()

if(NOT _GLEW_TARGET)
  # fallback: try to locate headers and library manually
  find_path(_GLEW_INCLUDE_DIR NAMES GL/glew.h PATHS
    $ENV{VCPKG_ROOT}/installed
    ${CMAKE_SOURCE_DIR}/vcpkg/installed
    /usr/include
    /usr/local/include
    NO_DEFAULT_PATH
  )
  find_library(_GLEW_LIBRARY NAMES GLEW glew glew32 glew32s PATHS
    $ENV{VCPKG_ROOT}/installed
    ${CMAKE_SOURCE_DIR}/vcpkg/installed
    /usr/lib
    /usr/local/lib
    NO_DEFAULT_PATH
  )
  if(NOT _GLEW_INCLUDE_DIR)
    find_path(_GLEW_INCLUDE_DIR GL/glew.h)
  endif()
  if(NOT _GLEW_LIBRARY)
    find_library(_GLEW_LIBRARY NAMES GLEW glew glew32 glew32s)
  endif()

  if(_GLEW_INCLUDE_DIR AND _GLEW_LIBRARY)
    message(STATUS "Found GLEW include: ${_GLEW_INCLUDE_DIR}, lib: ${_GLEW_LIBRARY}")
    target_include_directories(SGE PRIVATE ${_GLEW_INCLUDE_DIR})
    set(_GLEW_TARGET ${_GLEW_LIBRARY})
  else()
    message(STATUS "GLEW not found. Install via vcpkg: 'vcpkg install glew' or use your OS package manager.")
    message(FATAL_ERROR "Could not locate GLEW. Configure CMake with -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake if using vcpkg.")
  endif()
endif()

# ---------- GLM (header-only) ----------
find_package(glm CONFIG QUIET)
if(TARGET glm::glm)
  set(_GLM_TARGET glm::glm)
  message(STATUS "Found glm target: glm::glm")
else()
  find_path(_GLM_INCLUDE_DIR glm/glm.hpp
    HINTS $ENV{VCPKG_ROOT}/installed ${CMAKE_SOURCE_DIR}/vcpkg/installed /usr/include /usr/local/include
  )
  if(NOT _GLM_INCLUDE_DIR)
    find_path(_GLM_INCLUDE_DIR glm/glm.hpp)
  endif()
  if(_GLM_INCLUDE_DIR)
    message(STATUS "Found glm headers at: ${_GLM_INCLUDE_DIR}")
    target_include_directories(SGE PRIVATE ${_GLM_INCLUDE_DIR})
  else()
    message(FATAL_ERROR "glm not found. Install via vcpkg: 'vcpkg install glm' or your OS package manager.")
  endif()
endif()

# ---------- Link libraries ----------
if(TARGET OpenGL::GL)
  target_link_libraries(SGE PRIVATE OpenGL::GL)
else()
  target_link_libraries(SGE PRIVATE ${_OPENGL_LINK_TARGET})
endif()

# Link GLEW (can be a target or a library path)
if(TARGET ${_GLEW_TARGET})
  target_link_libraries(SGE PRIVATE ${_GLEW_TARGET})
else()
  target_link_libraries(SGE PRIVATE ${_GLEW_TARGET})
endif()

# Link GLFW (target or library)
if(TARGET ${_GLFW_TARGET})
  target_link_libraries(SGE PRIVATE ${_GLFW_TARGET})
else()
  target_link_libraries(SGE PRIVATE ${_GLFW_TARGET})
endif()

# Link glm target if available (header-only target)
if(_GLM_TARGET)
  target_link_libraries(SGE PRIVATE ${_GLM_TARGET})
endif()

# ----------- Copy resources (shaders, models) to the executable directory after build ----------
# Ensures runtime can open files like "resources/shaders/vertex.vert" and "resources/models/cube.obj"
add_custom_command(TARGET SGE POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    $<TARGET_FILE_DIR:SGE>/resources
)

# Compiler warnings (portable)
if(MSVC)
  target_compile_options(SGE PRIVATE /W4 /permissive-)
else()
  target_compile_options(SGE PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable generation of compile_commands.json for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Configured SGE (executable target: SGE)")
message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")