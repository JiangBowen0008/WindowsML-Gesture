# WindowsML-Gesture
 ML-based hand gesture recognition with Lenovo laptops.
 
## Supported Platforms
- Windows
- Linux (Not Tested)
- Mac (Not Tested)

## Step-by-step Compilation Guide 
 ### 1.Git Clone
 - If you do not have a compiled version of Eigen, use 
 `git clone --recursive [url]`
 - Otherwise, use
 `git clone [url]`

 ### 2. Download PyTorch C++ Frontend
 Download Pytorch C++ Frontend from (choose one)
 
 - Release: https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.6.0%2Bcpu.zip
 
 - Debug: https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-debug-1.6.0%2Bcpu.zip

 Unzip the downloaded file and copy the **libTorch** folder into the **extern** folder of this project.

 ### 3. Download CMake
 Download and install CMake
 https://cmake.org/download/

 ### 4. Compile Eigen
 If you have an installed version of Eigen, skip to step 5.
 1. Create a **build** folder inside the Eigen repo root directory.
 2. Generate the Visual Studio project files (makefiles for Linux) using CMake inside the **build** folder.
 3. For Windows users: open the project and build in Visual Studio.
    For Linux users: call `make`
 
 ### 5. Compile WindowsML-Gesture
 1. Create a **build** folder inside the repo root directory.
 2. Generate the Visual Studio project files (makefiles for Linux) using CMake inside the **build** folder.
 3. For Windows users: open the project and build in Visual Studio.
    For Linux users: call `make`


