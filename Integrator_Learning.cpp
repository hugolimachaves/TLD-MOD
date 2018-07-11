#include "Integrator_Learning.hpp"

#define TRACKER_WINDOW "Tracker answer"
#define DETECTOR_WINDOW "Detector answer"

#define NN_VALID			0.55	//Margem do core

//Descritores concatenados
//Histograma de cores
#define HC_BEGIN 0
#define HC_SIZE 256

//Histograma de textura - lbp
#define LBP_BEGIN HC_SIZE
#define DEFAULT_PATCH 32
#define CELL_SIZE 16
#define NUM_CELL SQR(DEFAULT_PATCH/CELL_SIZE)
#define EACH_HIST 256 //2^8
#define LBP_SIZE NUM_CELL*EACH_HIST

//Histograma de gradientes - hog
#define HOG_BEGIN HC_SIZE+LBP_SIZE
#define HOG_SIZE 16 //Numero de bins no histograma

#define DESCRIPTOR_SIZE (HC_SIZE + LBP_SIZE + HOG_SIZE)

//Aspectos ativos
#define HC 1
#define LBP 1
#define HOG 1

#define MARGIN 20 		//Margem em torno do objeto que define o contexto
#define N_OCCL 2 		//Número máximo de features cobrindo o objeto para que o rastreamento seja validado
#define N_D 1500 		//Número máximo de features de oclusão
#define WIND_LENGHT 10 	//Quantidade de frames que as features de contexto são mantidas

static 	int valid = 0, 	//0 = NCC, 1 = media, 2 = alien
			conf = 0; 	//0 = NCC, 1 = media
static bool init_fail = false,
			show;

typedef std::array<double,DESCRIPTOR_SIZE> Descriptor;

static vector<Descriptor> model; 	//Descritores de aparência do objeto
static SiftFeature *c_t, *d_t; 		//Conjunto de features de contexto e de oclusão
static int t, d_size;  				// t = contador de frame, d_size = tamanho de d_t

//Computa histograma de cores - Descritor de cor. Recebe descritor zerado nas posições do HC.
void colorHistogram(Mat patch, Descriptor &descr){
	uchar *row;
	double *color_hist = &descr[HC_BEGIN], norm;

	for(int y = 0; y < patch.rows; y++){
		row = patch.ptr<uchar>(y);
		for(int x = 0; x < patch.cols; x++, row++){
			if(*row < HC_SIZE) color_hist[(int)(*row)]++;
		}
	}

	norm = L2norm(color_hist, HC_SIZE);
	if(norm > EPSILON) for(int i = 0; i < HC_SIZE; i++) {
		color_hist[i]/=norm;
	}
}

//Computa histograma de local binary pattern - Descritor de textura. Recebe descritor zerado nas posições do LBP.
// 														|4|3|2|
// Sentido anti-horário começando pelo ponto(x+1,y) 	|5|*|1|
// 														|6|7|8|
void lbpHistogram(Mat patch, Descriptor &descr){
	int x_shift[8] = {1, 1, 0,-1,-1,-1, 0, 1}, //Sentido anti-horário começando pelo ponto (x+1,y)
		y_shift[8] = {0,-1,-1,-1, 0, 1, 1, 1};
	uchar threshold, color;
	Point2f point, corner(0,0);
	int code, hist_begin = LBP_BEGIN;
	double *hist, norm;
	Mat local_patch;

	resize(patch, local_patch, Size(DEFAULT_PATCH, DEFAULT_PATCH));
	SMOOTH(local_patch, local_patch);

	for(int cell = 0; cell < NUM_CELL; cell++){
		hist = &descr[hist_begin];
		point.x = corner.x + 1;
		point.y = corner.y + 1;

		for(int y = 1; y < CELL_SIZE-1; y++){ //Sem borda
			for(int x = 1; x < CELL_SIZE-1; x++){
				code = 0;
				threshold = local_patch.at<uchar>(point.y, point.x);

				for(int k = 0; k < 8; k++){
					code<<=1;
					color = local_patch.at<uchar>(point.y + y_shift[k], point.x + x_shift[k]);
					if(color > threshold) code|=1;
				}
				hist[code]++;
				point.x++;
			}
			point.x = corner.x + 1;
			point.y++;
		}

		norm = L2norm(hist, EACH_HIST);
		if(norm > EPSILON) for(int i = 0; i < EACH_HIST; i++) hist[i]/=norm;

		hist_begin+= EACH_HIST;
		corner.x+= CELL_SIZE;
		if(corner.x >= DEFAULT_PATCH){
			corner.x = 0;
			corner.y+= CELL_SIZE;
		}
	}
    local_patch.release();
}

//Converte coordenadas cartesianas em polares
void polarCoordinates(Mat grad_x, Mat grad_y, Vector2D *polar_coordinates, int vector_count){
	double 	ang,
			*x_row, *y_row,
			*x_cell, *y_cell;
	int index = 0;

	if(!polar_coordinates || grad_x.empty() || grad_x.cols != grad_y.cols || grad_x.rows != grad_y.rows || grad_x.rows*grad_x.cols != vector_count){
		printf("Invalid vectors.\n");
		return;
	}

	for(int row = 0; row < grad_x.rows; row++){
		x_row = grad_x.ptr<double>(row);
		y_row = grad_y.ptr<double>(row);
		x_cell = &x_row[0];
		y_cell = &y_row[0];

		for(int col = 0; col < grad_x.cols; col++, x_cell++, y_cell++){
			polar_coordinates[index][0] = sqrt(SQR(*x_cell) + SQR(*y_cell)); //magnitude
			ang = atan2f(*y_cell, *x_cell);
            ang = (ang >= 0.) ? RAD2DEG(ang): 360. + RAD2DEG(ang);
            polar_coordinates[index++][1] = ang;
		}
	}
}

//Computa histogram of oriented gradient - Descritor de forma
void hog(Mat patch, Descriptor &descriptor){
	Mat grad_x, grad_y;
	Vector2D *polar_coordinates;
	int vector_count;
	double  magn, ang, bin_midpoint, diff,
            stddev = 0.01, power, step = 1.0/HOG_SIZE,
            *hist = &descriptor[HOG_BEGIN];
    Mat local_patch;

	SMOOTH(patch, local_patch);
	Sobel(local_patch, grad_x, CV_64F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(local_patch, grad_y, CV_64F, 0, 1, 3, 1, 0, BORDER_DEFAULT);

	vector_count = grad_x.rows*grad_x.cols;
	polar_coordinates = (Vector2D*) malloc(sizeof(Vector2D)*vector_count);
	polarCoordinates(grad_x, grad_y, polar_coordinates, vector_count);

	for(int i = 0; i < vector_count; i++){
		magn = polar_coordinates[i][0];
		ang = polar_coordinates[i][1]/360.0; //Angle normalization
		for(int j = 0; j < HOG_SIZE; j++){
			bin_midpoint = ((double)(j*step + (j+1)*step))/2.0;	//Compute the bin midpoint
			diff = fabs(bin_midpoint - ang);					//Difference between the angle and the bin_midpoint...
			diff = LESSER(1-diff,diff); 						//... adjusted if closer to 0 or 2*Pi, in order to create a circular interval.
			power = -SQR(diff);
			power/= 2*SQR(stddev);
			hist[j] += exp(power)*magn;			//Histogram bin filling based on gaussian distribution around ang
		}
	}

	magn = L2norm(hist, HOG_SIZE);
	if(magn > EPSILON) for(int i = 0; i < HOG_SIZE; i++) hist[i]/= magn;

	free(polar_coordinates);
	local_patch.release();
	grad_x.release();
	grad_y.release();
}

//Computa features do candidato a objeto ou do objeto inicial na posição position do frame
void computeDescriptor(Mat frame, BoundingBox position, Descriptor &descriptor){
	Mat patch;
	Size _size(widthBB(position), heightBB(position));
	Point2f center(position[0] + _size.width/2., position[1] + _size.height/2.);

	getRectSubPix(frame, _size, center, patch);

    for(int i = 0; i < DESCRIPTOR_SIZE; i++) descriptor[i] = 0.;

    if(HC) colorHistogram(patch, descriptor);
    if(LBP) lbpHistogram(patch, descriptor);
    if(HOG) hog(patch, descriptor);

	patch.release();
}

double similarity(Descriptor d1, Descriptor d2){
    if(HC + LBP + HOG == 0) return 0.;

	int index_match;
	double hc_vote = 0., lbp_vote = 0., hog_vote = 0., max_vote = 0.;

	if(HC){
		for(int i = HC_BEGIN; i < HC_BEGIN+HC_SIZE; i++) hc_vote+= d1[i]*d2[i]; //NCC. Descritores já foram normalizados
		if(hc_vote > max_vote) max_vote = hc_vote;
	}
    if(LBP){
        for(int i = LBP_BEGIN; i < LBP_BEGIN+LBP_SIZE; i++) lbp_vote+= d1[i]*d2[i];
        lbp_vote/=NUM_CELL;
        if(lbp_vote > max_vote) max_vote = lbp_vote;
    }
	if(HOG){
		for(int i = HOG_BEGIN; i < HOG_BEGIN+HOG_SIZE; i++) hog_vote+= d1[i]*d2[i];
		if(hog_vote > max_vote) max_vote = hog_vote;
	}

	return (hc_vote + lbp_vote + hog_vote)/(HC + LBP + HOG);
	//return max_vote;
}

double conservativeSimilarity(Descriptor descr){
    int mid = (int)ceil(model.size()/2.);
    vector<Descriptor>::iterator    sample,
                                    middle = model.begin() + mid;
    double sim, max_sim = -1.;

    for(sample = model.begin(); sample != middle; sample++){
        sim = similarity(*sample, descr);
        if(sim > max_sim) max_sim = sim;
    }

    return max_sim;
}

double modelSimilarity(Descriptor descr){
    vector<Descriptor>::iterator    sample;
    double sim, max_sim = -1.;

    for(sample = model.begin(); sample != model.end(); sample++){
        sim = similarity(*sample, descr);
        if(sim > max_sim) max_sim = sim;
    }

    return max_sim;
}

//Validação da resposta do tracker utilizando sift
bool trackerValid(Mat frame, BoundingBox position, int &o_size){
	BoundingBox new_position, margin;

	///NOTE: Idêntico ao initJudge.
	//Correção das margens quando a bb está próxima das bordas do frame
	margin[0] = MIN(position[0], MARGIN);
	margin[1] = MIN(position[1], MARGIN);
	margin[2] = MIN(frame.size().width - 1 - position[2], MARGIN);
	margin[3] = MIN(frame.size().height - 1 - position[3], MARGIN);

	//Janela que inclui bb e contexto
	new_position[0] = position[0] - margin[0];
	new_position[1] = position[1] - margin[1];
	new_position[2] = position[2] + margin[2];
	new_position[3] = position[3] + margin[3];

	//Reaproveitando margin: Posição correta de bb dentro de window
	margin[2] = widthBB(new_position) - margin[2];
	margin[3] = heightBB(new_position) - margin[3];

	Point2f window_center((new_position[0] + new_position[2])/2., (new_position[1] + new_position[3])/2.);
	Size window_size(widthBB(new_position), heightBB(new_position));
	Mat window;
	getRectSubPix(frame, window_size, window_center, window);
    SiftFeature *s_t = sift(window), //Todos os pontos na janela
				*o_t = NULL, //Pontos dentro da bb que casaram com o contexto c_t (contexto acumulado) ou com d_t (oclusões acumuladas)
				*a_t = NULL, //Pontos de contexto atual
				*aux, *aux2;

	o_size = occlusionDetection(s_t, &o_t, &a_t, margin, c_t, d_t, t);
	s_t = NULL;

	if(o_size < N_OCCL){
		if(d_t != NULL){
			for(aux = d_t; aux->next != NULL; aux = aux->next);
			aux->next = o_t;
		}
		else d_t = o_t;
		d_size+= o_size;
		if(d_size > N_D){
			d_t = randomForgetting(d_t, d_size, N_D);
			d_size = N_D;
		}

		if(c_t != NULL){
			aux = c_t;
			//Features estão ordenadas pelo tempo de inserção. Features antigas são eliminadas de c_t
			while(aux->next != NULL && aux->flag <= (t - WIND_LENGHT)){
				aux2 = aux;
				aux = aux->next;
				delete aux2;
			}
			c_t = aux;
			//Adiciona features novas (a_t)
			for(; aux->next != NULL; aux = aux->next);
			aux->next = a_t;
		}
		else c_t = a_t;

		return true;
	}

	o_t = setClear(o_t);
	a_t = setClear(a_t);
    return false;
}

//Seleciona features de contexto iniciais.
SiftFeature* cInit(SiftFeature *s_0, BoundingBox corrected_bb){
	SiftFeature *s_pointer, 	//Percorre s_0
				*c_pointer = NULL, //Percorre c_t
				*aux_pointer; //Marca posições que devem ser apagadas
	int c_count = 0;

	c_t = NULL;

	for(s_pointer = s_0; s_pointer != NULL;){
		//Fora da bounding box = contexto
		if(s_pointer->point.x < corrected_bb[0] || s_pointer->point.x > corrected_bb[2] || s_pointer->point.y < corrected_bb[1] || s_pointer->point.y > corrected_bb[3]){
			if(c_t == NULL){
				s_pointer->flag = 0;
				c_t = c_pointer = s_pointer;
			}
			else {
				s_pointer->flag = 0;
				c_pointer->next = s_pointer;
				c_pointer = s_pointer;
			}
			s_pointer = s_pointer->next;
			c_count++;
		}
		else{
			aux_pointer = s_pointer;
			s_pointer = s_pointer->next;
			delete aux_pointer;
		}
	}

	if(c_pointer != NULL) c_pointer->next = NULL;

	return NULL;
}

//Computa features do objeto original
void initJudge(Mat frame, BoundingBox position, int _valid, int _conf, bool _show){
	if(0 <= _valid && _valid <= 2 && 0 <= _conf && _conf <= 1){
		valid = _valid;
		conf = _conf;
	}
	else {
		init_fail = true;
		return;
	}

	if(widthBB(position) <= 0 || heightBB(position) <= 0){
		init_fail = true;
		return;
	}

	show = _show;

    model.resize(1);
	computeDescriptor(frame, position, model[0]);

	BoundingBox new_position, margin;

	//Correção das margens quando a bb está próxima das bordas do frame
	margin[0] = MIN(position[0], MARGIN);
	margin[1] = MIN(position[1], MARGIN);
	margin[2] = MIN(frame.size().width - 1 - position[2], MARGIN);
	margin[3] = MIN(frame.size().height - 1 - position[3], MARGIN);

	//Janela que inclui bb e contexto
	new_position[0] = position[0] - margin[0];
	new_position[1] = position[1] - margin[1];
	new_position[2] = position[2] + margin[2];
	new_position[3] = position[3] + margin[3];

	//Reaproveitando margin: Posição correta de bb dentro de window
	margin[2] = widthBB(new_position) - margin[2];
	margin[3] = heightBB(new_position) - margin[3];

	Point2f window_center((new_position[0] + new_position[2])/2., (new_position[1] + new_position[3])/2.);
	Size window_size(widthBB(new_position), heightBB(new_position));
	Mat window;
	getRectSubPix(frame, window_size, window_center, window);
    SiftFeature *s_0 = sift(window);

	d_size = t = 0;
    d_t = NULL;

    s_0 = cInit(s_0, margin);

    window.release();
}

int mostConfident(vector<BoundingBox> c_answers, vector<double> c_conf, double &max_d_conf, int &max_d_index, BoundingBox t_bb, double t_conf = -1.){
	max_d_conf = -1.;
	max_d_index = -1;
	if(c_conf.empty()) return -1;

	int index_return = 0,
		index = 1;
	float dist;
	vector<double>::iterator conf;

	max_d_conf = c_conf[0];
	max_d_index = 0;
	for(conf = c_conf.begin()+1; conf != c_conf.end(); conf++, index++){
		if((*conf) > max_d_conf){
			max_d_conf = (*conf);
			index_return = max_d_index = index;
		}
	}

	if(t_conf == -1.) return index_return; //Sem resposta no tracker

	if(max_d_conf > t_conf && overlap(t_bb, c_answers[max_d_index]) < 0.5) return index_return;

	return -1;
}

int mostConfident2(Mat frame, vector<BoundingBox> c_answers, double &max_d_conf, int &max_d_index, BoundingBox t_bb, Descriptor t_descr, double &t_sim){
	max_d_conf = t_sim = -1.;
	max_d_index = -1;
	if(c_answers.empty()) return -1;

	int index_return = 0,
		index = 0;
    vector<Descriptor> c_descr(c_answers.size());
    vector<double> c_sim(c_answers.size());

    vector<Descriptor>::iterator descr;
    vector<BoundingBox>::iterator bb = c_answers.begin();
    vector<double>::iterator sim = c_sim.begin();

	for(descr = c_descr.begin(); descr != c_descr.end(); descr++, bb++, sim++, index++){
            computeDescriptor(frame, *bb, *descr);
            *sim = modelSimilarity(*descr);
            if((*sim) > max_d_conf){
                max_d_conf = (*sim);
                index_return = max_d_index = index;
            }
	}
	if(!std::isnan(t_bb[0])) t_sim = modelSimilarity(t_descr);
	else return index_return; //Sem resposta no tracker

	if(max_d_conf > t_sim && overlap(t_bb, c_answers[max_d_index]) < 0.5) return index_return;

	return -1;
}

bool IntegratorLearning(Mat frame, BoundingBox t_bb, vector<BoundingBox> detector_positions, vector<double> d_conf, bool tracked, bool detected, BoundingBox &output, Mat &object, bool enable_detect){
	static bool tracker_valid = false;
	t++;
	output[0] = output[1] = output[2] = output[3] = NAN;

	if(init_fail) return false;

	Size t_size(widthBB(t_bb), heightBB(t_bb));
	bool result = false;
	int most_confident = -1, max_d_index = -1, num_occl;
	Mat blur_img;
	Candidate t_candidate;
	double t_conf, max_d_conf;
	Descriptor t_descr, d_descr;

	if(tracked && t_size.width > 0 && t_size.height > 0){
		output[0] = t_bb[0];
		output[1] = t_bb[1];
		output[2] = t_bb[2];
		output[3] = t_bb[3];
		result = true;

		switch(valid){
			case 0: //NCC
				SMOOTH(frame, blur_img);
				normalize(frame, blur_img, t_bb, 0, 0, t_candidate.image, t_candidate.ens_img, t_candidate.nn_img);
				t_conf = conservativeSimilarity(t_candidate.nn_img);
				if(t_conf > NN_VALID) tracker_valid = true;
			break;
			case 1: //Media
				computeDescriptor(frame, t_bb, t_descr);
				t_conf = conservativeSimilarity(t_descr);
				if(t_conf > NN_VALID) tracker_valid = true;
			break;
			case 2: //Alien
				tracker_valid = trackerValid(frame, t_bb, num_occl);
			break;
		}

		if(detected){
			switch(conf){
				case 0: //NCC
					if(valid != 0){
						SMOOTH(frame, blur_img);
						normalize(frame, blur_img, t_bb, 0, 0, t_candidate.image, t_candidate.ens_img, t_candidate.nn_img);
						t_conf = conservativeSimilarity(t_candidate.nn_img);
					}
					most_confident = mostConfident(detector_positions, d_conf, max_d_conf, max_d_index, t_bb, t_conf);
				break;
				case 1:	//Media
					if(valid != 1) computeDescriptor(frame, t_bb, t_descr);
					most_confident = mostConfident2(frame, detector_positions, max_d_conf, max_d_index, t_bb, t_descr, t_conf);
				break;
			}

			if(most_confident != -1){ //Reinicializa o tracker
				output[0] = detector_positions[most_confident][0];
				output[1] = detector_positions[most_confident][1];
				output[2] = detector_positions[most_confident][2];
				output[3] = detector_positions[most_confident][3];
				tracker_valid = false;
			}
		}
	}
	else{
		tracker_valid = false;
		if(detected){
			switch(conf){
				case 0: //NCC
					most_confident = mostConfident(detector_positions, d_conf, max_d_conf, max_d_index, t_bb);
				break;
				case 1:	//Media
					most_confident = mostConfident2(frame, detector_positions, max_d_conf, max_d_index, t_bb, t_descr, t_conf);
				break;
			}

			if(most_confident != -1){
				output[0] = detector_positions[most_confident][0];
				output[1] = detector_positions[most_confident][1];
				output[2] = detector_positions[most_confident][2];
				output[3] = detector_positions[most_confident][3];
				result = true;
			}
		}
	}

	if(tracker_valid && enable_detect){
		tracker_valid = Retrain(frame, t_bb, show);
	}

	if(_DEBUG_IL){
		char conf[30];
		if(tracked){
			Mat t_object;
			Point2f t_center(t_bb[0]+t_size.width/2., t_bb[1]+t_size.height/2.);
			getRectSubPix(frame, t_size, t_center, t_object);
			resize(t_object, t_object, Size(200,200));
			sprintf(conf, "%.2f", t_conf);
			putText(t_object, conf, Point2f(15,15), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(255.));
			imshow(TRACKER_WINDOW, t_object);
			t_object.release();
		}
		if(detected){
			Mat d_object;
			Size d_size(widthBB(detector_positions[max_d_index]), heightBB(detector_positions[max_d_index]));
			Point2f d_center(detector_positions[max_d_index][0]+d_size.width/2., detector_positions[max_d_index][1]+d_size.height/2.);
			getRectSubPix(frame, d_size, d_center, d_object);
			resize(d_object, d_object, Size(200,200));
			sprintf(conf, "%.2f", max_d_conf);
			putText(d_object, conf, Point2f(15,15), FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(255.));
			imshow(DETECTOR_WINDOW, d_object);
		}
	}

	Size obj_size(widthBB(output), heightBB(output));
	Point2f obj_center(output[0]+obj_size.width/2., output[1]+obj_size.height/2.);
	if(obj_size.width != 0 || obj_size.height != 0) getRectSubPix(frame, obj_size, obj_center, object);

	t_candidate.image.release();
	t_candidate.ens_img.release();
	t_candidate.nn_img.release();

	return result;
}

