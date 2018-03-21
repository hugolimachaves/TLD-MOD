#ifndef TRACKER_HPP_INCLUDED
#define TRACKER_HPP_INCLUDED

#include <opencv2/video/tracking.hpp> //Implementação do LK
#include "../common/utils.hpp"

#define SSD_FILTER 1
#define NCC_FILTER 2
#define FB_FILTER 4

enum DRAW{
    DISABLED,
    CIRCLE,
    FLOW
};

using namespace cv;

bool Track(Mat curr_frame, Mat next_frame, BoundingBox &bb, DRAW draw_flow, int filter, bool detect_failure, int window_size);

#endif // TRACKER_HPP_INCLUDED

