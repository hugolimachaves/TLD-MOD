#include "Tracker.hpp"

#define GRID_SIZE 10
#define PYRAMIDAL_LEVEL 2 //No codigo é 5
#define SSD_PATCH_SIZE 5
#define NCC_PATCH_SIZE 10
#define FILTER_THRESHOLD 0.5
#define FAILURE_THRESHOLD 10

vector<Point2f> f_grid[2], //Grid origem (0) e destino (1) (forward)
                b_grid[2], //Grid origem (0) e destino (1) (backward)
                filtered_grid[2]; //Pontos que passaram pelos filtros

void initGrid(BoundingBox bb){
	if(!f_grid[0].empty()) f_grid[0].clear();

	float   step_w = widthBB(bb)/GRID_SIZE,
            step_h = heightBB(bb)/GRID_SIZE,
            x = bb[0] + step_w/2.,
            y = bb[1] + step_h/2.;

	for(int j = 0; j < GRID_SIZE; j++){
		for(int i = 0; i < GRID_SIZE; i++){
			f_grid[0].push_back(Point2f(x, y));
			x+= step_w;
		}
		y+=step_h;
		x = bb[0] + step_w/2;
	}
}

typedef struct _error{
	int index;
	float error;
}Error;

int maxError(const void * a, const void * b){
    Error *a2 = (Error*) a;
    Error *b2 = (Error*) b;

    if (a2->error >  b2->error) return -1; //maior primeiro
    if (a2->error <  b2->error) return 1;
    return 0;
}

void filterFlowSSD(Mat curr_frame, Mat next_frame, vector<uchar> status, bool *pass){
	Mat     subframe_1,
            subframe_2,
            res;
	Error   *ssd_err = (Error*) malloc(sizeof(Error)*f_grid[0].size());
	Size    patch_size(SSD_PATCH_SIZE, SSD_PATCH_SIZE);
    int     ssd_err_size = 0;
    Point2f center_0,
            center_1;

	for (int i = 0; i < f_grid[0].size(); i++) {
        if(status[i] == 1){ //Fluxo válido
            center_0.x = f_grid[0][i].x;
            center_0.y = f_grid[0][i].y;
            center_1.x = f_grid[1][i].x;
            center_1.y = f_grid[1][i].y;
            //Extrai vizinhança de tamanho s e centro center_0 da imagem img para subframe
            getRectSubPix(curr_frame, patch_size, center_0, subframe_1);
            getRectSubPix(next_frame, patch_size, center_1, subframe_2);
            //Sum of squared differences = medida de dissimilaridade
            matchTemplate(subframe_1, subframe_2, res, CV_TM_SQDIFF);
            ssd_err[ssd_err_size].index = i;
            ssd_err[ssd_err_size].error = ((float *)(res.data))[0];
            ssd_err_size++;
        }
        else pass[i] = false;
	}

	qsort(ssd_err, ssd_err_size, sizeof(Error), maxError); //Ordena erros, maiores erros primeiro
	for(int i = 0; i < ssd_err_size*FILTER_THRESHOLD; i++) {
		int index = ssd_err[i].index;
		pass[index] = false;
	}
	free(ssd_err);
}

void filterFlowNCC(Mat curr_frame, Mat next_frame, vector<uchar> status, bool *pass){
	Mat     subframe_1,
            subframe_2,
            res;
	Error   *ncc_err = (Error*) malloc(sizeof(Error)*f_grid[0].size());
	Size    patch_size(NCC_PATCH_SIZE, NCC_PATCH_SIZE);
    int     ncc_err_size = 0;
    Point2f center_0,
            center_1;
	float err;


	for (int i = 0; i < f_grid[0].size(); i++) {
        if(status[i] == 1){ //Fluxo válido
            center_0.x = f_grid[0][i].x;
            center_0.y = f_grid[0][i].y;
            center_1.x = f_grid[1][i].x;
            center_1.y = f_grid[1][i].y;
            //Extrai vizinhança de tamanho s e centro center_0 da imagem img para subframe
            getRectSubPix(curr_frame, patch_size, center_0, subframe_1);
            getRectSubPix(next_frame, patch_size, center_1, subframe_2);
            //Normalized cross correlation = medida de similaridade
            matchTemplate(subframe_1, subframe_2, res, CV_TM_CCOEFF_NORMED);
            //err = ncc(curr_frame, next_frame, center_0, center_1);
            ncc_err[ncc_err_size].index = i;
            ncc_err[ncc_err_size].error = 1. - ((float *)(res.data))[0]; //ncc está em [-1,1]. error em [0,2]
            ncc_err_size++;
        }
        else pass[i] = false;
	}

	qsort(ncc_err, ncc_err_size, sizeof(Error), maxError); //Ordena erros, maiores erros primeiro
	for(int i = 0; i < ncc_err_size*FILTER_THRESHOLD; i++) {
		int index = ncc_err[i].index;
		pass[index] = false;
	}
	free(ncc_err);
}

void filterFlowFB(Mat curr_frame, Mat next_frame, vector<uchar> status, bool *pass, float &median_error) {
	if(b_grid[1].size() != f_grid[0].size()) return;

	int     fb_err_size = 0;
	float 	x, y,	//original
			xB, yB; //Backward
	Error   *fb_err = (Error*) malloc(sizeof(Error)*f_grid[0].size());

	for(int i = 0; i < f_grid[0].size(); i++){
		if(status[i] == 1){ //Fluxo válido
			x = f_grid[0][i].x;
			y = f_grid[0][i].y;
			//grid[1] == nextTask->grid[0] == forward
			xB = b_grid[1][i].x;
			yB = b_grid[1][i].y;
			fb_err[fb_err_size].index = i;
			fb_err[fb_err_size].error = sqrt((x - xB)*(x - xB) + (y - yB)*(y - yB));
			fb_err_size++;
		}
		else pass[i] = false;
	}

	qsort(fb_err, fb_err_size, sizeof(Error), maxError); //Ordena distâncias, maiores erros primeiro
	for(int i = 0; i < fb_err_size*FILTER_THRESHOLD; i++) {
		int index = fb_err[i].index;
		pass[index] = false;
	}
	median_error = fb_err[(int)(fb_err_size*FILTER_THRESHOLD)].error;
	free(fb_err);
}

void removePoints(bool *pass){
    for(int i = f_grid[0].size() - 1; i >= 0; i--){
        if(pass[i]){
			filtered_grid[0].push_back(f_grid[0][i]);
			filtered_grid[1].push_back(f_grid[1][i]);
        }
//        else{
//            f_grid[0].erase(f_grid[0].begin()+i);
//            f_grid[1].erase(f_grid[1].begin()+i);
//        }
    }
}

void drawFlow(Mat imgOutput, DRAW draw_flow, bool *pass){
	Scalar color;

    for(int i = 0; i < f_grid[0].size(); i++){
		if(!pass[i]) color[0] = color[1] = color[2] = 0.;
		else color[0] = color[1] = color[2] = 255.;

		Point2f p1(f_grid[0][i].x, f_grid[0][i].y);
		if(draw_flow != DISABLED) circle(imgOutput, p1, 1, color);
		if(draw_flow == FLOW){
            Point2f p2(f_grid[1][i].x, f_grid[1][i].y);
            line(imgOutput, p1, p2, color);
        }
    }
}

int maxValue(const void *a, const void *b){
    float *a2 = (float*) a;
    float *b2 = (float*) b;

    if ((*a2) >  (*b2)) return -1; //maior primeiro
    if ((*a2) <  (*b2)) return 1;
    return 0;
}

void medianFlow(Mat curr_frame, BoundingBox &bb, bool detect_failure){
	if(filtered_grid[0].empty() || filtered_grid[0].size() != filtered_grid[1].size()) return;

	int height = curr_frame.size().height,
		width = curr_frame.size().width,
		width_bb = widthBB(bb),
		height_bb = heightBB(bb),
		ratio_size = (filtered_grid[0].size()*(filtered_grid[0].size()-1))/2,
		ratio_pointer = 0,
		median_index;
	float 	*flow_x, *flow_y,
			median_trans_x, median_trans_y,
			median_scale,
			dist_1, dist_2,
			*dist_ratio,
			*residual_x, *residual_y;
    BoundingBox new_bb;

    flow_x = (float*) malloc(sizeof(float)*filtered_grid[0].size());
    flow_y = (float*) malloc(sizeof(float)*filtered_grid[0].size());
    dist_ratio = (float*) malloc(sizeof(float)*ratio_size);
    residual_x = (float*) malloc(sizeof(float)*f_grid[0].size());
    residual_y = (float*) malloc(sizeof(float)*f_grid[0].size());

	for(int i = 0; i < filtered_grid[0].size(); i++){
		flow_x[i] = filtered_grid[1][i].x - filtered_grid[0][i].x;
		flow_y[i] = filtered_grid[1][i].y - filtered_grid[0][i].y;
		for(int j = i+1; j < filtered_grid[0].size(); j++){
			dist_1 = sqrt(SQR(filtered_grid[0][i].x - filtered_grid[0][j].x) + SQR(filtered_grid[0][i].y - filtered_grid[0][j].y));
			dist_2 = sqrt(SQR(filtered_grid[1][i].x - filtered_grid[1][j].x) + SQR(filtered_grid[1][i].y - filtered_grid[1][j].y));
			if(dist_1 > EPSILON) dist_ratio[ratio_pointer++] = dist_2/dist_1;
			else if(dist_2 <= EPSILON) dist_ratio[ratio_pointer++] = 1.;
			//else dist_ratio[ratio_pointer++] = INFINITY;
		}
	}

	//Mediana dos deslocamentos
    qsort(flow_x, filtered_grid[0].size(), sizeof(float), maxValue);
    qsort(flow_y, filtered_grid[0].size(), sizeof(float), maxValue);
    median_index = floor(filtered_grid[0].size()/2.);
    if(median_index >= filtered_grid[0].size()) {
		median_trans_x = 0.;
		median_trans_y = 0.;
	}
    else {
		median_trans_x = flow_x[median_index];
		median_trans_y = flow_y[median_index];
	}

	//Detecção de falhas
	if(detect_failure){
		median_index = floor(f_grid[0].size()/2.);
        for(int i = 0; i < f_grid[0].size(); i++){
            residual_x[i] = f_grid[1][i].x - f_grid[0][i].x - median_trans_x;
            residual_x[i] = fabs(residual_x[i]);

            residual_y[i] = f_grid[1][i].y - f_grid[0][i].y - median_trans_y;
            residual_y[i] = fabs(residual_y[i]);
        }
        qsort(residual_x, f_grid[0].size(), sizeof(float), maxValue);
        qsort(residual_y, f_grid[0].size(), sizeof(float), maxValue);
    }

    if(!detect_failure || (residual_x[median_index] + residual_y[median_index]) < FAILURE_THRESHOLD) {
		qsort(dist_ratio, ratio_pointer, sizeof(float), maxValue);
		median_index = floor(ratio_pointer/2.);
		if(median_index >= ratio_pointer) median_scale = 1.;
		else median_scale = dist_ratio[median_index];

        new_bb[0] = bb[0] + median_trans_x + ((width_bb*(1. - median_scale))/2.);
        new_bb[1] = bb[1] + median_trans_y + ((height_bb*(1. - median_scale))/2.);
        new_bb[2] = bb[2] + median_trans_x - ((width_bb*(1. - median_scale))/2.);
        new_bb[3] = bb[3] + median_trans_y - ((height_bb*(1. - median_scale))/2.);

		//Tamanho máximo
		if(widthBB(new_bb) < GRID_SIZE || widthBB(new_bb) >= width || heightBB(new_bb) < GRID_SIZE || heightBB(new_bb) >= height)
			new_bb[0] = new_bb[1] = new_bb[2] = new_bb[3] = NAN;
		//Fora do frame
		if((new_bb[2] < 0) || (new_bb[0] >= width) || (new_bb[3] < 0) || (new_bb[1] >= height))
			new_bb[0] = new_bb[1] = new_bb[2] = new_bb[3] = NAN;
	}
	else new_bb[0] = new_bb[1] = new_bb[2] = new_bb[3] = NAN;

	bb[0] = new_bb[0];
	bb[1] = new_bb[1];
	bb[2] = new_bb[2];
	bb[3] = new_bb[3];

	free(dist_ratio);
	free(flow_x);
	free(flow_y);
	free(residual_x);
	free(residual_y);
}

bool Track(Mat curr_frame, Mat next_frame, BoundingBox &bb, DRAW draw_flow, int filter, bool detect_failure, int window_size){
    //variáveis do lucas-kanade opencv
    static TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
    static Size windowSize(window_size,window_size);
    vector<uchar> status, status_b;
    vector<float> err, err_b;

	initGrid(bb);
	//Forward
	if(!f_grid[1].empty()) f_grid[1].clear();
	f_grid[1] = f_grid[0]; //cópia independente
	calcOpticalFlowPyrLK(curr_frame, next_frame, f_grid[0], f_grid[1], status, err, windowSize, PYRAMIDAL_LEVEL, termcrit, OPTFLOW_USE_INITIAL_FLOW);
	//Backward
	if(filter & FB_FILTER){
		if(!b_grid[0].empty()) b_grid[0].clear();
		b_grid[0] = f_grid[1]; //cópia independente
		if(!b_grid[1].empty()) b_grid[1].clear();
		b_grid[1] = f_grid[0]; //cópia independente
		calcOpticalFlowPyrLK(next_frame, curr_frame, b_grid[0], b_grid[1], status_b, err_b, windowSize, PYRAMIDAL_LEVEL, termcrit, OPTFLOW_USE_INITIAL_FLOW);
	}

	bool *pass = (bool*) malloc(sizeof(bool)*f_grid[0].size());
	float median_error = 0.;
	for(int i = 0; i < f_grid[0].size(); i++) pass[i] = true;

	if(filter & FB_FILTER) filterFlowFB(curr_frame, next_frame, status, pass, median_error);
	if(filter & SSD_FILTER) filterFlowSSD(curr_frame, next_frame, status, pass);
	if(filter & NCC_FILTER) filterFlowNCC(curr_frame, next_frame, status, pass);

	removePoints(pass);

	if(draw_flow != DISABLED) drawFlow(curr_frame, draw_flow, pass);

	medianFlow(curr_frame, bb, detect_failure);

	free(pass);

	if(!status.empty()) status.clear();
	if(!status_b.empty()) status_b.clear();
	if(!err.empty()) err.clear();
	if(!err_b.empty()) err_b.clear();
	if(!f_grid[0].empty()) f_grid[0].clear();
	if(!f_grid[1].empty()) f_grid[1].clear();
	if(!b_grid[0].empty()) b_grid[0].clear();
	if(!b_grid[1].empty()) b_grid[1].clear();
	if(!filtered_grid[0].empty()) filtered_grid[0].clear();
	if(!filtered_grid[1].empty()) filtered_grid[1].clear();

	if(std::isnan(bb[0]))
	{
		return false;
	}

	return true;
}
