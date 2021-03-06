﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.1)

project ("WindowsML-Gesture"
		LANGUAGES CXX
		VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

# include eigen
# set(Eigen3_DIR ${CMAKE_CURRENT_LIST_DIR}/extern/eigen/cmake)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
include_directories(${CMAKE_CURRENT_LIST_DIR}/extern/eigen)

# include tensorflow (uncomment and download necessary library to use tensorflow)
# set(tf_path ${CMAKE_CURRENT_LIST_DIR}/extern/libtensorflow-cpu-windows)
# find_library(TENSORFLOW_LIB tensorflow HINT ${CMAKE_CURRENT_LIST_DIR}/lib)
# include_directories(${tf_path}/include)
# link_directories(${tf_path}/lib)

# include cppflow (uncomment it when using tensorflow)
# set(cppflow_path ${CMAKE_CURRENT_LIST_DIR}/extern/cppflow)
# include_directories(${cppflow_path}/include)
# aux_source_directory(${cppflow_path}/src aux_src)

# include torch
set(torch_path ${CMAKE_CURRENT_LIST_DIR}/extern/libtorch)

set(CMAKE_PREFIX_PATH ${torch_path})
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# include onnx runtime (uncomment and download necessary library to use ONNX runtime)
# set(onnx_path ${CMAKE_CURRENT_LIST_DIR}/extern/onnxruntime-win-x64-1.4.0)
# include_directories(${onnx_path}/incldue)
# find_library(ONNX_LIB onnxruntime HINT ${onnx_path}/lib)
# link_directories(${onnx_path}/lib)
# message(${ONNX_LIB})

# include local source files
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src aux_src)

# copy the files we need
file(GLOB lib_copy "${CMAKE_CURRENT_LIST_DIR}/lib/*.dll")
file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
file(GLOB asset_copy "${CMAKE_CURRENT_LIST_DIR}/assets/*")
set(file_copy ${lib_copy} ${asset_copy} ${TORCH_DLLS})
message(${file_copy})

# Include the main project folder.
add_subdirectory ("WindowsML-Gesture")

# set as the startup project in VS
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WindowsML-Gesture)




