#include "Detector.hpp"
#define DEBUG_1 0
#define DEBUG_2 0
#define DEBUG_3 0
#define DEBUG_4 1

//Geração das scanning windows
#define SCALE_STEP          1.2
#define SCALE_EXP           10      //escala = SCALE_STEP^[-SCALE_EXP, SCALE_EXP]
#define SHIFT_STEP          0.1

//Geração de amostras
#define DEFAULT_PATCH_SIZE  15		//Patches normalizados = 15x15
#define NUM_POS_SAMPLE		10		//Número de amostras positivas extraídas da vizinhança
#define NUM_WARPS_INIT      20		//Número de amostras geradas sinteticamente no treino
#define NUM_WARPS_UPDATE   	10		//Número de amostras geradas sinteticamente no retreino
#define NUM_NEG_SAMPLE		100		//Número máximo de amostras negativas extraídas da vizinhança
#define GOOD_WINDOW_TH		0.6		//Overlap mínimo para amostra positiva
#define BAD_WINDOW_TH		0.2		//Overlap máximo para amostra negativa

//Comitê
#define NUM_FEAT_FERN       13      //Numero de comparações por fern
#define NUM_CODE            8192	//Possiveis codigos gerados por fern com as features escolhidas 2^NUM_FEAT_FERN
#define	AVERAGE_THRESHOLD 	0.5		//Média dos votos mínima
#define FERN_MAX_OUTPUT     100     //Número maximo de amostras depois de passar pelo comitê

//NN
#define NN_LAMBDA			0.1	 //Margem de classificação
#define THE_SAME			0.95 //Acima dessa confiabilidade eh considerada a msma amostra
#define NN_THETA 			0.6 //Margem do classificador 1NN

#define VARIANCE_THRESHOLD 	0.5		//Porcentagem mínima da variância do candidato
#define POSITIVE_WINDOW 	"Positive samples"
#define NEGATIVE_WINDOW 	"Negative samples"

#define VAR_TEST(var) (var > (obj_variance * VARIANCE_THRESHOLD))
#define ENS_TEST(vote) (vote > AVERAGE_THRESHOLD)
#define NN_TEST(r_conf) (r_conf > NN_THETA)
#define NN_TEST_MARGIN_P(r_conf) (r_conf - NN_THETA <= NN_LAMBDA) //Fracos positivos e negativos
#define NN_TEST_MARGIN_N(r_conf) (NN_THETA - r_conf < NN_LAMBDA) //Fracos negativos e positivos

typedef int Feature[4]; 		//Par de pontos a ser comparado (x1, y1, x2, y2)


#if DEBUG_3 == 1
    static int contador =0;
#endif // DEBUG_3

static float 				obj_variance;						//Variância da bb inicial
static int  				positive[NUM_FERNS*NUM_CODE],		//Contador de amostras positivas em cada classificador
							negative[NUM_FERNS*NUM_CODE];		//Contador de amostras negativas em cada classificador
static double 				posteriors[NUM_FERNS*NUM_CODE];		//Posteriores dos ferns concatenados
static vector<BoundingBox> 	scanning_windows;       			//Posições dos candidatos
static Feature 				features[NUM_FERNS*NUM_FEAT_FERN];	//Features dos ferns concatenadas
static vector<ModelSample> 	object_model[2];         			//1 = amostras positivas, 0 = amostras negativas
static Point2f				p_pointer,							//Próxima posição disponivel para amostras positivas
							n_pointer;							//Próxima posição disponivel para amostras negativas
static Mat 					dataset_p,							//Imagem com amostras positivas
							dataset_n;							//Imagem com amostras negativas
static vector<Candidate>	last_ens_candidates;				//Candidatos que passaram pelo ensemble na última detecção

vector<BoundingBox>	good_windows, 						//Posiçoes das amostras positivas
					bad_windows;						//Posiçoes das amostras negativas
vector<ModelSample>	good_samples,						//Amostras positivas prontas para o treino
					bad_samples;						//Amostras negativas prontas para o treino
BoundingBox			good_windows_hull;					//Envoltoria das amostras positivas
vector<Candidate> 	candidates;							//Amostras candidatas

ModelSample::ModelSample(){
	for(int i = 0; i < NUM_FERNS; i++) code[i] = 0;
}

ModelSample::~ModelSample(){
	image.release();
	ens_img.release();
	nn_img.release();
}

//Similaridade entre amostras. No intervalo [0., 1.]
//voce passa uma imagem para comparção aqui
double ModelSample::similarity(Mat pattern2)
{
	//assegurar que esta no tamanho padrao e que pattern2 tenha a mesma dimensao que nn_img
	if(nn_img.cols != DEFAULT_PATCH_SIZE || nn_img.rows != DEFAULT_PATCH_SIZE || nn_img.rows != pattern2.rows || nn_img.cols != pattern2.cols)
		return 0.;

	double 	corr = 0.,norm1 = 0.,norm2 = 0.,norm, gray1, gray2;
	double *nn_row, *nn_cell, *patt_row, *patt_cell;


	#if DEBUG_3
        std::cout<<"Entrou no similarity: "<< contador++ <<std::endl;
		namedWindow( "nn_img", WINDOW_AUTOSIZE ); // Create a window for display.
		resizeWindow("nn_img", 200,200);
		imshow("nn_img",nn_img);
		namedWindow( "pattern2", WINDOW_AUTOSIZE ); // Create a window for display.
		resizeWindow("pattern2", 200,200);
		imshow("pattern2",pattern2);
		waitKey(100);
	#endif



	for(int j = 0; j < DEFAULT_PATCH_SIZE; j++)
	{
		nn_row = nn_img.ptr<double>(j);
		patt_row = pattern2.ptr<double>(j);
		nn_cell = &nn_row[0];
		patt_cell = &patt_row[0];
		//vai fazer bitwise operation
		for(int i = 0; i < DEFAULT_PATCH_SIZE; i++, nn_cell++, patt_cell++)
		{
			gray1 = nn_cell[0];
			gray2 = patt_cell[0];
			corr += gray1 * gray2;
			norm1 += gray1 * gray1;
			norm2 += gray2 * gray2;
			#if DEBUG_3
				std::cout<<"gray1 :"<<gray1<<"- gray2: "<<gray2<<std::endl;
			#endif
		}
	}
	//norma a ser dividida
	norm = sqrt(norm1*norm2);

	if(norm1 < EPSILON && norm2 < EPSILON) corr = 1.; //Duas imagens praticamente vazias
	else if(norm > EPSILON) corr/= norm;
	else return 0.;

	corr = (corr + 1.)/2.;
	//retorna algo semelhante a uma correlação
	return corr;
}

Candidate::Candidate()
{
	for(int i = 0; i < NUM_FERNS; i++) code[i] = 0;
}

Candidate::~Candidate()
{
	image.release();
	ens_img.release();
	nn_img.release();
}

//Similaridade entre amostra e modelo. No intervalo [0., 1.]
double modelSimilatiry(vector<ModelSample> model, Mat pattern, int &isin)
{
	if(model.empty())
	{
        isin = -1;
        return 0.;
	}



	double max_sim, sim;
	vector<ModelSample>::iterator sample;
	int max_pos = -1, i = 1;

	isin = -1;

    max_sim = (*(model.begin())).similarity(pattern);
    max_pos = 0;

	for(sample = model.begin()+1; sample != model.end(); sample++, i++)
	{
        sim = (*sample).similarity(pattern);
		if(sim > max_sim)
		{
			max_sim = sim;
			max_pos = i;
		}
	}

	if(max_sim >= THE_SAME)
		isin = max_pos;

	return max_sim;
}

//Similaridade relativa. Amostra e todo o modelo. No intervalo [0., 1.]
double relativeSimilarity(Mat pattern, int &isin_p, int &isin_n)
{
	double pos_sim, neg_sim, sim;
	isin_p = isin_n = -1;

	if(object_model[1].empty())
		return 0.;
	if(object_model[0].empty())
		return 1.;

	neg_sim = modelSimilatiry(object_model[0], pattern, isin_n); //em [0., 1.]
	pos_sim = modelSimilatiry(object_model[1], pattern, isin_p); //em [0., 1.]

//	if(pos_sim + neg_sim != 0) sim = pos_sim / (pos_sim + neg_sim);
//	else sim = 0.;
	if(pos_sim + neg_sim != 2)
		sim = (1. - neg_sim) / (2 - pos_sim - neg_sim);
	else if(pos_sim == neg_sim)
		sim = 0.5;
	else
		sim = 0.;
	return sim;
}

void fastSimilarity(Mat nn_img, double &relative_sim, double &conservative_sim, int &isin_p, int &isin_n){
	int i = 0, max_pos = -1,pos_size = ceil(object_model[1].size()/2.);
	double 	pos_sim = 0.,neg_sim = 0.,earl_sim = 0.,sim;
	vector<ModelSample>::iterator sample;
	isin_p = isin_n = -1;
	if(object_model[1].empty())
	{
        relative_sim = conservative_sim = 0.;
        return;
    }
	if(object_model[0].empty())
	{
        relative_sim = conservative_sim = 1.;
        return;
    }

	//Amostras positivas
	for(sample = object_model[1].begin(); sample != object_model[1].end(); sample++, i++)
	{
        sim = (*sample).similarity(nn_img);
		if(sim > pos_sim)
		{
			pos_sim = sim;
			max_pos = i;
		}
		if(i < pos_size && sim > earl_sim)
			earl_sim = sim;
	}
	if(pos_sim >= THE_SAME)
		isin_p = max_pos;
	max_pos = -1;
	i = 0;

	//Amostras negativas
	for(sample = object_model[0].begin(); sample != object_model[0].end(); sample++, i++)
	{
        sim = (*sample).similarity(nn_img);
		if(sim > neg_sim)
		{
			neg_sim = sim;
			max_pos = i;
		}
	}
	if(neg_sim > THE_SAME)
		isin_n = max_pos;

//	if(pos_sim + neg_sim != 0) relative_sim = pos_sim / (pos_sim + neg_sim);
//	else relative_sim = 0.;
//	if(earl_sim + neg_sim != 0) conservative_sim = earl_sim / (earl_sim + neg_sim);
//	else conservative_sim = 0.;

	if(pos_sim + neg_sim != 2)
		relative_sim = (1. - neg_sim) / (2 - pos_sim - neg_sim);
	else if(pos_sim == neg_sim)
		relative_sim = 0.5;
	else
		relative_sim = 0.;

	if(earl_sim + neg_sim != 2)
		conservative_sim = (1. - neg_sim) / (2 - earl_sim - neg_sim);
	else if(earl_sim == neg_sim)
		conservative_sim = 0.5;
	else
		conservative_sim = 0.;
}

//Similaridade com a metade inicial do modelo. No intervalo [0.5, 1.]
double earliestSimilatiry(vector<ModelSample> model, Mat pattern){
    if(model.empty())
		return 0.;

	double max_sim, sim;
	vector<ModelSample>::iterator   sample, middle = model.begin() + ceil(model.size()/2.);

    max_sim = (*(model.begin())).similarity(pattern);

	for(sample = model.begin()+1; sample != middle; sample++){
        sim = (*sample).similarity(pattern);
		if(sim > max_sim) max_sim = sim;
	}
	return max_sim;
}

//Similaridade conservativa. Amostra, modelo negativo e metade do positivo. No intervalo [1/3, 2/3]
double conservativeSimilarity(Mat pattern)
{
	double earl_sim, neg_sim, sim;
	int isin; //ignorado

	if(object_model[1].empty())
		return 0.;
	if(object_model[0].empty())
		return 1.;

	neg_sim = modelSimilatiry(object_model[0], pattern, isin); //[0.,1.]
	earl_sim = earliestSimilatiry(object_model[1], pattern); //[0.,1.]
//	if(earl_sim + neg_sim != 0) sim = earl_sim / (earl_sim + neg_sim);
//	else sim = 0.;
	if(earl_sim + neg_sim != 2)
		sim = (1. - neg_sim) / (2 - earl_sim - neg_sim);
	else if(earl_sim == neg_sim)
		sim = 0.5;
	else sim = 0.;
		return sim;
}

//Scanning windows - seta posições a serem scaneadas
void scanningWindows(int img_width, int img_height, int bb_width, int bb_height){
    float scale = pow(SCALE_STEP,-SCALE_EXP);
	float window_shift_x,window_shift_y;
	int window_witdh,window_height;
    BoundingBox window;

    for(int i = 0; i < 2*SCALE_EXP+1; i++, scale*=SCALE_STEP)
	{
        window_witdh = rint(scale*bb_width);
        window_height = rint(scale*bb_height);
        if(window_witdh < MIN_BB || window_height < MIN_BB)
			continue;

        window_shift_x = rint(SHIFT_STEP*window_witdh);
        window_shift_y = rint(SHIFT_STEP*window_height);

        for(int left = 2; left < img_width - window_witdh; left+=window_shift_x)
		{
            for(int top = 2; top < img_height - window_height; top+=window_shift_y)
			{
                window[0] = left;
                window[1] = top;
                window[2] = left + window_witdh;
                window[3] = top + window_height;
                scanning_windows.push_back(window);
            }
        }
    }
}

//Define, permuta e seleciona features
void setFeatures()
{
	//Todos os possíveis pares verticais (x fixo) de pontos do patch normalizado. (Tamanho calculado a partir dos for's aninhados)
	Feature points[DEFAULT_PATCH_SIZE*DEFAULT_PATCH_SIZE*(DEFAULT_PATCH_SIZE-1)/2];
	Feature *point;
	int indexes[DEFAULT_PATCH_SIZE*DEFAULT_PATCH_SIZE*(DEFAULT_PATCH_SIZE-1)], //Indices dos pares de pontos verticais(pares) e horizontais (impares)
		ind_points = 0,
		tam_indexes = DEFAULT_PATCH_SIZE*DEFAULT_PATCH_SIZE*(DEFAULT_PATCH_SIZE-1),
		*index;

	point = &points[0];
	index = &indexes[0];
	//Gera todas as possibilidades verticais. Duplica indice em indexes para considerar comparações horizontais.
    for(int x = 0; x < DEFAULT_PATCH_SIZE; x++)
	{
		for(int y1 = 0; y1 < DEFAULT_PATCH_SIZE-1; y1++)
		{
			for(int y2 = y1+1; y2 < DEFAULT_PATCH_SIZE; y2++, ind_points++)
			{
				(*point)[0] = x;
				(*point)[1] = y1;
				(*point)[2] = x;
				(*point++)[3] = y2;
				(*index++) = ind_points << 1; //2*ind_points = ponto na vertical
				(*index++) = (ind_points << 1)|1; //2*ind_points + 1 = mesmo ponto transposto (horizontal, y fixo)
			}
		}
    }

    int aux;
	//Permuta comparações horizontais e verticais (indexes)
    for(int i = 0; i < tam_indexes; i++)
	{
        ind_points = rand()%tam_indexes;
        aux = indexes[i];
        indexes[i] = indexes[ind_points];
        indexes[ind_points] = aux;
    }

	bool vertical;
	//Seleciona as primeiras features e corrige coordenadas (par = vertical, impar = horizontal).
	point = &features[0];
	for(int i = 0; i < NUM_FERNS*NUM_FEAT_FERN; i++)
	{
		vertical = indexes[i]%2==0;
		ind_points = indexes[i]/2;
		if(vertical)
		{
			(*point)[0] = points[ind_points][0];
			(*point)[1] = points[ind_points][1];
			(*point)[2] = points[ind_points][2];
			(*point++)[3] = points[ind_points][3];
		}
		else
		{ //Horizontal -> transpoe cada ponto da comparação
			(*point)[0] = points[ind_points][1];
			(*point)[1] = points[ind_points][0];
			(*point)[2] = points[ind_points][3];
			(*point++)[3] = points[ind_points][2];
		}
	}

	if(_DEBUG_DETECTOR && _DEBUG_WORKSPACE)
	{
		Mat a(150,150,CV_8UC1, Scalar(0.));
		Point2f p1,p2;
		for(int fern = 0; fern < NUM_FERNS; fern++)
		{
			int index = fern*NUM_FEAT_FERN;
			for(int i = 0; i < NUM_FEAT_FERN; i++, index++)
			{
				p1.x = features[index][0]*10;
				p1.y = features[index][1]*10;
				p2.x = features[index][2]*10;
				p2.y = features[index][3]*10;
				circle(a, p1, 2, Scalar(255.),2);
				circle(a, p2, 2, Scalar(255.),2);
				line(a, p1, p2, Scalar(255.));
			}
			imshow("Feature", a);
            waitKey(800);
			a-=a;
		}
	}
}

//Gera feature code
int getCode(Mat img, int fern_index)
{
    Feature *fern_features = &features[fern_index*NUM_FEAT_FERN];

    int code = 0;
	float x, y;
    uchar color1, color2;
    for(int i = 0; i < NUM_FEAT_FERN; i++, fern_features++)
	{
        color1 = img.at<uchar>(fern_features[0][1],fern_features[0][0]);
        color2 = img.at<uchar>(fern_features[0][3],fern_features[0][2]);
        code<<= 1;
        if(color2 > color1) code|=1;
    }
    return code;
}

//Seleciona janelas proximas e distantes (se negative = true). Calcula envoltórias das amostras positivas.
//Retorna no máximo NUM_POS_SAMPLE amostras positivas e todas as negativas encontradas, ambas ordenadas pelo overlap.
void setOverlapingWindows(BoundingBox position, bool negative = true)
{
	good_windows.clear();
	bad_windows.clear();

	//Ordena janelas por overlap
	int sw_size = scanning_windows.size(),index = 0;
	SortElement *overlap_vector = (SortElement*) malloc(sizeof(SortElement)*sw_size),*overlap_pointer = &overlap_vector[0];
	vector<BoundingBox>::iterator window;

	for(window = scanning_windows.begin(); window != scanning_windows.end(); window++, index++, overlap_pointer++)
	{
		(*overlap_pointer).index = index;
		(*overlap_pointer).val = overlap(position, *window);
	}

	qsort(overlap_vector, sw_size, sizeof(SortElement), maxVal);

	good_windows_hull[0] = good_windows_hull[1] = INFINITY;
	good_windows_hull[2] = good_windows_hull[3] = 0;

	//Seleciona janelas positivas (maiores overlaps) e calcula envoltoria
	for(int i = 0; i < sw_size; i++)
	{
		if(good_windows.size() < NUM_POS_SAMPLE && overlap_vector[i].val > GOOD_WINDOW_TH)
		{
			index = overlap_vector[i].index;
			good_windows.push_back(scanning_windows[index]);

			//Menor envoltoria convexa das bounding boxes selecionadas
			if(scanning_windows[index][0] < good_windows_hull[0])
				good_windows_hull[0] = scanning_windows[index][0];
			if(scanning_windows[index][1] < good_windows_hull[1])
				good_windows_hull[1] = scanning_windows[index][1];
			if(scanning_windows[index][2] > good_windows_hull[2])
				good_windows_hull[2] = scanning_windows[index][2];
			if(scanning_windows[index][3] > good_windows_hull[3])
				good_windows_hull[3] = scanning_windows[index][3];
		}
		else break;
	}

	//Seleciona janelas negativas (menores overlaps)
	if(negative)
	{
		for(int i = sw_size - 1; i >= 0; i--)
		{
			if(overlap_vector[i].val < BAD_WINDOW_TH)
			{
				index = overlap_vector[i].index;
				bad_windows.push_back(scanning_windows[index]);
			}
			else
				break;
		}
	}

	free(overlap_vector);
}

//Normalização para o ensemble: ajusta tamanho
void normalize(Mat img, Mat blur_img, BoundingBox bb, float shift_x, float shift_y, Mat &sample, Mat &ens_img, Mat &nn_img){
	ens_img.release();
	nn_img.release();
	sample.release();

	ens_img = Mat(DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE, blur_img.type());
	nn_img = Mat(DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE, CV_64FC1);
	sample = Mat(DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE, img.type());

	int step_w = floor(widthBB(bb)/DEFAULT_PATCH_SIZE),step_h = floor(heightBB(bb)/DEFAULT_PATCH_SIZE),x = floor(bb[0] - shift_x + step_w/2.), //Posição correta nas imagens
		y = floor(bb[1] - shift_y + step_h/2.);
	double *nn_row, *nn_cell, nn_mean = 0.;
	uchar 	*img_row, *blur_row, *ens_row, *sample_row,
			*img_cell, *blur_cell, *ens_cell, *sample_cell;

	for(int j = 0; j < DEFAULT_PATCH_SIZE; j++)
	{
		img_row = img.ptr<uchar>(y);
		blur_row = blur_img.ptr<uchar>(y);
		ens_row = ens_img.ptr<uchar>(j);
		sample_row = sample.ptr<uchar>(j);
		nn_row = nn_img.ptr<double>(j);

		img_cell = &img_row[x];
		blur_cell = &blur_row[x];
		ens_cell = &ens_row[0];
		nn_cell = &nn_row[0];
		sample_cell = &sample_row[0];

		for(int i = 0; i < DEFAULT_PATCH_SIZE; img_cell+=step_w, blur_cell+=step_w, ens_cell++, nn_cell++, sample_cell++, i++)
		{
			ens_cell[0] = blur_cell[0];
			nn_cell[0] = (double)img_cell[0];
			sample_cell[0] = img_cell[0];
			nn_mean+= nn_cell[0];
		}
		y+=step_h;
	}

	nn_mean/= DEFAULT_PATCH_SIZE*DEFAULT_PATCH_SIZE;
	nn_img-=nn_mean;
}

//Seleciona patches positivos e gera positivos sinteticos
void setPositiveSamples(Mat frame, int num_warps)
{
    //Amostras extraídas e sintéticas
	PatchGenerator generator(0, 0, 5, true, 1-0.02, 1+0.02,-20*CV_PI/180, 20*CV_PI/180, -20*CV_PI/180, 20*CV_PI/180); //Gerador de warps
	RNG& rng = theRNG(); //Random number generator
	Size hull_size; //Deforma a envoltória e extrai amostras, reduz custo de deformar cada amostras
	Point2f hull_center;
	Mat warped, blur_img;
	int index;
	vector<BoundingBox>::iterator window;

	hull_size.width = widthBB(good_windows_hull);
	hull_size.height = heightBB(good_windows_hull);
	hull_center.x = good_windows_hull[0] + hull_size.width/2;
	hull_center.y = good_windows_hull[1] + hull_size.height/2;

	index = 0;
	good_samples.resize(num_warps*good_windows.size());

    //Utiliza a primeira amostra original
	getRectSubPix(frame, hull_size, hull_center, warped);
	SMOOTH(warped, blur_img);

	for(int i = 0; i < num_warps; i++)
	{
        if(i > 0)
		{
			generator(frame, hull_center, warped, hull_size, rng); //Gera warping aleatório
			SMOOTH(warped, blur_img);
		}

        for(window = good_windows.begin(); window != good_windows.end(); window++)
		{
            //Prepara padrão usado pelo comitê e nn. E normaliza tamanho da imagem para ser mostrada
			normalize(warped, blur_img, *window, good_windows_hull[0], good_windows_hull[1], good_samples[index].image,
																good_samples[index].ens_img, good_samples[index].nn_img);
			//Avalia amostras positivas no comitê
			for(int fern = 0; fern < NUM_FERNS; fern++)
				good_samples[index].code[fern] = getCode(good_samples[index].ens_img, fern);
            index++;
        }
        warped.release();
        blur_img.release();
	}
}

//Seleciona patches negativos. Nenhum negativo sintetico eh gerado.
void setNegativeSamples(Mat frame)
{
	vector<BoundingBox>::iterator window;
	ModelSample bad_sample;
	Mat blur_img;
	float 	*var = getVariance(frame, bad_windows),
			*var_pointer = &var[0];

	SMOOTH(frame, blur_img);

	//Seleciona amostras distantes e com variância alta
	for(window = bad_windows.begin(); window != bad_windows.end(); window++, var_pointer++)
	{
		//if(!VAR_TEST(*var_pointer)) continue;
		if((*var_pointer) > (obj_variance * VARIANCE_THRESHOLD/2.))
			continue;

		normalize(frame, blur_img, *window, 0, 0, bad_sample.image, bad_sample.ens_img, bad_sample.nn_img);

		//Avalia amostras negativas
        for(int fern = 0; fern < NUM_FERNS; fern++)
            bad_sample.code[fern] = getCode(bad_sample.ens_img, fern);

		bad_samples.push_back(bad_sample); //Adiciona cópia
	}
	bad_sample.image.release();
	bad_sample.ens_img.release();
	bad_sample.nn_img.release();

	if(var)
		free(var);
}

//Medidas dos ferns
double votes(int code[NUM_FERNS])
{
	double average = 0.;
	for (int fern = 0; fern < NUM_FERNS; fern++)
		average += posteriors[fern*NUM_CODE + code[fern]];
	return average/NUM_FERNS;
}

//Atualiza probabilidades do comitê
void updatePosteriors(ModelSample sample, bool positive_label)
{
	int p, n, index;

	for(int fern = 0; fern < NUM_FERNS; fern++)
	{
		index = fern*NUM_CODE + sample.code[fern];
		if(positive_label)
			positive[index]++;
		else
			negative[index]++;

		p = positive[index];
		n = negative[index];

		posteriors[index] = (double) p/(n+p);
	}
}

//Treina comite
void ensTrain(int bootstrap = 2)
{
	int g_size = good_samples.size(),
		b_size = bad_samples.size(),
		samples_size = g_size + b_size;
	vector<int> indexes(samples_size);
	vector<int>::iterator index;
	vector<ModelSample>::iterator sample;

	for(int i = 0; i < samples_size; i++)
		indexes[i] = i;

	random_shuffle(indexes.begin(), indexes.end()); //Permuta todos
	//random_shuffle(indexes.begin(), indexes.begin()+g_size); //Permuta positivos
	//random_shuffle(indexes.begin() + g_size, indexes.end()); //Permuta negativos
	for(int i = 0; i < bootstrap; i++){
		for(index = indexes.begin(); index != indexes.end(); index++)
		{
			if(*index < g_size)
			{
				sample = good_samples.begin() + (*index);
				if(!ENS_TEST(votes((*sample).code)))
				{ //Falso negativo
					updatePosteriors(*sample, true);
				}
			}
			else
			{
				sample = bad_samples.begin() + (*index) - g_size;
				if(ENS_TEST(votes((*sample).code)))
				{ //Falso positivo
					updatePosteriors(*sample, false);
				}
			}
		}
	}

	indexes.clear();
}

//Adiciona amostra na imagem do dataset
void addSample(Mat normalized_sample, int label, int isin)
{
	if(label == 0)
	{
        normalized_sample.copyTo(dataset_n(Rect(n_pointer.x, n_pointer.y, DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE)));
        n_pointer.y+= DEFAULT_PATCH_SIZE;
        if(n_pointer.y + DEFAULT_PATCH_SIZE > dataset_n.size().height)
		{
            n_pointer.y = 0;
            n_pointer.x+= DEFAULT_PATCH_SIZE;
            if(n_pointer.x + DEFAULT_PATCH_SIZE > dataset_n.size().width)
			{
                n_pointer.x = n_pointer.y = 0;
                dataset_n = 255.; //Limpa
            }
        }
	}
	else
	{
        if(isin == -1 || isin == object_model[1].size()-2)
            normalized_sample.copyTo(dataset_p(Rect(p_pointer.x, p_pointer.y, normalized_sample.cols, normalized_sample.rows)));
        else
		{
            vector<ModelSample>::iterator sample;
            Mat img;
            Point2f aux;
            int shift = 0;
            //aux.x = ((isin + 1)*DEFAULT_PATCH_SIZE) / dataset_p.size().height;
            //aux.y = ((isin + 1)*DEFAULT_PATCH_SIZE) % dataset_p.size().height;
            aux.x = 0.;
            aux.y = 0.;
            //if(object_model[1].size() > 400) shift = (object_model[1].size()/400)*400;
            for(sample = object_model[1].begin() /*+ shift + isin + 1*/; sample != object_model[1].end(); sample++)
			{
                (*sample).image.copyTo(img); //Realoca se não tiver o tamanho correto. Não precisa de release.
                img.copyTo(dataset_p(Rect(aux.x, aux.y, DEFAULT_PATCH_SIZE, DEFAULT_PATCH_SIZE)));
                aux.y+= DEFAULT_PATCH_SIZE;
                if(aux.y >= dataset_p.size().height)
				{
                    aux.y = 0;
                    aux.x+= DEFAULT_PATCH_SIZE;
					if(aux.x >= dataset_p.size().width)
					{
						aux.x = aux.y = 0;
						dataset_p = 255.; //Limpa
					}
                }
            }
        }
		p_pointer.y+= DEFAULT_PATCH_SIZE;
		if(p_pointer.y + DEFAULT_PATCH_SIZE > dataset_p.size().height)
		{
			p_pointer.y = 0;
			p_pointer.x+= DEFAULT_PATCH_SIZE;
			if(p_pointer.x + DEFAULT_PATCH_SIZE > dataset_p.size().width)
			{
                p_pointer.x = p_pointer.y = 0;
                dataset_p = 255.; //Limpa
			}
		}
	}
}

//Treina nearest neighbor. Se ens_filter = true, utiliza para o treinamento apenas amostras que passam pelo comitê
void nnTrain(bool show, bool ens_filter = false)
{
	int g_size = good_samples.size(), //Numero das amostras positivias (?)
		b_size = bad_samples.size(), ////Numero das amostras negativas (?)
		isin_p, isin_n, //ignorados
		vote_pointer = 0;
	double conf, vote;
	vector<int> indexes(g_size);
	vector<int>::iterator index;
	vector <ModelSample>::iterator sample;
	SortElement *vote_list = (SortElement*)malloc(sizeof(SortElement)*b_size);

	for(int i = 0; i < g_size; i++)
		indexes[i] = i;

	sample = bad_samples.end()-1;
	for(int i = b_size - 1; i >= 0; i--, sample--)
	{
		vote = votes((*sample).code);
		if(ens_filter && !ENS_TEST(vote))
			continue; //Ignora amostras classificadas corretamente pelo comitê
        vote_list[vote_pointer].val = (float)vote;
        vote_list[vote_pointer].index = i;
        vote_pointer++;
	}

	qsort(vote_list, vote_pointer, sizeof(SortElement), maxVal);
	for(int i = 0; i < vote_pointer; i++)
		indexes.push_back(g_size + vote_list[i].index);

	if(vote_pointer > NUM_NEG_SAMPLE)
		indexes.erase(indexes.begin() + g_size + NUM_NEG_SAMPLE, indexes.end());

	//random_shuffle(indexes.begin()+1, indexes.end()); //Permuta todos
	//random_shuffle(indexes.begin()+1, indexes.begin()+g_size); //Permuta positivos
	//random_shuffle(indexes.begin()+g_size, indexes.end()); //Permuta negativos
	//random_shuffle(indexes.begin()+1, indexes.begin()+NUM_NEG_SAMPLE); //Permuta parte

	for(index = indexes.begin(); index != indexes.end(); index++)
	{
		if(*index < g_size)
		{
			sample = good_samples.begin() + (*index);
			conf = relativeSimilarity((*sample).nn_img, isin_p, isin_n);
			//if(NN_TEST_MARGIN_P(conf)){ //Falsos negativos e fracos positivos
			if(!NN_TEST(conf))
			{ //Falsos negativos
				if(isin_p == -1 || isin_p == object_model[1].size() - 1)
					object_model[1].push_back(*sample); // adicionando ao object model
				else
					object_model[1].insert(object_model[1].begin() + isin_p + 1, *sample); //Insere depois da amostra similar
				if(show)
					addSample((*sample).image, 1, isin_p);
			}
		}
		else
		{
			sample = bad_samples.begin() + (*index) - g_size;
			conf = relativeSimilarity((*sample).nn_img, isin_p, isin_n);
			if(NN_TEST_MARGIN_N(conf))
			{ //Falso positivo e fracos negativos
			//if(NN_TEST(conf)){ //Falso positivo
				object_model[0].push_back(*sample); // adicionando ao object model
				if(show)
					addSample((*sample).image, 0, -1);
			}
		}
	}

	indexes.clear();
}

//Treina detector
void Train(Mat frame, BoundingBox &position, bool show)
{
	clock_t start_g = clock();
	DetClear();

	float 	frame_width = frame.size().width,
			frame_height = frame.size().height,
			bb_width = widthBB(position), //É 0 se alguma coordenada eh nan
			bb_height = heightBB(position);

	dataset_p = Mat(20*DEFAULT_PATCH_SIZE, 20*DEFAULT_PATCH_SIZE, frame.type(), Scalar(255.)); //Imagem vazia e branca. Capacidade: 400 amostras
	dataset_n = Mat(20*DEFAULT_PATCH_SIZE, 20*DEFAULT_PATCH_SIZE, frame.type(), Scalar(255.));

	scanningWindows(frame_width, frame_height, bb_width, bb_height);
	setFeatures();
	setOverlapingWindows(position);

	if(good_windows.empty() || bad_windows.empty())
	{
		DetClear();
		return;
	}

	position[0] = good_windows[0][0];
	position[1] = good_windows[0][1];
	position[2] = good_windows[0][2];
	position[3] = good_windows[0][3];

	vector<BoundingBox> bb_list;
	bb_list.push_back(position);
	float *var = getVariance(frame, bb_list);
	obj_variance = *var;
	bb_list.clear();
	free(var);

	setPositiveSamples(frame, NUM_WARPS_INIT);
	setNegativeSamples(frame); //Etapa mais lenta.

	ensTrain();
	nnTrain(show);

	good_windows.clear();
	bad_windows.clear();
	good_samples.clear();
	bad_samples.clear();

	if(show)
	{
		imshow(POSITIVE_WINDOW, dataset_p);
		imshow(NEGATIVE_WINDOW, dataset_n);
	}
	clock_t end_g = clock();

	double elapsed = (double)(end_g - start_g)*1000./CLOCKS_PER_SEC;
	if(_DEBUG_DETECTOR && _DEBUG_PERF)
	{
        printf("Neg: %d samples. Pos: %d samples\n", (int)object_model[0].size(), (int)object_model[1].size());
        printf("Trainning: %.2f ms\n", elapsed); ///NOTE: considera tempo do waitKey
        getchar();
    }
}

bool Retrain(Mat frame, BoundingBox &position, bool show)
{
	clock_t start_g = clock();

	float 	frame_width = frame.size().width,
			frame_height = frame.size().height,
			bb_width = widthBB(position), //É 0 se alguma coordenada eh nan
			bb_height = heightBB(position);
    double r_sim;
	Mat blur_img;
	ModelSample t_answer;
	int isin_n, isin_p;

	if(bb_width < MIN_BB || bb_height < MIN_BB || bb_width > frame_width || bb_height > frame_width)
	{
        printf("Invalid size\n");
        return false;
    }

	SMOOTH(frame, blur_img);
	normalize(frame, blur_img, position, 0, 0, t_answer.image, t_answer.ens_img, t_answer.nn_img);
	r_sim = relativeSimilarity(t_answer.nn_img, isin_p, isin_n);

	vector<BoundingBox> bb_list;
	bb_list.push_back(position);
	float *var = getVariance(frame, bb_list);
	bb_list.clear();

	//Baixa variância || Forte negativo (Não é fraco negativo e nem positivo). Mudança brusca. || Similaridade forte com os negativos
	if(!VAR_TEST(*var))
	{
        printf("Low variance\n");
		free(var);
		t_answer.image.release();
		t_answer.ens_img.release();
		t_answer.nn_img.release();
		return false;
	}
	if(r_sim < 0.5)
	{
        printf("Fast change\n");
        free(var);
		t_answer.image.release();
		t_answer.ens_img.release();
		t_answer.nn_img.release();
		return false;
	}
	if(isin_n != -1)
	{
        printf("In negative model.\n");
		free(var);
		t_answer.image.release();
		t_answer.ens_img.release();
		t_answer.nn_img.release();
		return false;
	}
	free(var);

	setOverlapingWindows(position);

	if(good_windows.empty() || bad_windows.empty())
	{
		t_answer.image.release();
		t_answer.ens_img.release();
		t_answer.nn_img.release();
        return false;
    }

	setPositiveSamples(frame, NUM_WARPS_UPDATE);
	setNegativeSamples(frame); //Etapa mais lenta.

	ensTrain();

    good_windows.clear();
	good_samples.clear();
	bad_windows.clear();
	bad_samples.clear();

	vector<Candidate>::iterator candidate;
	ModelSample sample;
	float overl;
	int sw_index;

	for(candidate = last_ens_candidates.begin(); candidate != last_ens_candidates.end(); candidate++)
	{
        sw_index = (*candidate).scanning_windows_index;
        overl = overlap(scanning_windows[sw_index], position);
        if(overl < BAD_WINDOW_TH)
		{
			//normalize(frame, blur_img, *bb, 0, 0, sample.image, sample.ens_img, sample.nn_img);
			sample.image = (*candidate).image;
			sample.ens_img = (*candidate).ens_img;
			sample.nn_img = (*candidate).nn_img;
			bad_samples.push_back(sample);

			sample.image.release();
			sample.ens_img.release();
			sample.nn_img.release();
		}
    }

    good_samples.push_back(t_answer);
    nnTrain(show);

	good_samples.clear();
	bad_samples.clear();
	good_samples.clear();

	if(show)
	{
		imshow(POSITIVE_WINDOW, dataset_p);
		imshow(NEGATIVE_WINDOW, dataset_n);
	}
	clock_t end_g = clock();

	double elapsed = (double)(end_g - start_g)*1000./CLOCKS_PER_SEC;
	if(_DEBUG_DETECTOR && _DEBUG_PERF)
	{
        printf("Neg: %d samples. Pos: %d samples\n", (int)object_model[0].size(), (int)object_model[1].size());
        printf("Retrain: %.2f ms\n", elapsed); ///NOTE: considera tempo do waitKey
    }
    return true;
}

//Salva respostas que ainda são válidas em arquivo com nome name
void saveAnswers(char *classifier_name, int option)
{ //default = Sem conf, 1 = variance, 2 = average_vote, 3 = r_conf e c_conf
	int window;
    char name[60];
	vector<Candidate>::iterator candidate;

    sprintf(name, "output/%s.yml", classifier_name);

    FILE *answers = fopen(name, "w");
    if(!answers)
		return;

    fprintf(answers, "%%YAML:1.0\n");
    fprintf(answers, "option: %d\nn_samples: %d\n", option, (int)candidates.size());

    switch(option)
	{
		case 1:
			fprintf(answers, "obj_variance: %.2f\nthreshold: %.2f\n", obj_variance, VARIANCE_THRESHOLD);
		break;
		case 2:
			fprintf(answers, "threshold: %.2f\n", AVERAGE_THRESHOLD);
		break;
		case 3:
			fprintf(answers, "threshold: %.2f\n", NN_THETA);
		break;
    }

    fprintf(answers, "samples:\n");

    candidate = candidates.begin();
    for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
        window = (*candidate).scanning_windows_index;
        fprintf(answers, " - {bb: [%.2f %.2f %.2f %.2f]", scanning_windows[window][0], scanning_windows[window][1], scanning_windows[window][2], scanning_windows[window][3]);
        switch(option)
		{
			case 1:
				fprintf(answers, ", variance: %.3f}\n", (*candidate).variance);
			break;
			case 2:
				fprintf(answers, ", average_vote: %.3f}\n", (*candidate).average_vote);
			break;
			case 3:
				fprintf(answers, ", r_sim: %.3f, c_sim: %.3f}\n", (*candidate).r_sim, (*candidate).c_sim);
			break;
			default:
				fprintf(answers, "}\n");
			break;
        }
    }

    fclose(answers);
}

//Visualizar candidatos que passaram pela última etapa realizada
void showRemainingScanningWindows(Mat image)
{
	Mat sg;
	int window;
	vector<Candidate>::iterator candidate;
	Point2f p1, p2;
	Scalar color(255.,255.,255.);

    image.copyTo(sg);

    for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
        window = (*candidate).scanning_windows_index;
        p1.x = scanning_windows[window][0];
        p1.y = scanning_windows[window][1];
        p2.x = scanning_windows[window][2];
        p2.y = scanning_windows[window][3];
        rectangle(sg, p1, p2, color);
	}
	imshow("Remaining scanning windows", sg);
    waitKey(800);
	sg.release();
}

//Cascade classifier
//Step 1: Filtrar pela variância
void varianceFilter(Mat frame)
{
	vector<Candidate>::iterator candidate;
	int i = 0;
	float *var = getVariance(frame, scanning_windows);

	for(candidate = candidates.begin(); candidate != candidates.end(); candidate++, i++)
		(*candidate).variance = var[i];

	candidates.erase(
		remove_if(candidates.begin(), candidates.end(), [](Candidate c){return (c.variance < obj_variance * VARIANCE_THRESHOLD);}),
		candidates.end()
	);

	free(var);
}

//Extrai da imagem os candidatos válidos
void setCandidates(Mat frame)
{
    vector<Candidate>::iterator candidate;
    int sw_index;
    Mat blur_img;

    SMOOTH(frame, blur_img);
    for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
		sw_index = (*candidate).scanning_windows_index;
		//ens_img recebe uma image filtrada com um filtro gaussiano (para evitar ruidos), isso é feito na função normalize
		//nn_img é uma imagem normalizada, com média zero... também na função normalize
		normalize(frame, blur_img, scanning_windows[sw_index], 0, 0, (*candidate).image, (*candidate).ens_img, (*candidate).nn_img);

        //Avalia candidatos no comitê
		for(int fern = 0; fern < NUM_FERNS; fern++)
			(*candidate).code[fern] = getCode((*candidate).ens_img, fern);
	}

    blur_img.release();
}

//Step 2: Comite de classificadores bayesianos
void ensembleClassifier(){
    vector<Candidate>::iterator candidate;

	for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
		(*candidate).average_vote = votes((*candidate).code);
	}

	candidates.erase(
		remove_if(candidates.begin(), candidates.end(), [](Candidate c){return (c.average_vote <= AVERAGE_THRESHOLD);}),
		candidates.end()
	);

	if(candidates.size() > FERN_MAX_OUTPUT)
	{
        //Ordem descendente de votos
		sort(candidates.begin(), candidates.end(), [](Candidate c1, Candidate c2){return (c1.average_vote > c2.average_vote);});
		candidates.erase(candidates.begin() + FERN_MAX_OUTPUT, candidates.end());
	}

	last_ens_candidates.assign(candidates.begin(), candidates.end()); //Guarda saída do comitê. Cópia independente
}

//Step 3: NN classifier - Possivelmente passa o endereco onde seram ecsritos as posicoes onde foram encontradas os objetos e suas confiabilidade
void nearestNeighbor(vector<BoundingBox> &positions, vector<double> &conf, int numeroQuadro)
{
	vector<Candidate>::iterator candidate;
	int isin_p, isin_n; //ignorados
	Mat view;
	if(DEBUG_2)
	{
		vector<ModelSample>::iterator goodIterator;
		for ( goodIterator = object_model[1].begin(); goodIterator < object_model[1].end(); goodIterator++ )
		{
			namedWindow( "goodSamples", WINDOW_AUTOSIZE ); // Create a window for display.
			resizeWindow("goodSamples", 200,200);
			imshow("goodSamples",(*goodIterator).image);
			waitKey(100);
		}
	}

	for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
		fastSimilarity((*candidate).nn_img, (*candidate).r_sim, (*candidate).c_sim, isin_p, isin_n);

		if(DEBUG_1)
		{
			namedWindow( "Debug", WINDOW_AUTOSIZE ); // Create a window for display.
			resizeWindow("Debug", 200,200);
			imshow("Debug",(*candidate).nn_img );
			waitKey(100);
		}
		/* TODO
			Vamos pegar todos os candidatos dentro desse for, olhar as delimitacoes da sua BB e compararar com o object model,
		*/

		if(DEBUG_4)
		{
			std::cout<<"scanningWindows index 0: "<<scanning_windows[(*candidate).scanning_windows_index][0]<<std::endl;
			std::cout<<"scanningWindows index 1: "<<scanning_windows[(*candidate).scanning_windows_index][1]<<std::endl;
		}
	}



	candidates.erase(
		remove_if(candidates.begin(), candidates.end(), [](Candidate c){return !NN_TEST(c.r_sim);}),
		candidates.end()
	);

	for(candidate = candidates.begin(); candidate != candidates.end(); candidate++)
	{
		conf.push_back((*candidate).c_sim); //Adiciona cópia
		positions.push_back(scanning_windows[(*candidate).scanning_windows_index]); //Adiciona cópia
		//positions são as coordenadas de onde foram encontrados objetos!
	}

	if(DEBUG_1)
	{
		std::cout<<"index 0: "<<scanning_windows[(*candidate).scanning_windows_index][0]<<" index 1: "
		<<scanning_windows[(*candidate).scanning_windows_index][1]<<" index 2: "<<
		scanning_windows[(*candidate).scanning_windows_index][2]<<" index 3: "<<
		scanning_windows[(*candidate).scanning_windows_index][3]<<std::endl;
		vector<BoundingBox>::iterator iterador;

		for (iterador = positions.begin(); iterador != positions.end(); iterador++ )
		{
            //std::cout<<"positions[0][0]:"<<(*iterador)[0]<<std::endl;
            //vector<BoundingBox>::iterator iterador;
            for ( auto it = iterador->begin() ; it != iterador->end(); it++ )
            {
                std::cout<<"positions:"<<*it<<std::endl;
            }
		}
	}
}

//Retorna bounding boxes que contém o objeto em 'positions' e suas respectivas similaridades conservativas em 'd_conf'
bool Detect(Mat frame, vector<BoundingBox> &detector_positions, vector<double> &d_conf, int frame_number)
{
	last_ens_candidates.clear();
	clock_t start_detection = clock();
	detector_positions.clear();
    d_conf.clear();

	if(object_model[0].empty() && object_model[1].empty())
		return false; //Não foi treinado

    vector<Candidate>::iterator candidate;
    clock_t start_t, end_t;
    double elapsed;

    int i = 0;
	candidates.resize(scanning_windows.size());
	for(candidate = candidates.begin(); candidate != candidates.end(); candidate++, i++)
        (*candidate).scanning_windows_index = i;

	char name[30];
	if(_DEBUG_DETECTOR)
	{
        if(_DEBUG_PERF)
		{
			printf("Initial: %d candidates\n", (int)candidates.size());
		}
		if(_DEBUG_WORKSPACE)
		{
			sprintf(name, "initial%d", frame_number);
			saveAnswers(name, 0);
			showRemainingScanningWindows(frame);
		}
    }

    start_t = clock();
	varianceFilter(frame); // remove janela de baixa variancia (?)
	end_t = clock();

    if(_DEBUG_DETECTOR)
	{
        if(_DEBUG_PERF)
		{
			printf("Variance filter: %d candidates\n", (int)candidates.size());
			elapsed = (double)(end_t - start_t)*1000 / CLOCKS_PER_SEC;
			printf("Elapsed: %.3lf ms\n", elapsed);
		}
		if(_DEBUG_WORKSPACE)
		{
			sprintf(name, "variance%d", frame_number);
			saveAnswers(name, 1);
			showRemainingScanningWindows(frame);
		}
    }

    setCandidates(frame);
    start_t = clock();
    ensembleClassifier();
    end_t = clock();

    if(_DEBUG_DETECTOR)
    {
        if(_DEBUG_PERF)
        {
			printf("Ensemble Classifier: %d candidates\n", (int)candidates.size());
			elapsed = (double)(end_t - start_t)*1000 / CLOCKS_PER_SEC;
			printf("Elapsed: %.3lf ms\n", elapsed);
		}
        if(_DEBUG_WORKSPACE)
        {
			sprintf(name, "ensemble%d", frame_number);
			saveAnswers(name, 2);
			showRemainingScanningWindows(frame);
		}
    }

    start_t = clock();
    nearestNeighbor(detector_positions, d_conf, frame_number); //saidas do detector, similaridade conservativa das amostras (?)
    end_t = clock();

    if(_DEBUG_DETECTOR){
        if(_DEBUG_PERF){
			printf("NN Classifier: %d candidates\n", (int)candidates.size());
			elapsed = (double)(end_t - start_t)*1000 / CLOCKS_PER_SEC;
			printf("Elapsed: %.3lf ms\n", elapsed);
		}
        if(_DEBUG_WORKSPACE){
			sprintf(name, "nn%d", frame_number);
			saveAnswers(name, 3);
			showRemainingScanningWindows(frame);
		}
    }

    candidates.clear();

    clock_t end_detection = clock();
    elapsed = (double)(end_detection - start_detection) / CLOCKS_PER_SEC;
    if(_DEBUG_DETECTOR && _DEBUG_PERF){
		printf("Detection: %.3lfs\n", elapsed);
	}

    if(detector_positions.empty())
		return false;

	return true;
}

//Limpa detector
void DetClear(){
	object_model[0].clear();
	object_model[1].clear();
	scanning_windows.clear();
	dataset_p.release();
	dataset_n.release();
	p_pointer.x = p_pointer.y = n_pointer.x = n_pointer.y = 0;
	last_ens_candidates.clear();

	for(int i = 0; i < NUM_FERNS*NUM_CODE; i++){
        posteriors[i] = 0.;
        positive[i] = negative[i] = 0;
    }
}
