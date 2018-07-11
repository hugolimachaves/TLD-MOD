#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/legacy/legacy.hpp"
#include <iostream>

#define MAX_DIST_CLUSTER	0.5		//Distância máxima da amostra até o centro de seu cluster

#ifndef MIN
	#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
	#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef SQR
	#define SQR(a) ((a)*(a))
#endif

#ifndef EPSILON
    #define EPSILON 0.0000077
#endif

#define _DEBUG_IL 0
#define _DEBUG_TRACKER 0
#define _DEBUG_DETECTOR 1

#define OVERLAP_THRESHOLD 0.5
#define MIN_BB 20      //Tamanho mínimo de bb

#define SMOOTH(image, blur) GaussianBlur(image, blur, Size(-1, -1), 3, 3);

#define UCHAR 0
#define SIGNED8 1
#define UNSIGNED16 2
#define SIGNED16 3
#define SIGNED32 4
#define FLOAT32 5
#define DOUBLE64 6
#define ONEBIT 7

#ifndef RAD2DEG
#define RAD2DEG(a)  (((a) * 180) / M_PI)
#endif

#ifndef LESSER
#define LESSER(a, b) (a < b ? a : b)
#endif


using namespace cv;

enum VIDEO_TYPE{
    WEBCAM,
    VIDEO,
    IMAGE_LIST
};

typedef std::array<float,4> BoundingBox; //xMin, yMin, xMax, yMax. &BoundingBox no cabeçalho para alterar conteúdo dentro da função. A chamada é feita com BoundingBox.
typedef double Vector2D[2];

typedef struct _sortElement{	//Elemento para ordenação
	int index;
	double val;
}SortElement;

double L2norm(double hist[], int hist_size);
int maxVal(const void * a, const void * b);
float widthBB(BoundingBox bb);
float heightBB(BoundingBox bb);
float areaBB(BoundingBox bb);
float intersectionArea(BoundingBox bb1, BoundingBox bb2);
float overlap(BoundingBox bb1, BoundingBox bb2); // interseção / união \in [0,1]
void writeBBInfos(string filename, BoundingBox bb, int frame, bool type);

Mat integralImage(Mat image, int p);
float* getVariance(Mat image, vector<BoundingBox> bb_list);
int matType(Mat frame, int &channels);
int clusterConf(Mat frame, vector<BoundingBox> d_answers, vector<double> d_conf, vector<BoundingBox> &c_answers, vector<double> &c_conf, bool show);

#endif // UTILS_HPP_INCLUDED
