cmake_minimum_required(VERSION 3.15)
project(Snake-Game LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Build raylib games" FORCE)
set(RAYLIB_INSTALL OFF CACHE BOOL "Disable raylib install target" FORCE)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(Snake-Game main.cpp button.cpp)
target_include_directories(Snake-Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Snake-Game PRIVATE raylib)

if(APPLE)
  find_library(COCOA Cocoa REQUIRED)
  find_library(OPENGL OpenGL REQUIRED)
  find_library(IOKIT IOKit REQUIRED)
  find_library(COREVIDEO CoreVideo REQUIRED)
  target_link_libraries(Snake-Game PRIVATE ${COCOA} ${OPENGL} ${IOKIT} ${COREVIDEO})

  set_target_properties(Snake-Game PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.snake-game
    MACOSX_BUNDLE_BUNDLE_NAME Snake-Game
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
  )
endif()