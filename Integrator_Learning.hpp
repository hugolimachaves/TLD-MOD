#ifndef INTEGRATOR_LEARNING_HPP_INCLUDED
#define INTEGRATOR_LEARNING_HPP_INCLUDED

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../common/utils.hpp"
#include "../common/sift_utils.hpp"
#include "Detector.hpp"

using namespace cv;

void initJudge(Mat frame, BoundingBox position, int _valid, int _conf, bool show);
bool IntegratorLearning(Mat frame, BoundingBox t_bb, vector<BoundingBox> detector_positions, vector<double> d_conf, bool tracked, bool detected, BoundingBox &output, Mat &object, bool enable_detect);

#endif // INTEGRATOR_LEARNING_HPP_INCLUDED

