﻿# CMakeList.txt : CMake project for WindowsML-Gesture, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.1)

# Add source to this project's executable.
add_executable (WindowsML-Gesture WindowsML-Gesture.cpp ${aux_src})
target_link_libraries(WindowsML-Gesture Eigen3::Eigen rtaudio dsound ${TORCH_LIBRARIES})

add_custom_command(
  TARGET WindowsML-Gesture POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${file_copy}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)

