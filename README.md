# AI Demo App

## Introduction

Qualcomm AI demos are console applications that provide templates to help implement specific deep learning inference scenarios using QNN for Windows on Snapdragon. These applications show how to preprocess and postprocess data for model inference and organize processing pipelines.

## How to run the Inference

To run the inference, you can use the following command:

```cmd
qnn-demo-app.exe --model_name <Model NAME> --retrieve_context <PATH TO MODEL SERIALIZED BIN FILE> --input <INPUT FILE> [--input <INPUT FILE>...]
```
```
REQUIRED ARGUMENTS:
-------------------
  --model_name        <VAL>       model name related to the model path.
                                  1. DeepLabv3
                                  2. DeepFillV2
                                  3. DexiNed
                                  4. DMSHN
                                  5. Headpose
                                  6. HRNet
                                  7. Midas_v2
                                  8. OpenPose
                                  9. QuickSRNet
  --input             <FILE>      Path to input for the network. --input img.jpg
                                  If the graph takes multiple inputs, then send the inputs
                                  separated by comma.
                                  i.e., --input .\Data\deepfill_v2\1.png,.\Data\deepfill_v2\1_mask.png
                                  This switch can be used multiple times to make more than one
                                  inference.

  --retrieve_context  <VAL>       Path to cached binary from which to load a saved
                                  context from and execute graphs.


OPTIONAL ARGUMENTS:
-------------------

  --output_dir        <DIR>       The directory to save output to. Defaults to ./output.

  --profiling_level   <VAL>       Enable profiling. Valid Values:
                                    1. basic:    captures execution and init time.
                                    2. detailed: in addition to basic, captures
                                                 per Op timing for execution.
  --kpi                           Print the KPI info.

  --log_level                     Specifies max logging level to be set.  Valid settings:
                                 "error", "warn", "info" and "verbose".

  --version                       Print the QNN SDK version.

  --help                          Show this help message.
```

#### Example:

```cmd
.\qnn-demo-app.exe --model_name HRNet --retrieve_context ./Data/hr_net/pose_quantized.serialized.v73.bin --input ./Data/hr_net/img3.jpg --kpi
```
```
model name hrnet
Model Load time (ms): 104
Total number of Executions: 1
Avg Model Preprocessing time (ms): 2
Avg Model Inference time (ms): 3
Avg Model Postprocessing time (ms): 6
Avg Execution time (ms): 12
Sum of all Execution times (ms): 12
Aprox. FPS: 83
```

The output will be stored in the output directory formed in the same directory from where you run the command.

## How to Generate .bin Files

If you want to convert the models to serialized .bin files, you can use the Python notebooks [here](./Notebooks/models) to convert the models to serialized binary files.

### Environment to Run the Notebook

- Python 3.8.10
- Install QNN (Notebooks are tested on QNN 2.18 and QNN 2.21)
- Place the models directory in "C:\Qualcomm\AIStack"
- Run the notebooks

## Model Performance

| Sno | Model      | Model Input Dimensions | Image size  | Preprocessing Time(ms) | Inference Time(ms) | Postprocessing Time(ms) | Total Time(ms) | FPS |
| --- | ---------- | ---------------------- | ----------- | ---------------------- | ------------------ | ----------------------- | -------------- | --- |
| 1   | DeepLabv3  | 1 x 288 x 512 x 3      | 1000 x 667  | 7                      | 12                 | 4                       | 24             | 42  |
| 2   | DeepFillV2 | 1 x 512 x 512 x 3      | 680 x 512   | 12                     | 71                 | 7                       | 91             | 11  |
| 3   | DexiNed    | 1 x 288 x 512 x 3      | 512 x 288   | 1                      | 8                  | 6                       | 16             | 63  |
| 4   | DMSHN      | 1 x 288 x 512 x 3      | 1536 x 1024 | 12                     | 55                 | 6                       | 75             | 14  |
| 5   | HeadPose   | 1 x 224 x 224 x 3      | 281 x 280   | 1                      | 1                  | 0                       | 3              | 333 |
| 6   | HRNet      | 1 x 512 x 512 x 3      | 512 x 512   | 2                      | 4                  | 2                       | 8              | 125 |
| 7.1 | Midas_v2   | 1 x 256 x 256 x 3      | 292 x 263   | 1                      | 4                  | 0                       | 6              | 167 |
| 7.2 | Midas_v2   | 1 x 256 x 256 x 3      | 772 x 570   | 3                      | 4                  | 3                       | 11             | 91  |
| 8   | OpenPose   | 1 x 240 x 320 x 3      | 426 x 640   | 2                      | 13                 | 4                       | 20             | 50  |
| 9   | QuickSRNet | 1 x 720 x 1280 x 3     | 1280 x 720  | 6                      | 142                | 26                      | 176            | 6   |


> **_NOTE:_** The size of input jpg image used for taking KPI numbers was very close to the input dimensions.


## License Information

This project is licensed under the BSD-3-Clause License. For the full license text, please refer to the LICENSE file in this repository.
