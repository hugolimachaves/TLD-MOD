#ifndef TLD_HPP_INCLUDED
#define TLD_HPP_INCLUDED

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../common/utils.hpp"
#include "Tracker.hpp"
#include "Detector.hpp"
#include "Integrator_Learning.hpp"

using namespace std;
using namespace cv;


void TLD(char *parameters_path);

#endif // TLD_HPP_INCLUDED
