#pragma once

#include <Models/Model.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

class OpenPoseModel : public Model {
public:
	void preProcess(PreProcessContext preContext);
	bool postProcess(PostProcessContext postContext);
	static std::string getModelName();
	cv::Mat image;
};