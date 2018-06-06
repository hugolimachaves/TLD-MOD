#include "TLD.hpp"

#ifdef _WIN32
    #define CLEAR() system("cls");
#else
    #define CLEAR() system("clear");
#endif // _WIN32

#define PRINT_STATUS if(print_status) cout << "Frame: " << frame_count << endl;

#define WINDOW "Tracking"
#define OBJECT_WINDOW "Object"

static int                 	delay = 5,                          //Delay para waitKey() do opencv
                            image_list_size = 0,                //Número de frames no arquivo do IMAGE_LIST
                            image_list_pointer = 0,             //Número de frames mostrados do IMAGE_LIST
                            filter,                             //Filtro de pontos
                            frame_count = 0,                    //Contador de frames para impressão de status
                            name_count = 0;                     //Contador de frames salvos
static DRAW                 draw_flow;                          //Forma de representar o fluxo
static bool                 draw_bb = false,                    //verdadeiro enquanto (e depois) a bb está sendo definida
                            has_bb = false,                     //verdadeiro quando a bb foi definida
                            pause_cap = false,     	            //pausar captura
                            enable_bb = true, 		            //permite ou desativa a definição da bb pelo mouse
                            reset = false,                      //Captura foi reiniciada?
                            reset_key = false,                  //tecla reset foi pressionada?
                            detect_failure,                     //Ativar detecção de falhas
                            train = false,                      //Treinar detector
                            init_il = false,					//Inicializar integrator_learning
                            enable_detect = false,              //Detecção ligada
                            enable_track,                       //Rastreador ligado
                            save = false,                       //Salvar frame? : imwrite
                            show = false;
static char                 **image_list = NULL,                //Lista de imagens do IMAGE_LIST
                            name[100];                           //Nome do frame: imwrite
static BoundingBox			bb = {NAN, NAN, NAN, NAN},          //Bounding box atual: xmin, ymin, xmax, ymax
							init_bb = {NAN, NAN, NAN, NAN};     //Bounding box inicial para reiniciar: xmin, ymin, xmax, ymax
static VideoCapture         cap;                                //Captura da WEBCAM ou VIDEO
static VIDEO_TYPE           video_type;                         //Tipo de captura

//Inicializa captura: retorna true se inicializou com sucesso. path = null para webcam
bool captureOpen(string path){
	if(video_type == IMAGE_LIST){ //Armazena lista de imagens (caminhos)
		int list_size;
		FILE *list_file = fopen(path.c_str(), "r");
		if(!list_file) {
			cout << "Could not open image list...\n";
			return false;
		}
		fscanf(list_file, "%d\n", &list_size);
		image_list_size = list_size;
		image_list = (char**) malloc(sizeof(char*)*image_list_size);
		image_list_pointer = 0;
		for(int i = 0; i < list_size; i++){
			image_list[i] = (char*) malloc(sizeof(char)*255);
			fgets(image_list[i], 255, list_file);
			for (int j = 0; j < strlen(image_list[i]); j++){
				if (image_list[i][j] == '\n' || image_list[i][j] == '\r')
					image_list[i][j] = '\0';
			}
		}
		fclose(list_file);
	}
	else{
        if(video_type == WEBCAM){
			cap.open(0);
			cap.set(CV_CAP_PROP_FRAME_WIDTH,340);
			cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);
		}
        else cap.open(path);
        if(!cap.isOpened()) {
            cout << "Could not initialize capturing...\n";
            return false;
        }
	}
	return true;
}

void captureClose(){
    if(image_list){
        for(int i = 0; i < image_list_size; i++) free(image_list[i]);
        free(image_list);
    }
    cap.release();
    has_bb = draw_bb = false;
}

//Copia valor da bounding box inicial para a atual
void resetBB(){
    if(!isnan(init_bb[0])){
        bb[0] = init_bb[0];
        bb[1] = init_bb[1];
        bb[2] = init_bb[2];
        bb[3] = init_bb[3];
        has_bb = draw_bb = true;
    }
    else has_bb = draw_bb = false;
}

//Pega próximo frame em tons de cinza ou colorido (grayscale) e se repeat for igual a true a função reinicia quando a captura chega ao fim ou a tecla de reset é pressionada.
void nextFrame(Mat *frame, bool repeat, bool grayscale){
	reset = false;
	frame->release();

	if(video_type == IMAGE_LIST){
        //Reinicia se chegou ao fim ou se a tecla de reset foi pressionada
		if((image_list_pointer >= image_list_size || reset_key) && repeat){
			frame_count = 0;
			resetBB();
			image_list_pointer = 0;
			train = reset = true;
		}
		else if(image_list_pointer >= image_list_size) return;
		reset_key = false;

		if(grayscale) (*frame) = imread(image_list[image_list_pointer++], CV_LOAD_IMAGE_GRAYSCALE);
		else (*frame) = imread(image_list[image_list_pointer++], CV_LOAD_IMAGE_COLOR);
		frame_count++;
	}
	else{
		cap >> (*frame);
		//Reinicia se chegou ao fim ou se a tecla foi pressionada. Para webcam, repeat é automaticamente false
		if((reset_key || frame->empty()) && repeat){
			frame_count = 0;
			resetBB();
			cap.set(CV_CAP_PROP_POS_FRAMES, 0);
			train = reset = true;
			cap >> (*frame);
		}
		else if(frame->empty()) return;
		reset_key = false;

		if(grayscale) cvtColor(*frame, *frame, COLOR_BGR2GRAY);
		frame_count++;
	}
}

//Menu
void help(){
    cout << "-------------------------------------------------------------" << endl;
    cout << "FILTER:" << endl;
    cout << "\tPress 'z' to remove current filters." << endl;
    cout << "\tPress 's' to add SSD filter." << endl;
    cout << "\tPress 'n' to add NCC filter." << endl;
    cout << "\tPress 'f' to add FB filter." << endl;
    cout << "COMPONENTS:" << endl;
    cout << "\tPress 't' to enable/disable tracker." << endl;
    cout << "\tPress 'd' to enable/disable detector." << endl;
    cout << "\tPress 'e' to enable/disable tracker failures detection." << endl;
    cout << "VIEWER:" << endl;
    cout << "\tPress 'p' to pause/resume." << endl;
    cout << "\tPress 'r' to restart." << endl;
    cout << "\tPress 'w' to save current frame." << endl;
    cout << "\tPress '-' to slow down." << endl;
    cout << "\tPress '+' to speed up." << endl;
    cout << "\tPress 'o' to change the flow representation." << endl;
    cout << "\tPress ESC or 'q' to exit." << endl;
    cout << "\tClick and drag to define a bounding box." << endl;
    cout << "\tRight-click to remove bounding box." << endl;
    cout << "-------------------------------------------------------------\n" << endl;
}

//Define bounding box: Pega primeiro ponto e desenha retângulo resultante até que o usuario solte o botão
//Botão direito permite a redefinição da bb
void mouseHandler(int event, int x, int y, int flags, void *param){
    if(!enable_bb) return;
    switch(event){
        case CV_EVENT_LBUTTONDOWN: //Pega primeiro ponto da bb
            if(!has_bb){
				pause_cap = true;
                bb[0] = bb[2] = (float)x;
                bb[1] = bb[3] = (float)y;
                draw_bb = true;
            }
        break;
        case CV_EVENT_MOUSEMOVE:
            if(!has_bb&&draw_bb){ //draw_bb eh verdadeiro quando pelo menos um ponto foi definido
                bb[2] = (float)x;
                bb[3] = (float)y;
            }
        break;
        case CV_EVENT_LBUTTONUP: //Pega segundo ponto
            if(!has_bb&&draw_bb){
                if(x == bb[0] || y == bb[1]) { //Clique instantaneo (sem arrastar) não é considerado bb
                    has_bb = false;
                    draw_bb = false;
                    return;
                }
                //ordena coordenadas
                if(x < bb[0]){
                    bb[2] = bb[0];
                    bb[0] = (float)x;
                }
                else bb[2] = (float)x;
                if(y < bb[1]){
                    bb[3] = bb[1];
                    bb[1] = (float)y;
                }
                else bb[3] = (float)y;
                has_bb = train = init_il = true;
            }
        break;
        case CV_EVENT_RBUTTONUP: //'Apaga' bb
            has_bb = false;
            draw_bb = false;
            bb[0] = bb[1] = bb[2] = bb[3] = NAN;
        break;
    }
}

inline void addFilter(int new_filter){
	filter = filter | new_filter;
	cout << "Current filter(s): ";
	if(filter & SSD_FILTER) cout << "SSD ";
	if(filter & NCC_FILTER) cout << "NCC ";
	if(filter & FB_FILTER) cout << "FB ";
	cout << endl;
}

//Controles do player no teclado. Retorna true se o usuário finalizou o programa (q ou ESC)
bool keyboardCallBack(){
	char key = waitKey(delay);
    bool kill = false;

    if(!show) return false;

    switch(key){
        //Filtros
        case 'Z':
        case 'z':
			cout << "Remove filters" << endl;
			filter = 0;
		break;
		case 'S':
		case 's':
			cout << "Add SSD filter" << endl;
			addFilter(SSD_FILTER);
		break;
		case 'N':
		case 'n':
			cout << "Add NCC filter" << endl;
			addFilter(NCC_FILTER);
		break;
		case 'F':
		case 'f':
			cout << "Add FB filter" << endl;
			addFilter(FB_FILTER);
		break;
		case 'T':
		case 't':
			cout << "Tracker: ";
            if(enable_track) cout << "Disabled" << endl;
            else cout << "Enabled" << endl;
            enable_track = !enable_track;
		break;
		case 'D':
		case 'd':
			cout << "Detector: ";
            if(enable_detect) cout << "Disabled" << endl;
            else {
				cout << "Enabled" << endl;
				if(has_bb) train = true;
			}
			enable_detect = !enable_detect;
		break;
		case 'E':
		case 'e':
			cout << "Tracker failure detection: ";
            if(detect_failure) cout << "Disabled" << endl;
            else cout << "Enabled" << endl;
			detect_failure = !detect_failure;
		break;
		//Controles do visualizador
		case 'P': //Pausar/retomar vídeo
		case 'p':
			if(pause_cap) cout << "Resume" << endl;
			else cout << "Pause" << endl;
			pause_cap = !pause_cap;
        break;
        case 'R': //Reiniciar
        case 'r':
			if(video_type != WEBCAM){
				reset_key = true;
			}
        break;
        case '-': //Desacelerar
			cout << "Slow down. Delay " << delay << endl;
			if(delay < 300) delay+=10;
        break;
        case '+': //Acelerar
			if(delay > 10) delay-=10;
			cout << "Speed up. Delay " << delay << endl;
        break;
        case 'O': //Habilita/desabilita desenho do fluxo
        case 'o':
			switch(draw_flow){
                case DISABLED:
                    cout << "Draw circle" << endl;
                    draw_flow = CIRCLE;
                break;
                case CIRCLE:
                    cout << "Draw flow" << endl;
                    draw_flow = FLOW;
                break;
                case FLOW:
                    cout << "Disable draw" << endl;
                    draw_flow = DISABLED;
                break;
			}
        break;
		case 'Q': //Finalizar
		case 'q':
        case 27:
            cout << "q or Esc key was pressed by user" << endl;
            kill = true;
        break;
        case 'W': //Salvar
        case 'w':
            sprintf(name, "shots/%d.jpg", name_count++);
            save = true;
        break;
    }
    return kill;
}

//Imprime componentes ativos e filtros
void printComponents(int valid, int conf){
	cout << "Components:" << endl;

    cout << "\tTracker: ";
    if(enable_track) cout << "Enabled" << endl;
    else cout << "Disabled" << endl;

    cout << "\tTracker failure detection: ";
    if(detect_failure) cout << "Enabled" << endl;
    else cout << "Disabled" << endl;

    cout << "\tDetector: ";
    if(enable_detect) cout << "Enabled" << endl;
    else cout << "Disabled" << endl;

	cout << endl << "Current filter(s): ";
    if(filter & SSD_FILTER) cout << "SSD ";
    if(filter & NCC_FILTER) cout << "NCC ";
    if(filter & FB_FILTER) cout << "FB ";
    cout << endl << endl;

    cout << "Valid: ";
    switch(valid){
        case 0:
            cout << "NCC" << endl;
        break;
        case 1:
            cout << "Mean" << endl;
        break;
        case 2:
            cout << "Alien" << endl;
        break;
    }

    cout << "Conf: ";
    if(conf == 0) cout << "NCC" << endl;
    else if(conf == 1) cout << "Mean" << endl;
}

//Pega primeira bounding box no arquivo de inicialização ou inicia vídeo no pause para que o usuário selecione o objeto
void initBB(string init_path, int width, int height){
	if(init_path.c_str()) {
		FILE *init = fopen(init_path.c_str(), "r");
		if(init){
			fscanf(init, "%f,%f,%f,%f", &bb[0], &bb[1], &bb[2], &bb[3]);
			fclose(init);

			//Testa limites
			if(bb[0] >= 0 && bb[0] < width && bb[1] >= 0 && bb[1] < height && bb[2] >= 0 && bb[2] < width && bb[3] >= 0 && bb[3] < height){
				init_bb[0] = bb[0];
				init_bb[1] = bb[1];
				init_bb[2] = bb[2];
				init_bb[3] = bb[3];
				enable_bb = false;
				has_bb = draw_bb = true;
				return;
			}
			else cout << "Invalid bounding box." << endl;
		}
		else cout << "Could not open init file" << endl;
	}

	//Se por algum motivo não conseguiu pegar a bb inicial, pausa a visualização.
	cout << "Pause" << endl;
	pause_cap = true;
}

//Salva saída em arquivo
inline void fprintfBB(FILE *bb_file){
	if(!isnan(init_bb[0])){
		if(has_bb) fprintf(bb_file, "%f,%f,%f,%f\n", bb[0], bb[1], bb[2], bb[3]);
		else fprintf(bb_file, "%f,%f,%f,%f\n", NAN, NAN, NAN, NAN);
	}
}

//Mostra imagem na tela e/ou grava
inline void show_save(Mat frame){
	if(draw_bb && !isnan(bb[0])) rectangle(frame, Point2d(bb[0], bb[1]), Point2d(bb[2], bb[3]), Scalar(255.0, 255.0, 255.0), 2.);
	if(show) imshow(WINDOW, frame);
	if(save){
		imwrite(name, frame);
		save = false;
		cout << name << " saved." << endl;
	}
}

//Lê .yml
bool readParameters(char *parameters_path, int &window_size, int &valid, int &conf, string &bb_path, string &video_path, string &init_path, bool &repeat_video, bool &print_status){
    FileStorage parameters;

    parameters.open(parameters_path, FileStorage::READ);
	if(!parameters.isOpened()) return false;

	video_type = (VIDEO_TYPE)((int)parameters["video_type"]);
	parameters["filter"] >> filter;
	parameters["window_size"] >> window_size;
	parameters["bb_path"] >> bb_path;
	parameters["video_path"] >> video_path;
	parameters["init_path"] >> init_path;
	parameters["detect"] >> enable_detect;
	parameters["track"] >> enable_track;
	parameters["detect_failure"] >> detect_failure;
	parameters["show"] >> show;
	parameters["repeat_video"] >> repeat_video;
	parameters["print_status"] >> print_status;
	parameters["valid"] >> valid;
	parameters["conf"] >> conf;

	parameters.release();
	return true;
}

void TLD(char *parameters_path){
	Mat         next_frame,		 	//Next frame
                curr_frame, 		//Current frame
                view_frame, 		//Frame para visualização com pause
                object;     		//Objeto na bounding box
	FILE        *bb_file;			//Arquivo de saída
	BoundingBox tBB,   				//bounding box do tracker
				new_bb;				//bb calculada
	vector<Candidate> d_answers;	//Respostas do detector
	vector<BoundingBox> d_positions; //Saídas do detector
	vector<double> d_conf;			//Similaridade conservativa das amostras
	bool        tracked,    		//Rastreou?
                detected,   		//Detectou?
                final_bb,			//Visivel?
                repeat_video,		//Repetir a sequência
				print_status;		//Imprimir número de frame
	int window_size,
		valid,
		conf;
	string 	bb_path,
			video_path,
			init_path;

	//Inicializa parametros alteraveis pelo usuario
	if(!readParameters(parameters_path, window_size, valid, conf, bb_path, video_path, init_path, repeat_video, print_status)) return;

	repeat_video = show && repeat_video && (video_type != WEBCAM); //Se não mostra, não repete. Webcam n tem repetição

	//Inicia captura do video ou webcam, ou le arquivo da lista de imagens
    if(!captureOpen(video_path)) return;

	//Pega frame inicial
    nextFrame(&curr_frame, repeat_video, true);
    if(curr_frame.empty()){
		cout << "Could not capture frame..." << endl;
		return;
	}

	initBB(init_path, curr_frame.cols, curr_frame.rows);

	//Se conseguiu fazer a leitura da bb inicial
    if(!isnan(init_bb[0])){
		//Abre arquivo para gravar trajetoria, (seq. de bounding boxes)
		bb_file = fopen(bb_path.c_str(), "w");
		if(!bb_file){
			cout << "Could not create bounding box file" << endl;
			return;
		}

		if(enable_detect) Train(curr_frame, bb, show);
		//initSIFT(curr_frame, bb);
		initJudge(curr_frame, bb, valid, conf, show);
		train = false;

		//Grava bb inicial
		fprintf(bb_file, "%lf,%lf,%lf,%lf\n", bb[0], bb[1], bb[2], bb[3]);
	}

	//Prepara janela e menu
	if(show){
		CLEAR();
		help();
		namedWindow(WINDOW, CV_WINDOW_FREERATIO);
		namedWindow(OBJECT_WINDOW, CV_WINDOW_FREERATIO);
		cvSetMouseCallback(WINDOW, mouseHandler, NULL);
	}

	printComponents(valid, conf);

	PRINT_STATUS;

	clock_t start_t, end_t;
	double elapsed;

	//pause_cap = true;
	//enable_bb = true;
	while(1){
		//if(frame_count == 2021) pause_cap = true; //wtf????
        if(!pause_cap){
            nextFrame(&next_frame, repeat_video, true);
            if(next_frame.empty()){
                if(repeat_video) cout << "Could not restart video..." << endl;
				break;
			}
			PRINT_STATUS;

            //Se resetou só tem o primeiro frame, precisa pegar o segundo
            if(!reset){
                tBB[0] = bb[0];
                tBB[1] = bb[1];
                tBB[2] = bb[2];
                tBB[3] = bb[3];
                tracked = detected = final_bb = false;

				if(has_bb){
					if(enable_track && !isnan(tBB[0])){
						start_t = clock();
						tracked = Track(curr_frame, next_frame, tBB, show?draw_flow:DISABLED, filter, detect_failure, window_size);
						end_t = clock();
						elapsed = (double)(end_t - start_t)/CLOCKS_PER_SEC;
						//printf("Tracker Elapsed: %.3lf s\n", elapsed);
					}
					if(enable_detect){
						start_t = clock();
						detected = Detect(next_frame, d_positions, d_conf, frame_count);
						end_t = clock();
						elapsed = (double)(end_t - start_t)/CLOCKS_PER_SEC;
						//printf("Detector Elapsed: %.3lf s\n", elapsed);
					}

					start_t = clock();
					final_bb = IntegratorLearning(next_frame, tBB, d_positions, d_conf, tracked, detected, new_bb, object, enable_detect);
					end_t = clock();
					elapsed = (double)(end_t - start_t)/CLOCKS_PER_SEC;
					//printf("IL Elapsed: %.3lf s\n", elapsed);
				}

                if(show && !object.empty()) imshow(OBJECT_WINDOW, object);

                show_save(curr_frame);
                bb[0] = new_bb[0];
                bb[1] = new_bb[1];
                bb[2] = new_bb[2];
                bb[3] = new_bb[3];
                fprintfBB(bb_file);
            }
			else{
				if(enable_detect) Train(curr_frame, bb, show);
				train = false;
				show_save(next_frame);
			}

            next_frame.copyTo(curr_frame);
        }
        else{
			if(reset_key) nextFrame(&curr_frame, repeat_video, true);
            curr_frame.copyTo(view_frame);
            show_save(view_frame);
        }

        if(keyboardCallBack()) break;
    }
	captureClose();
	if(!isnan(init_bb[0])) fclose(bb_file);
}
