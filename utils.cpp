#include "utils.hpp"
#include <math.h>
#include <fstream>
///NOTE: minhas alterações
/// Função para salvar x, y, w, h, frame, 0/1 (positivo ou negativo)
void writeBBInfos(string filename, BoundingBox bb, int frame, bool type){

	float x,y;
    std::ofstream ofs;
    float width = widthBB(bb), height = heightBB(bb);
	x = bb[0]+width/2;
	y =bb[1]+ height/2;
    ofs.open (filename, std::ofstream::out | std::ofstream::app);
    ofs << x << " " << y << " " << width << " " << height << " " << frame <<" "<< int(type) << "\n";
    ofs.close();

}
/// end

double L2norm(double hist[], int hist_size){
    double norm = 0.0;

    for(int i = 0; i < hist_size; i++) norm += SQR(hist[i]);

    norm = sqrt(norm);

	return norm;
}

//Ordenação do sortElement
int maxVal(const void * a, const void * b)
{
    SortElement *a2 = (SortElement*) a;
    SortElement *b2 = (SortElement*) b;

    if (a2->val >  b2->val) return -1; //maior primeiro
    if (a2->val <  b2->val) return 1;
    return 0;
}

float widthBB(BoundingBox bb)
{
	if(std::isnan(bb[2])||std::isnan(bb[0])) return 0.;
	return (bb[2] - bb[0] + 1);
}

float heightBB(BoundingBox bb)
{
	if(std::isnan(bb[3])||std::isnan(bb[1])) return 0.;
	return (bb[3] - bb[1] + 1);
}

float areaBB(BoundingBox bb)
{
	float w, h, area;
	w = widthBB(bb);
    h = heightBB(bb);
    area = w * h;
    return area;
}

float intersectionArea(BoundingBox bb1, BoundingBox bb2)
{
    BoundingBox intersection_bb;
	float 	intersection_area;

    //intersection
    intersection_bb[0] = MAX(bb1[0], bb2[0]);
    intersection_bb[1] = MAX(bb1[1], bb2[1]);
    intersection_bb[2] = MIN(bb1[2], bb2[2]);
    intersection_bb[3] = MIN(bb1[3], bb2[3]);
    //area
    intersection_area = areaBB(intersection_bb);
    return intersection_area;
}

float overlap(BoundingBox bb1, BoundingBox bb2)
{
	float 	intersection_area, _overlap;

	if(std::isnan(bb1[0]) || std::isnan(bb1[1]) || std::isnan(bb1[2]) || std::isnan(bb1[3])) return 0.;
	if(std::isnan(bb2[0]) || std::isnan(bb2[1]) || std::isnan(bb2[2]) || std::isnan(bb2[3])) return 0.;
	if(bb1[0] > bb2[2]) return 0.0;
	if(bb1[1] > bb2[3]) return 0.0;
	if(bb1[2] < bb2[0]) return 0.0;
	if(bb1[3] < bb2[1]) return 0.0;

    intersection_area = intersectionArea(bb1, bb2);
    _overlap = areaBB(bb1) + areaBB(bb2) - intersection_area; //União

    _overlap = intersection_area/_overlap;

    return _overlap;
}

//Imagem integral II(x,y) = sum{x'<=x,y'<=y}I(x',y')^p (Viola-Jones)
Mat integralImage(Mat image, int p)
{
	if(image.empty()) return Mat();

	int width = image.size().width,
        height = image.size().height,
		x, y;
	Mat ii(width, height, CV_32FC1), //imagem integral transposta (height X width)
        s(image.size(), CV_32FC1), //soma da coluna até y (width x height)
        grayImg;
	uchar *row; //Linha da imagem
	float 	*row_s, *row_s_1, //row_s = row(x), row_s = row(x-1) linhas da imagem s
			*col_s, *col_ii, *col_ii_1; //col de col_ii = col(y) col_ii_1 = col(y-1) colunas de ii

	if(image.channels() == 3) cvtColor(image, grayImg, COLOR_BGR2GRAY);
	else image.copyTo(grayImg);

	row = grayImg.ptr<uchar>(0);
	row_s = s.ptr<float>(0);
	for(x = 0; x < width; x++, row++, row_s++)
		row_s[0] = pow((float)row[0]/255., p); // s(x,0) = s(x,-1) + i(x,0)^p = i(x,0)^p

	for(y = 1; y < height; y++){
		row = grayImg.ptr<uchar>(y);
		row_s = s.ptr<float>(y);
		row_s_1 = s.ptr<float>(y-1);
		for(x = 0; x < width; x++, row++, row_s++, row_s_1++)
			row_s[0] = row_s_1[0] + pow((float)row[0]/255., p); //s(x,y) = s(x,y-1) + i(x,y)^p
    }

    transpose(s, s); //linha vira coluna, facilita acesso a mat

	col_s = s.ptr<float>(0);
	col_ii = ii.ptr<float>(0);
    for(y = 0; y < height; y++, col_ii++, col_s++)
		col_ii[0] = col_s[0]; //ii(0,y) = ii(-1, y) + s(0,y) = s(0,y)

    for(x = 1; x < width; x++){
		col_s = s.ptr<float>(x);
		col_ii = ii.ptr<float>(x);
		col_ii_1 = ii.ptr<float>(x-1);
		for(y = 0; y < height; y++, col_s++, col_ii++, col_ii_1++){
			col_ii[0] = col_ii_1[0] + col_s[0]; //ii(x,y) = ii(x-1, y) + s(x,y)
        }
    }

	transpose(ii,ii); //Coluna vira linha. Corrige ii pq usou ela transposta
	s.release();
    return ii;
}

//Retorna variância de cada patch delimitado pelas bounding boxes
float* getVariance(Mat image, vector<BoundingBox> bb_list){
	if(bb_list.empty()) return NULL;

    Mat e_x = integralImage(image, 1),
        e_x2 = integralImage(image, 2);
    int width_window, height_window,
		width = image.size().width,
		height = image.size().height,
		index = 0;
    vector<BoundingBox>::iterator window;
    // |_e1_|   | , | e2 |    | , |____e3_| , |        | , |       |
    // |        |   |    |    |   |       |   |   e4   |   |   |-e-|
    float 	e, e_1, e_2, e_3, e_4,
            e2, e2_1, e2_2, e2_3, e2_4,
			*variances = (float*) malloc(sizeof(float)*bb_list.size());

    for(window = bb_list.begin(); window != bb_list.end(); window++, index++){
		//Testa limites
		if(	(*window)[0] < 0 || (*window)[0] >= width  ||
			(*window)[2] < 0 || (*window)[2] >= width  ||
			(*window)[1] < 0 || (*window)[1] >= height ||
			(*window)[3] < 0 || (*window)[3] >= height ){
				variances[index] = 0.;
				continue;
			}

        width_window = widthBB(*window);
        height_window = heightBB(*window);

        if((*window)[0] == 0){
            e_1 = e_2 = e2_1 = e2_2 = 0.;
            if((*window)[1] == 0) e_3 = e2_3 = 0.;
            else {
                e_3 = e_x.at<float>((*window)[1] - 1, (*window)[2]);
                e2_3 = e_x2.at<float>((*window)[1] - 1, (*window)[2]);
            }
        }
        else if((*window)[1] == 0){
            e_1 = e_3 = e2_1 = e2_3 = 0.;
            e_2 = e_x.at<float>((*window)[3], (*window)[0] - 1);
            e2_2 = e_x2.at<float>((*window)[3], (*window)[0] - 1);
        }
        else {
            e_1 = e_x.at<float>((*window)[1] - 1, (*window)[0] - 1);
            e_2 = e_x.at<float>((*window)[3], (*window)[0] - 1);
            e_3 = e_x.at<float>((*window)[1] - 1, (*window)[2]);

            e2_1 = e_x2.at<float>((*window)[1] - 1, (*window)[0] - 1);
            e2_2 = e_x2.at<float>((*window)[3], (*window)[0] - 1);
            e2_3 = e_x2.at<float>((*window)[1] - 1, (*window)[2]);
        }
        e_4 = e_x.at<float>((*window)[3], (*window)[2]);
        e2_4 = e_x2.at<float>((*window)[3], (*window)[2]);

        e = e_4 - e_3 - e_2 + e_1;
        e2 = e2_4 - e2_3 - e2_2 + e2_1;
        variances[index] = e2/(width_window*height_window); //E[x²]
        variances[index]-= SQR(e/(width_window*height_window)); //E[x²] - (E[x])²
    }
    e_x.release();
    e_x2.release();

    return variances;
}

//Retorna tipo de imagem e número de canais
int matType(Mat frame, int &channels){
	int t = frame.type(),
        ch = t/CV_DEPTH_MAX + 1,
        basic_type = t%CV_DEPTH_MAX;
	channels = ch;
	return basic_type;
}

//Calcula distância entre bounding boxes
inline float distance(BoundingBox bb1, BoundingBox bb2){
	return 1. - overlap(bb1, bb2);
}

//Clusterização hierarquica aglomerativa
int clusterConf(Mat frame, vector<BoundingBox> d_answers, vector<double> d_conf, vector<BoundingBox> &c_answers, vector<double> &c_conf, bool show){
	//Init
	int	num_samples = d_answers.size(),	//Número de respostas do detector
		num_clusters = num_samples,		//No inicio cada amostra forma um cluster
		next_index = num_samples,		//Indice do proximo cluster criado
		c1 = 0, c2 = 0, cluster1, cluster2;
	float min_dist, d;	//Menor distância intercluster encontrada na iteração
	vector<BoundingBox>::iterator bb1, bb2;
	vector<int> cluster_index, 	//Indice do cluster de cada resposta
				c_count;		//Número de amostras em cada cluster

	switch(num_samples){
        case 0:
            return 0;
        case 1:
            c_answers.push_back(d_answers[0]);
            c_conf.push_back(d_conf[0]);
            return 1;
        case 2:
            if(distance(d_answers[0], d_answers[1]) <= MAX_DIST_CLUSTER){ //Um cluster
                c_answers.resize(1);
                c_answers[0][0] = (d_answers[0][0] + d_answers[1][0])/2.;
                c_answers[0][1] = (d_answers[0][1] + d_answers[1][1])/2.;
                c_answers[0][2] = (d_answers[0][2] + d_answers[1][2])/2.;
                c_answers[0][3] = (d_answers[0][3] + d_answers[1][3])/2.;
                c_conf.push_back((d_conf[0] + d_conf[1])/2.);
                return 1;
            }
            //Dois clusters
            c_answers.push_back(d_answers[0]);
            c_answers.push_back(d_answers[1]);
            c_conf.push_back(d_conf[0]);
            c_conf.push_back(d_conf[1]);
            return 2;
	}

	//Distância dos pares
	Mat dist(num_clusters, num_clusters, CV_32F);

	cluster_index.resize(num_samples);

	cluster_index[num_samples-1] = num_samples-1;
	dist.at<float>(num_clusters-1, num_clusters-1) = INFINITY;
	for(bb1 = d_answers.begin(), c1 = 0; bb1 != d_answers.end()-1; bb1++, c1++){
		cluster_index[c1] = c1;
		dist.at<float>(c1,c1) = INFINITY; //Infinito para n ser retornado como distância minima

		c2 = c1 + 1;
		for(bb2 = d_answers.begin() + c2; bb2 != d_answers.end(); bb2++, c2++)
			dist.at<float>(c1,c2) = dist.at<float>(c2,c1) = distance(*bb1, *bb2);
	}

	//Linkage. O processo é repetido até q tenha apenas um cluster e a distância máxima intracluster seja menor que um threshold
	while(num_clusters > 1){
		//Encontra par com menor distancia. Como num_clusters é maior que 1, sempre encontra uma dist válida
		min_dist = INFINITY;
		for(int i = 0; i < num_samples-1; i++){
			for(int j = i + 1; j < num_samples; j++){
				d = dist.at<float>(i,j);
				if(cluster_index[i] != cluster_index[j] && d < min_dist){
					min_dist = d;
					c1 = i;
					c2 = j;
				}
			}
		}

		if(min_dist > MAX_DIST_CLUSTER) break;

		//Agrupa clusters e atualiza dist
		num_clusters--;
		cluster1 = cluster_index[c1];
		cluster2 = cluster_index[c2];

		cluster_index[c1] = next_index;
		for(int j = 0; j < num_samples; j++){
			if(j != c1 && cluster_index[j] != cluster1 && cluster_index[j] != cluster2)
				dist.at<float>(c1,j) = dist.at<float>(j, c1) = MIN(dist.at<float>(c1,j), dist.at<float>(c2,j));
			else
				dist.at<float>(c1,j) = dist.at<float>(j, c1) = INFINITY;
		}

		for(int i = 0; i < num_samples; i++){
			if(i != c1 && (cluster_index[i] == cluster1 || cluster_index[i] == cluster2)){
				cluster_index[i] = next_index;
				for(int j = 0; j < num_samples; j++)
						dist.at<float>(i,j) = dist.at<float>(j, i) = dist.at<float>(c1,j);
			}
		}
		next_index++;
	}

	//Corrige indices
	int aux = -1, cluster;
	for(int i = 0; i < num_samples; i++){
		cluster = cluster_index[i];
		if(cluster < 0) continue;

		cluster_index[i] = aux;

		for(int j = i+1; j < num_samples; j++)
			if(cluster_index[j] == cluster)
				cluster_index[j] = aux;
		aux--;
	}

	//Visualização das amostras. A cor indica o cluster
	if(show){
		Mat view;
		int ind;
		float step = 255./num_clusters;
		frame.copyTo(view);
		for(int i = 0; i < num_samples; i++){
			ind = -cluster_index[i] - 1;
			rectangle(view, Point2f(d_answers[i][0], d_answers[i][1]), Point2f(d_answers[i][2], d_answers[i][3]), Scalar(ind*step, ind*step, ind*step), 2);
		}
		imshow("Clusters", view);
		view.release();
	}

	//Final: Calcula centro dos clusters e confiabilidade média
	c_answers.resize(num_clusters);
	c_conf.resize(num_clusters);
	c_count.resize(num_clusters);

	for(int i = 0; i < num_clusters; i++){
		c_answers[i][0] = c_answers[i][1] = c_answers[i][2] = c_answers[i][3] = 0.;
		c_conf[i] = 0.;
		c_count[i] = 0;
	}

	for(int i = 0; i < num_samples; i++){
		cluster = -cluster_index[i] - 1;
		c_answers[cluster][0]+= d_answers[i][0];
		c_answers[cluster][1]+= d_answers[i][1];
		c_answers[cluster][2]+= d_answers[i][2];
		c_answers[cluster][3]+= d_answers[i][3];
		c_conf[cluster]+= d_conf[i];
		c_count[cluster]++;
	}

	for(int i = 0; i < num_clusters; i++){
		c_answers[i][0]/= c_count[i];
		c_answers[i][1]/= c_count[i];
		c_answers[i][2]/= c_count[i];
		c_answers[i][3]/= c_count[i];
		c_conf[i]/= c_count[i];
	}

	return num_clusters;
}
