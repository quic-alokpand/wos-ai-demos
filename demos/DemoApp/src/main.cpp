//==============================================================================
//
//  @@-COPYRIGHT-START-@@
//
//  Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
//  SPDX-License-Identifier: BSD-3-Clause
//
//  @@-COPYRIGHT-END-@@
//
//==============================================================================

#include <iostream>
#include <memory>
#include <string>

#include "BuildId.hpp"
#include "DynamicLoadUtil.hpp"
#include "Logger.hpp"
#include "Models/ModelFactory.hpp"
#include "Models/ModelUtils.hpp"
#include "PAL/DynamicLoading.hpp"
#include "PAL/GetOpt.hpp"
#include "QnnSampleApp.hpp"
#include "QnnSampleAppUtils.hpp"

#include <opencv2/highgui.hpp>

static void *sg_backendHandle{nullptr};
static void *sg_modelHandle{nullptr};

namespace qnn {
namespace tools {
namespace sample_app {

void showHelp() {
  std::cout
      << "\nDESCRIPTION:\n"
      << "------------\n"
      << "Demo application demonstrating how to load and execute a neural "
         "network\n"
      << "using QNN APIs with pre and post processing.\n"
      << "\n\n"
      << "REQUIRED ARGUMENTS:\n"
      << "-------------------\n"
      << "  --model_name        <VAL>       model name related to the model "
         "path.\n"
         "                                  1. DMSHN\n"
         "                                  2. DeepLabV3\n"
         "                                  3. Midas_v2\n"
         "                                  4. DexiNed\n"
         "                                  5. QuickSRNet\n"
         "                                  6. HRNet\n"
         "                                  7. DeepFillV2\n"
         "                                  8. HeadPose\n"
         "                                  9. OpenPose\n"
      << "  --input             <FILE>      Path to input for the network. "
         "--input img.jpg\n"
         "                                  If the graph takes multiple "
         "inputs, then send the inputs\n"
         "                                  separated by comma.\n"
         "                                  i.e., --input "
         ".\\Data\\deepfill_v2\\1.png,.\\Data\\deepfill_v2\\1_mask.png\n"
         "                                  This switch can be used multiple "
         "times to make more than one\n"
         "                                  inference.\n"
      << "\n"
      << "  --retrieve_context  <VAL>       Path to cached binary from which "
         "to load a saved\n"
         "                                  context from and execute graphs"
      << "\n\n"

      << "OPTIONAL ARGUMENTS:\n"
      << "-------------------\n"

      << "  --debug                         Specifies that output from all "
         "layers of the network\n"
      << "                                  will be saved.\n"
      << "\n"
      << "  --output_dir        <DIR>       The directory to save output to. "
         "Defaults to "
         "./output.\n"
      << "\n"
      << "  --profiling_level   <VAL>       Enable profiling. Valid Values:\n"
         "                                    1. basic:    captures execution "
         "and init time.\n"
         "                                    2. detailed: in addition to "
         "basic, captures\n"
         "                                                 per Op timing for "
         "execution.\n"
      << "  --kpi                           Print the KPI info.\n"
      << "\n"
#ifdef QNN_ENABLE_DEBUG
      << "  --log_level                     Specifies max logging level to be "
         "set.  Valid "
         "settings: \n"
         "                                 \"error\", \"warn\", \"info\", "
         "\"verbose\" and "
         "\"debug\"."
         "\n"
#else
      << "  --log_level                     Specifies max logging level to be "
         "set.  Valid "
         "settings: \n"
         "                                 \"error\", \"warn\", \"info\" and "
         "\"verbose\"."
         "\n"
#endif
      << "\n"
      << "  --version                       Print the QNN SDK version.\n"
      << "\n"
      << "  --help                          Show this help message.\n"
      << std::endl;
}

void showHelpAndExit(std::string &&error) {
  std::cerr << "ERROR: " << error << "\n";
  std::cerr << "Please check help below:\n";
  showHelp();
  std::exit(EXIT_FAILURE);
}

std::unique_ptr<sample_app::QnnSampleApp>
processCommandLine(int argc, char **argv, bool &loadFromCachedBinary) {
  enum OPTIONS {
    OPT_HELP = 0,
    OPT_MODEL_NAME = 1,
    OPT_INPUT = 2,
    OPT_OUTPUT_DIR = 3,
    OPT_DEBUG_OUTPUTS = 4,
    OPT_OUTPUT_DATA_TYPE = 5,
    OPT_LOG_LEVEL = 6,
    OPT_PROFILING_LEVEL = 7,
    OPT_RETRIEVE_CONTEXT = 8,
    OPT_VERSION = 9,
    OPT_KPI = 10
  };

  // Create the command line options
  static struct pal::Option s_longOptions[] = {
      {"help", pal::no_argument, NULL, OPT_HELP},
      {"model_name", pal::required_argument, NULL, OPT_MODEL_NAME},
      {"input", pal::required_argument, NULL, OPT_INPUT},
      {"output_dir", pal::required_argument, NULL, OPT_OUTPUT_DIR},
      {"debug", pal::no_argument, NULL, OPT_DEBUG_OUTPUTS},
      {"output_data_type", pal::required_argument, NULL, OPT_OUTPUT_DATA_TYPE},
      {"log_level", pal::required_argument, NULL, OPT_LOG_LEVEL},
      {"profiling_level", pal::required_argument, NULL, OPT_PROFILING_LEVEL},
      {"retrieve_context", pal::required_argument, NULL, OPT_RETRIEVE_CONTEXT},
      {"version", pal::no_argument, NULL, OPT_VERSION},
      {"kpi", pal::no_argument, NULL, OPT_KPI},
      {NULL, 0, NULL, 0}};

  // Command line parsing loop
  int longIndex = 0;
  int opt = 0;
  std::string modelName;
  std::string modelPath;
  std::string backEndPath = "QnnHtp.dll";
  vector<std::string> inputArgs;
  bool debug = false;
  std::string outputPath;
  std::string opPackagePaths;
  iotensor::OutputDataType parsedOutputDataType =
      iotensor::OutputDataType::FLOAT_ONLY;
  iotensor::InputDataType parsedInputDataType = iotensor::InputDataType::NATIVE;
  sample_app::ProfilingLevel parsedProfilingLevel = ProfilingLevel::OFF;
  bool dumpOutputs = true;
  bool needKpi = false;
  std::string cachedBinaryPath;
  std::string saveBinaryName;
  QnnLog_Level_t logLevel{QNN_LOG_LEVEL_ERROR};
  std::string systemLibraryPath = "QnnSystem.dll";
  ModelFactory *modelFactory = model_utils::getModelFactory();

  while ((opt = pal::getOptLongOnly(argc, argv, "", s_longOptions,
                                    &longIndex)) != -1) {
    switch (opt) {
    case OPT_HELP:
      showHelp();
      std::exit(EXIT_SUCCESS);
      break;

    case OPT_VERSION:
      std::cout << "QNN SDK " << qnn::tools::getBuildId() << "\n";
      std::exit(EXIT_SUCCESS);
      break;

    case OPT_MODEL_NAME:
      modelName = pal::g_optArg;
      std::transform(modelName.begin(), modelName.end(), modelName.begin(),
                     [](unsigned char uc) { return std::tolower(uc); });
      std::cout << "model name " << modelName << std::endl;

      if (!modelFactory->hasModel(modelName)) {
        std::vector<std::string> err = {"Model with name=" + modelName +
                                        " is not supported!"};
        showHelpAndExit(&err[0][0]);
      }
      break;

    case OPT_INPUT:
      inputArgs.push_back(pal::g_optArg);
      break;

    case OPT_DEBUG_OUTPUTS:
      debug = true;
      break;

    case OPT_OUTPUT_DIR:
      outputPath = pal::g_optArg;
      break;

    case OPT_OUTPUT_DATA_TYPE:
      parsedOutputDataType = iotensor::parseOutputDataType(pal::g_optArg);
      if (parsedOutputDataType == iotensor::OutputDataType::INVALID) {
        showHelpAndExit("Invalid output data type string.");
      }
      break;

    case OPT_PROFILING_LEVEL:
      parsedProfilingLevel = sample_app::parseProfilingLevel(pal::g_optArg);
      if (parsedProfilingLevel == sample_app::ProfilingLevel::INVALID) {
        showHelpAndExit("Invalid profiling level.");
      }
      break;

    case OPT_LOG_LEVEL:
      logLevel = sample_app::parseLogLevel(pal::g_optArg);
      if (logLevel != QNN_LOG_LEVEL_MAX) {
        if (!log::setLogLevel(logLevel)) {
          showHelpAndExit("Unable to set log level.");
        }
      }
      break;

    case OPT_RETRIEVE_CONTEXT:
      loadFromCachedBinary = true;
      cachedBinaryPath = pal::g_optArg;
      if (cachedBinaryPath.empty()) {
        showHelpAndExit("Cached context binary file not specified.");
      }
      break;
    case OPT_KPI:
      needKpi = true;
      break;

    default:
      std::cerr
          << "ERROR: Invalid argument passed: " << argv[pal::g_optInd - 1]
          << "\nPlease check the Arguments section in the description below.\n";
      showHelp();
      std::exit(EXIT_FAILURE);
    }
  }

  if (!modelPath.empty()) {
    if (!cachedBinaryPath.empty()) {
      showHelpAndExit("Error: both --model and --cached_binary specified. Only "
                      "one option is valid at a "
                      "time.\n");
    }
  } else {
    if (cachedBinaryPath.empty()) {
      showHelpAndExit("Missing option: --model\n");
    }
  }

  if (!cachedBinaryPath.empty() && !saveBinaryName.empty()) {
    showHelpAndExit("Error: both --cached_binary and --save_binary specified");
  }

  if (modelName.empty()) {
    showHelpAndExit("Missing option: --model_name\n");
  }

  if (inputArgs.empty()) {
    showHelpAndExit("Missing option: --input\n");
  }

  if (loadFromCachedBinary && systemLibraryPath.empty()) {
    showHelpAndExit("Missing option: --system_library. QNN System shared "
                    "library (libQnnSystem.so) is needed "
                    "to load from a cached binary\n");
  }

  QNN_INFO("Model: %s", modelPath.c_str());
  QNN_INFO("Backend: %s", backEndPath.c_str());

  QnnFunctionPointers qnnFunctionPointers;
  // Load backend and model .so and validate all the required function symbols
  // are resolved
  auto statusCode = dynamicloadutil::getQnnFunctionPointers(
      backEndPath, modelPath, &qnnFunctionPointers, &sg_backendHandle,
      !loadFromCachedBinary, &sg_modelHandle);
  if (dynamicloadutil::StatusCode::SUCCESS != statusCode) {
    if (dynamicloadutil::StatusCode::FAIL_LOAD_BACKEND == statusCode) {
      exitWithMessage(
          "Error initializing QNN Function Pointers: could not load backend: " +
              backEndPath,
          EXIT_FAILURE);
    } else if (dynamicloadutil::StatusCode::FAIL_LOAD_MODEL == statusCode) {
      exitWithMessage(
          "Error initializing QNN Function Pointers: could not load model: " +
              modelPath,
          EXIT_FAILURE);
    } else {
      exitWithMessage("Error initializing QNN Function Pointers", EXIT_FAILURE);
    }
  }

  if (loadFromCachedBinary) {
    statusCode = dynamicloadutil::getQnnSystemFunctionPointers(
        systemLibraryPath, &qnnFunctionPointers);
    if (dynamicloadutil::StatusCode::SUCCESS != statusCode) {
      exitWithMessage("Error initializing QNN System Function Pointers",
                      EXIT_FAILURE);
    }
  }

  Model *model = modelFactory->createInstance(modelName);

  std::unique_ptr<sample_app::QnnSampleApp> app(new sample_app::QnnSampleApp(
      model, qnnFunctionPointers, inputArgs, opPackagePaths, sg_backendHandle,
      outputPath, debug, parsedOutputDataType, parsedInputDataType,
      parsedProfilingLevel, dumpOutputs, cachedBinaryPath, saveBinaryName,
      needKpi));
  return app;
}

} // namespace sample_app
} // namespace tools
} // namespace qnn

int main(int argc, char **argv) {
  using namespace qnn::tools;

  if (!qnn::log::initializeLogging()) {
    std::cerr << "ERROR: Unable to initialize logging!\n";
    return EXIT_FAILURE;
  }

  {
    bool loadFromCachedBinary{false};
    std::unique_ptr<sample_app::QnnSampleApp> app =
        sample_app::processCommandLine(argc, argv, loadFromCachedBinary);

    if (nullptr == app) {
      return EXIT_FAILURE;
    }

    QNN_INFO("qnn-sample-app build version: %s",
             qnn::tools::getBuildId().c_str());
    QNN_INFO("Backend        build version: %s",
             app->getBackendBuildId().c_str());

    app->m_model->model_load_start_time =
        std::chrono::high_resolution_clock::now();
    if (sample_app::StatusCode::SUCCESS != app->initialize()) {
      return app->reportError("Initialization failure");
    }

    if (sample_app::StatusCode::SUCCESS != app->initializeBackend()) {
      return app->reportError("Backend Initialization failure");
    }

    auto devicePropertySupportStatus = app->isDevicePropertySupported();
    if (sample_app::StatusCode::FAILURE != devicePropertySupportStatus) {
      auto createDeviceStatus = app->createDevice();
      if (sample_app::StatusCode::SUCCESS != createDeviceStatus) {
        return app->reportError("Device Creation failure");
      }
    }

    if (sample_app::StatusCode::SUCCESS != app->createPowerConfigId()) {
      return app->reportError("Create power config failure");
    }

    if (sample_app::StatusCode::SUCCESS != app->setPowerConfig()) {
      return app->reportError("Set power config failure");
    }

    if (sample_app::StatusCode::SUCCESS != app->initializeProfiling()) {
      return app->reportError("Profiling Initialization failure");
    }
    app->m_model->model_load_stop_time =
        std::chrono::high_resolution_clock::now();
    if (!loadFromCachedBinary) {
      if (sample_app::StatusCode::SUCCESS != app->createContext()) {
        return app->reportError("Context Creation failure");
      }
      if (sample_app::StatusCode::SUCCESS != app->composeGraphs()) {
        return app->reportError("Graph Prepare failure");
      }
      if (sample_app::StatusCode::SUCCESS != app->finalizeGraphs()) {
        return app->reportError("Graph Finalize failure");
      }
    } else {
      if (sample_app::StatusCode::SUCCESS != app->createFromBinary()) {
        return app->reportError("Create From Binary failure");
      }
    }

    app->m_model->sum_of_all_executions_start_time =
        std::chrono::high_resolution_clock::now();
    if (sample_app::StatusCode::SUCCESS != app->executeGraphs()) {
      return app->reportError("Graph Execution failure");
    }
    app->m_model->sum_of_all_executions_stop_time =
        std::chrono::high_resolution_clock::now();

    if (sample_app::StatusCode::SUCCESS != app->freeContext()) {
      return app->reportError("Context Free failure");
    }

    if (sample_app::StatusCode::SUCCESS != app->destroyPowerConfigId()) {
      return app->reportError("Destroy power failure");
    }

    app->displayKPI();

    if (sample_app::StatusCode::FAILURE != devicePropertySupportStatus) {
      auto freeDeviceStatus = app->freeDevice();
      if (sample_app::StatusCode::SUCCESS != freeDeviceStatus) {
        return app->reportError("Device Free failure");
      }
    }
  }

  if (sg_backendHandle) {
    pal::dynamicloading::dlClose(sg_backendHandle);
  }
  if (sg_modelHandle) {
    pal::dynamicloading::dlClose(sg_modelHandle);
  }

  return EXIT_SUCCESS;
}
