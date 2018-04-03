#ifndef SIFT_UTILS_HPP_INCLUDED
#define SIFT_UTILS_HPP_INCLUDED

#include <opencv2/highgui/highgui.hpp>
extern "C"
{
	#include "vl/generic.h"
	#include "vl/sift.h"
	#include "vl/stringop.h"
	#include "vl/pgm.h"
	#include "vl/getopt_long.h"
}
#include "utils.hpp"

#define DESCRIPTOR_SIFT_SIZE 	128		//Tamanho do descritor do sift
#define LAMBDA 0.7		//Parâmetro do classificador NN para o sift

using namespace cv;

class SiftFeature{
	public:
		Point2f point;
		vl_sift_pix descriptor[DESCRIPTOR_SIFT_SIZE];
		float radius;
		int flag;
		SiftFeature *next;
};

vl_sift_pix *Mat2vl(Mat imgCV);
void desenha_key(Mat frame, SiftFeature *keypoints);
double euclideanDistance(vl_sift_pix d1[DESCRIPTOR_SIFT_SIZE], vl_sift_pix d2[DESCRIPTOR_SIFT_SIZE]);
SiftFeature *sift(Mat gray_img);
SiftFeature* setClear(SiftFeature *root);
int setSize(SiftFeature *root);
SiftFeature* randomForgetting(SiftFeature *sift_set, int curr_size, int new_size);
int occlusionDetection(SiftFeature *s_t, SiftFeature **o_t, SiftFeature **a_t, BoundingBox corrected_bb, SiftFeature *c_t, SiftFeature *d_t, int t);

#endif // SIFT_UTILS_HPP_INCLUDED
