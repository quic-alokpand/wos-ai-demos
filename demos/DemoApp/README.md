
# Demo App using QNN for Windows on Snapdragon

# Table of Contents
1. [Pre-requisites](#1)
   1. [Install Visual Studio 2022](#1.1)
   1. [Install QNN SDK](#1.2)

2. [Build app](#2)

3. [Run app](#3)
   1. [Install VC runtime](#3.1)
   1. [Install app](#3.2)

## 1. Pre-requisites
<div id="1" ></div>

### 1.1 Install Visual Studio 2022
<div id="1.1" ></div>
Install Visual Studio 2022. Install additional components by importing components.vsconfig configuration into Visual Studio installer.

### 1.2 Install QNN SDK
<div id="1.2" ></div>
Install QNN 2.21.0 SDK from https://developer.qualcomm.com/software/qualcomm-ai-engine-direct-sdk


## 2. Build app
<div id="2" ></div>

Run the below command to build the app. Pass the QNN SDK path as an argument.
```
.\build.ps1 "C:\Qualcomm\AIStack\QAIRT\2.21.0.240401"
```

The app will be generated at .\build\demo-snapdragon


## 3. Install app on Windows on Snapdragon
<div id="3" ></div>

### 3.1 Install VC runtime
<div id="3.1" ></div>
Install vc runtime on WOS from https://aka.ms/vs/17/release/vc_redist.arm64.exe

### 3.3 Install app
<div id="3.2" ></div>
Copy the generated app folder .\build\demo-snapdragon to desired location
on Windows on Snapdragon machine. Run qnn-demo-app.exe for demo.
