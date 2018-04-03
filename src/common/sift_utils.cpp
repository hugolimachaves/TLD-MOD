#include "sift_utils.hpp"
#include <math.h>

//Converte Imagem do opencv para imagem do vlfeat
vl_sift_pix *Mat2vl(Mat imgCV)
{
	int width = imgCV.size().width,height = imgCV.size().height;
    uint8_t 	*cv_img = (uint8_t*)imgCV.data,*cv_pixel = &cv_img[0];
    vl_sift_pix *vl_img = (vl_sift_pix*)malloc(width*height*sizeof(vl_sift_pix)),*vl_pixel = &vl_img[0];
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            *vl_pixel = (float)(*cv_pixel);
            vl_pixel++;
            cv_pixel++;
		}
    }
    return vl_img;
}

//Desenha keypoints no frame
void desenha_key(Mat frame, SiftFeature *keypoints)
{
    SiftFeature *kp;
    if(keypoints == NULL) return;
    for (kp = keypoints; kp != NULL; kp = kp->next)
    {
		circle(frame, cvPoint(kp->point.x, kp->point.y), 0, Scalar(0.0, 255.0, 0.0), 1, 8, 0);
		circle(frame, cvPoint(kp->point.x, kp->point.y), kp->radius, Scalar(255.0, 255.0, 255.0), 1, 8, 0);
	}
}

////Distância euclideana entre dois descritores
double euclideanDistance(vl_sift_pix d1[DESCRIPTOR_SIFT_SIZE], vl_sift_pix d2[DESCRIPTOR_SIFT_SIZE])
{
	double dist = 0.;
	for(int i = 0; i < DESCRIPTOR_SIFT_SIZE; i++) dist+= SQR(d2[i] - d1[i]);
	return sqrt(dist);
}

//Extrai keypoints de gray_img
SiftFeature* sift(Mat gray_img)
{
    SiftFeature *root, *pointer, *pre_pointer;
	if(gray_img.empty()) return NULL;
	int ch, type;
	type = matType(gray_img, ch);
	if(type != UCHAR || ch != 1)
	{
		printf("Invalid image.\n");
		return NULL;
	}

	int n_octaves = -1,	//Até qual resoluçao processar. Quanto maior, maior os blobs (imagens de escala menores)
		n_levels = 3,	//Número de niveis em cada octave
		o_min = -1,		//Indice do octave inicial. Negativo: Dobra a resolução -> features menores. 0: Original. Positivo: Metade da resoluçao -> features maiores.
		err;
	double	edge_thresh  = 200, //Filtra pontos em arestas. Menores valores -> menos pontos
			peak_thresh  = 0,	//Filtra pontos em blobs de baixo constraste. Maiores valores -> menos pontos
			magnif       = -1;	//?
	VlSiftFilt *filter = NULL;

	//Criar filtro
    filter = vl_sift_new(gray_img.size().width, gray_img.size().height, n_octaves, n_levels, o_min);
    if(!filter) printf("Could not create SIFT filter.");

	//Definir valores para filtrar keypoints
    vl_sift_set_edge_thresh(filter, edge_thresh);
    vl_sift_set_peak_thresh(filter, peak_thresh);
    if (magnif >= 0) vl_sift_set_magnif(filter, magnif);

	//converte imagem, vl_sift_pix tem valores entre 0 e 255
    vl_sift_pix *img = Mat2vl(gray_img);

	//Pega número correto de octaves
	n_octaves = vl_sift_get_noctaves(filter);

	int n_keys, n_angles;
	double angles[4];

	//Processa primeiro octave
	err = vl_sift_process_first_octave(filter, img);
	if(err) n_octaves = 0;

	vl_sift_detect(filter);

	VlSiftKeypoint const *keys = vl_sift_get_keypoints(filter);
	n_keys = vl_sift_get_nkeypoints(filter);

	root = new SiftFeature();
	pre_pointer = root->next = NULL;
	pointer = root;

	for(int j = 0; j < n_keys; j++)
	{
        pointer->point.x = keys[j].x;
        pointer->point.y = keys[j].y;
        pointer->radius = keys[j].sigma;
        pointer->next = new SiftFeature();
        n_angles = vl_sift_calc_keypoint_orientations(filter, angles, &keys[j]);
        for (vl_size q = 0; q < (unsigned)n_angles; q++)
            vl_sift_calc_keypoint_descriptor(filter, pointer->descriptor, &keys[j], angles[q]);
        pre_pointer = pointer;
        pointer = pointer->next;
	}

    for(int i = 1; i < n_octaves; i++)
    {
        err = vl_sift_process_next_octave(filter) ;
        if(err) break;
		vl_sift_detect(filter);
		VlSiftKeypoint const *keys = vl_sift_get_keypoints(filter);
        n_keys = vl_sift_get_nkeypoints(filter);

        for (int j = 0; j < n_keys; j++)
        {
			pointer->point.x = keys[j].x;
			pointer->point.y = keys[j].y;
			pointer->radius = keys[j].sigma;
			pointer->next = new SiftFeature();
            n_angles = vl_sift_calc_keypoint_orientations(filter, angles, &keys[j]);

            for (vl_size q = 0; q < (unsigned)n_angles; q++)
                vl_sift_calc_keypoint_descriptor(filter, pointer->descriptor, &keys[j], angles[q]);
            pre_pointer = pointer;
            pointer = pointer->next;
        }
    }

    vl_sift_delete(filter);
    free(img);

    if(pre_pointer == NULL) {
        delete root;
        return NULL;
    }

    pre_pointer->next = NULL;
    delete pointer;
    return root;
}

//Limpa lista encadeada
SiftFeature* setClear(SiftFeature *root){
    SiftFeature *s, *next;

    if(root == NULL) return NULL;

    for(s = root, next = root->next; s->next != NULL; s = next, next = s->next) delete s;

    delete s;

    return NULL;
}

int setSize(SiftFeature *root){
    SiftFeature *s;
    int set_count = 0;

    if(root == NULL) return 0;

    for(s = root; s != NULL; s = s->next, set_count++);

    return set_count;
}

//Elimina aleatoriamente new_size - curr_size features
SiftFeature* randomForgetting(SiftFeature *sift_set, int curr_size, int new_size){
	SiftFeature *pointer,  //Percorre sift_set
				*prev_pointer = NULL, //Ponteiro para o anterior
				*aux; //
	vector<bool> forget_indexes(curr_size, false); //Marca indices que serão removidos
	vector<bool>::iterator forget; //Percorre forget indexes

	forget = forget_indexes.begin() + new_size;
	for(int i = new_size; i < curr_size; i++, forget++) (*forget) = true;
	random_shuffle(forget_indexes.begin(), forget_indexes.end());

	forget = forget_indexes.begin();
	for(pointer = sift_set; pointer != NULL; forget++){
		if(*forget){
			if(prev_pointer == NULL){ //Ainda não manteve nenhum elemento em sift_set
				prev_pointer = pointer;
				sift_set = pointer->next;
				pointer = pointer->next;
				delete prev_pointer;
				prev_pointer = NULL;
			}
			else{
				prev_pointer->next = pointer->next;
				delete pointer;
				pointer = prev_pointer->next;
			}
		}
		else{
			prev_pointer = pointer;
			pointer = pointer->next;
		}
	}

	return sift_set;
}

//Casa feature com contexto
bool contextMatching(vl_sift_pix descr[DESCRIPTOR_SIFT_SIZE], SiftFeature *c_t, SiftFeature *d_t, int t){
	if(c_t == NULL && d_t == NULL) return false;

	double 	dist,
			smallest_dist1 = INFINITY, //menor distância
			smallest_dist2 = INFINITY; //2ª menor
	SiftFeature *pointer;
	int has2 = 0;

	for(pointer = c_t; pointer != NULL; pointer = pointer->next, has2++){
		dist = euclideanDistance(pointer->descriptor, descr);
		//ordena
		if(dist < smallest_dist2){
			if(dist >= smallest_dist1){ //s1 dist | s2
				smallest_dist2 = dist;
			}
			else{ // dist s1 | s2
				smallest_dist2 = smallest_dist1;
				smallest_dist1 = dist;
			}
		}
		//else s1 s2 | dist
	}
	for(pointer = d_t; pointer != NULL; pointer = pointer->next, has2++){
		dist = euclideanDistance(pointer->descriptor, descr);
		//ordena
		if(dist < smallest_dist2){
			if(dist >= smallest_dist1){ //s1 dist | s2
				smallest_dist2 = dist;
			}
			else{ // dist s1 | s2
				smallest_dist2 = smallest_dist1;
				smallest_dist1 = dist;
			}
		}
		//else s1 s2 | dist
	}

	if(has2 < 2) dist = smallest_dist1;
	else dist = smallest_dist1 / smallest_dist2;

	//O matching não foi único
	if(dist >= LAMBDA){
		return false;
	}

	return true;
}

//Detecta features de contexto que estão dentro da bounding box.
//s_t = features extraídas na janela, o_t = features de contexto dentro da bb, a_t = features fora da bb
//Retorna número de oclusões
int occlusionDetection(SiftFeature *s_t, SiftFeature **o_t, SiftFeature **a_t, BoundingBox corrected_bb, SiftFeature *c_t, SiftFeature *d_t, int t){
	///NOTE: Similar a cInit
	SiftFeature *s_pointer, //Percorre s_t
				*a_pointer = NULL, //Percorre a_t
				*o_pointer = NULL, //Percorre o_t
				*aux_pointer; //Marca posições que devem ser apagadas
	int o_size = 0;

	for(s_pointer = s_t; s_pointer != NULL;){
		//Fora da bounding box = contexto
		if(s_pointer->point.x < corrected_bb[0] || s_pointer->point.x > corrected_bb[2] || s_pointer->point.y < corrected_bb[1] || s_pointer->point.y > corrected_bb[3]){
			if(*a_t == NULL){
				s_pointer->flag = t;
				*a_t = a_pointer = s_pointer;
			}
			else {
				s_pointer->flag = t;
				a_pointer->next = s_pointer;
				a_pointer = s_pointer;
			}
			s_pointer = s_pointer->next;
		}
		else{
			//Features dentro de bb que casam com o contexto = oclusão
			if(contextMatching(s_pointer->descriptor, c_t, d_t, t)){
				if(*o_t == NULL){
					s_pointer->flag = t;
					*o_t = o_pointer = s_pointer;
				}
				else {
					s_pointer->flag = t;
					o_pointer->next = s_pointer;
					o_pointer = s_pointer;
				}
				s_pointer = s_pointer->next;
				o_size++;
			}
			else{
				aux_pointer = s_pointer;
				s_pointer = s_pointer->next;
				delete aux_pointer;
			}

		}
	}

	if(a_pointer != NULL) a_pointer->next = NULL;
	if(o_pointer != NULL) o_pointer->next = NULL;
	s_t = NULL;

	return o_size;
}
