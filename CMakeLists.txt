# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.1)

project ("WindowsML-Gesture"
		LANGUAGES CXX
		VERSION 1.0)

SET(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

# include eigen
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
include_directories(${CMAKE_CURRENT_LIST_DIR}/extern/eigen-master)
link_libraries(Eigen3::Eigen)

# include tensorflow
set(tf_path ${CMAKE_CURRENT_LIST_DIR}/extern/libtensorflow-cpu-windows)
find_library(TENSORFLOW_LIB tensorflow HINT ${tf_path}/lib REQUIRED ABSOLUTE)
include_directories(${tf_path}/include)
link_directories(${tf_path}/lib)

# include cppflow
set(cppflow_path ${CMAKE_CURRENT_LIST_DIR}/extern/cppflow)
include_directories(${cppflow_path}/include)
# aux_source_directory(${cppflow_path}/src aux_src)

# local files
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src aux_src)

# Include sub-projects.
message(${TENSORFLOW_LIB})
add_subdirectory ("WindowsML-Gesture")
