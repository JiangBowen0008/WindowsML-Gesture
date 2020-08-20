# WindowsML-Gesture
 ML-based hand gesture recognition with Lenovo laptops.
 
**Table of Contents**
- [WindowsML-Gesture](#windowsml-gesture)
 * [Supported Platforms](#supported-platforms)
 * [Step-by-step Compilation Guide](#step-by-step-compilation-guide)
    
## Supported Platforms
- Windows
- Linux (Not Tested)
- Mac (Not Tested)

## Step-by-step Compilation Guide 
 ### 1. Git Clone
 - If you do not have a compiled version of Eigen, use 
 `git clone --recursive [url]`
 - Otherwise, use
 `git clone [url]`

 ### 2. Download PyTorch C++ Frontend
 Download Pytorch C++ Frontend from (choose one)
 
 Release: https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.6.0%2Bcpu.zip
 
 Debug: https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-debug-1.6.0%2Bcpu.zip

 Unzip the downloaded file and copy the **libTorch** folder into the **extern** folder of this project.

 ### 3. Download CMake
 Download and install CMake from https://cmake.org/download/

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
    
## Customizing VADNet and GestNet

### NN Requirements

#### VADNet

- Input: Size 1 * 1 * 11. See **featureTest.m** inside the **scripts** folder for the composition of the input.

- Output: Size 2. Probabilities for 

   + **0:** No Gesture
   + **1:** Gesture

#### GestNet

- Input: Size WindowSize(by default 72) * 2 * 136. Raw SFT results inside the window duration. To use a different window size, change the 'WINDOW_SIZE' in **utils.hpp**.

- Output: Size 4. Probabilities for

  + **0:** No Gesture
  + **1:** Left Swing
  + **2:** Right Swing
  + **3:** Double Click





