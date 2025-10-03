cmake_minimum_required(VERSION 3.20)
project(BrickBreaker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Let vcpkg provide these:
find_package(glfw3 CONFIG REQUIRED)         # target: glfw
find_package(GLEW   CONFIG REQUIRED)        # targets: GLEW::GLEW or glew32/glew32s
find_package(SQLite3 REQUIRED)              # target: SQLite::SQLite3
find_package(GTest  CONFIG REQUIRED)        # targets: GTest::gtest GTest::gtest_main

# Your game
add_executable(BrickBreaker
  src/MainCode.cpp
  src/Brick.cpp
  src/Circle.cpp
  src/Paddle.cpp
  src/Collision.cpp
  src/Grid.cpp
  src/PowerUp.cpp
  src/Level.cpp
  src/DataAccess.cpp
  # add remaining .cpp files here
)

target_include_directories(BrickBreaker PRIVATE include)
target_link_libraries(BrickBreaker
  PRIVATE
    glfw
    GLEW::GLEW
    SQLite::SQLite3
    # opengl on each platform:
)

# OpenGL link (platform-specific helpers)
if (WIN32)
  target_link_libraries(BrickBreaker PRIVATE opengl32)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(BrickBreaker PRIVATE OpenGL::GL)
endif()

# Tests (optional)
add_executable(tests tests/collision_tests.cpp tests/db_tests.cpp)
target_link_libraries(tests PRIVATE GTest::gtest_main)
enable_testing()
add_test(NAME all-tests COMMAND tests)
