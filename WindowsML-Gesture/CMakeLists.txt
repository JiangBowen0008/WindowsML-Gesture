# CMakeList.txt : CMake project for WindowsML-Gesture, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.1)

# Add source to this project's executable.
add_executable (WindowsML-Gesture "WindowsML-Gesture.cpp" ${aux_src} ${cppflow_path}/src/Model.cpp ${cppflow_path}/src/Tensor.cpp)
message(${TENSORFLOW_LIB})
target_link_libraries(WindowsML-Gesture ${TENSORFLOW_LIB} Eigen3::Eigen)

# TODO: Add tests and install targets if needed.
