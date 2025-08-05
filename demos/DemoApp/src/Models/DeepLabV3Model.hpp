#pragma once

#include <Models/Model.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

class DeepLabV3Model : public Model {
public:
  void preProcess(PreProcessContext preProcessContext);
  bool postProcess(PostProcessContext postProcessContext);
  static std::string getModelName();

private:
  cv::Mat input_image;
};