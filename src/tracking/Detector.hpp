#ifndef DETECTOR_HPP_INCLUDED
#define DETECTOR_HPP_INCLUDED

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "../common/utils.hpp"

#define NUM_FERNS           10      //Numero de ferns. NUM_FERNS*NUM_FEAT_FERN < DEFAULT_PATCH_SIZE*DEFAULT_PATCH_SIZE*(DEFAULT_PATCH_SIZE-1)
#define _DEBUG_PERF 		0		//Imprimir tempo de cada processo da detecção
#define _DEBUG_WORKSPACE 	0		//Mostrar features, windows, respostas...

using namespace cv;

class ModelSample{
	public:
		Mat image;
		Mat ens_img;
		Mat nn_img;

		int code[NUM_FERNS];

		ModelSample();
		~ModelSample();
		double similarity(Mat pattern2);
};

class Candidate{
	public:
		Mat image;
		Mat ens_img;
		Mat nn_img;

		int code[NUM_FERNS];

		float variance;
		float average_vote;
		double r_sim;
		double c_sim;
		int scanning_windows_index;

		Candidate();
		~Candidate();
};

void Train(Mat frame, BoundingBox &position, bool show);
bool Retrain(Mat frame, BoundingBox &position, bool show);
bool Detect(Mat frame, vector<BoundingBox> &detector_position, vector<double> &d_conf, int frame_number);
void DetClear();

void normalize(Mat img, Mat blur_img, BoundingBox bb, float shift_x, float shift_y, Mat &sample, Mat &ens_img, Mat &nn_img);
double conservativeSimilarity(Mat pattern);

#endif // DETECTOR_HPP_INCLUDED
