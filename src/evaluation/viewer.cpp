/**
	Visualizador de videos/seq de imagens com as bounding boxes do avaliador (viewerEvaluation)
**/
#include "viewer.hpp"

#ifdef _WIN32
    #define CLEAR() system("cls");
#else
    #define CLEAR() system("clear");
#endif // _WIN32

#define WINDOW "Result"

using namespace cv;
using namespace std;

static int                 	delay = 30,                          //Delay para waitKey() do opencv
                            image_list_size = 0,                //Número de frames no arquivo do IMAGE_LIST
                            image_list_pointer = 0,             //Número de frames mostrados do IMAGE_LIST
                            name_count = 0;                     //Contador de frames salvos
static bool                 pause_cap = false,     	            //pausar captura
                            reset = false,                      //Captura foi reiniciada?
                            reset_key = false,                  //tecla reset foi pressionada?
                            save = false;                       //Salvar frame? : imwrite
static char                 **image_list = NULL,                //Lista de imagens do IMAGE_LIST
                            name[30];                           //Nome do frame: imwrite
static VideoCapture         cap;                                //Captura da WEBCAM ou VIDEO
static VIDEO_TYPE           video_type;                         //Tipo de captura

bool captureOpen(VIDEO_TYPE type, char *path){
	/**
		Inicializa captura: retorna true se inicializou com sucesso
		path = caminho do vídeo ou da lista
		lista no formato: n \n imagem_1 \n ... imagem_n
	**/
	video_type = type;
	if(type == IMAGE_LIST){ //Armazena lista de imagens (caminhos)
		int list_size;
		FILE *list_file = fopen(path, "r");
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
        cap.open(path);
        if(!cap.isOpened()) {
            cout << "Could not initialize capturing...\n";
            return false;
        }
	}
	return true;
}

void captureClose(){
	/**
		Libera estruturas do visualizador
		Comum ao viewer e viewerEvaluation
	**/
    if(image_list){
        for(int i = 0; i < image_list_size; i++) free(image_list[i]);
        free(image_list);
    }
    cap.release();
}

void nextFrame(Mat *frame, bool repeat, bool grayscale){
	/**
		Pega próximo frame em preto e branco ou colorido e reinicia a captura se chegou ao fim
		Volta com bb inicial se houver. Caso contrário, limpa bb
		Comum ao viewer e viewerEvaluation
	**/
	reset = false;
	frame->release();

	if(video_type == IMAGE_LIST){
        //Reinicia se chegou ao fim ou se a tecla foi pressionada
		if(image_list_pointer >= image_list_size || reset_key){
            if(repeat){
                image_list_pointer = 0;
                reset = true;
                reset_key = false;
            }
            else if(!reset_key) return;
		}
		if(grayscale) (*frame) = imread(image_list[image_list_pointer++], CV_LOAD_IMAGE_GRAYSCALE);
		else (*frame) = imread(image_list[image_list_pointer++], CV_LOAD_IMAGE_COLOR);
	}
	else{
        cap >> (*frame);
        //Reinicia se chegou ao fim ou se a tecla foi pressionada
        if(frame->empty() || reset_key){
            if(repeat){
                cap.set(CV_CAP_PROP_POS_FRAMES, 0);
                cap >> (*frame);
                reset = true;
                reset_key = false;
            }
            else if(!reset_key) return;
        }
        if(grayscale) cvtColor(*frame, *frame, COLOR_BGR2GRAY);
	}
}

bool keyboardCallBackEval(){
	/**
		Retorna true se foi finalizado pelo usuário
		viewerEvaluation
	**/
    char key = waitKey(delay);
    bool kill = false;

    switch(key){
		case 'p': //Pausar/retomar vídeo
			if(pause_cap){
				cout << "Resume" << endl;
				pause_cap = false;
			}
			else{
				cout << "Pause" << endl;
				pause_cap = true;
			}
        break;
		case 'r': //Reiniciar
            cout << "Restarting" << endl;
            reset_key = true;
            //if(video_type == VIDEO) cap.set(CV_CAP_PROP_POS_FRAMES, 0);
			//else image_list_pointer = 0;
        break;
        case '-': //Desacelerar
			cout << "Slow down. Delay " << delay << endl;
			if(delay < 300) delay+=10;
        break;
        case '+': //Acelerar
			cout << "Speed up. Delay " << delay << endl;
			if(delay > 10) delay-=10;
        break;
		case 'q': //Finalizar
        case 27:
            cout << "q or Esc key was pressed by user" << endl;
            kill = true;
        break;
        case 's':
            sprintf(name, "shots/%d.jpg", name_count++);
            save = true;
        break;
    }
    return kill;
}

void helpEval(){
	/** viewerEvaluation **/
    cout << "-------------------------------------------------------------" << endl;
    cout << "\tPress 'p' to pause/resume." << endl;
    cout << "\tPress 'r' to restart." << endl;
    cout << "\tPress '-' to slow down." << endl;
    cout << "\tPress '+' to speed up." << endl;
    cout << "\tPress 's' to save frame." << endl;
    cout << "\tPress ESC or 'q' to exit." << endl;
    cout << "-------------------------------------------------------------\n" << endl;
}

void viewerEvaluation(VIDEO_TYPE type, bool repeat, char *video_path, BoundingBox *gt_bb_vector, BoundingBox *method_bb_vector, bool *correct_vector, float *overlap_vector, int bb_count){
	Mat frame;
	int pointer = 0;
	Point center;
	char value[10];

   	if(!captureOpen(type, video_path)) return;

	CLEAR();
	helpEval();
	namedWindow(WINDOW, CV_WINDOW_FREERATIO);

    while(1){
        if(!pause_cap || reset_key){
            nextFrame(&frame, repeat, false);
            if(reset) pointer = 0; //Se resetou dentro do nextFrame...

            if(frame.empty()){
                if(repeat) cout << "Could not restart video..." << endl;
				break;
			}

			if(pointer >= bb_count){
				pointer = 0;
				if(pointer >= bb_count) break;
			}

            rectangle(frame, Point2d(gt_bb_vector[pointer][0], gt_bb_vector[pointer][1]), Point2d(gt_bb_vector[pointer][2], gt_bb_vector[pointer][3]), Scalar(0.0, 255.0, 0.0), 2.);
            if(correct_vector[pointer])
				rectangle(frame, Point2d(method_bb_vector[pointer][0], method_bb_vector[pointer][1]), Point2d(method_bb_vector[pointer][2], method_bb_vector[pointer][3]), Scalar(255., 0., 0.), 2.);
			else
				rectangle(frame, Point2d(method_bb_vector[pointer][0], method_bb_vector[pointer][1]), Point2d(method_bb_vector[pointer][2], method_bb_vector[pointer][3]), Scalar(0., 0., 255.), 2.);

            center.x = (method_bb_vector[pointer][2] + method_bb_vector[pointer][0])/2.;
            center.y = (method_bb_vector[pointer][3] + method_bb_vector[pointer][1])/2.;
            sprintf(value, "%.2f", overlap_vector[pointer]);
            putText(frame, value, center, FONT_HERSHEY_COMPLEX_SMALL, 0.7, Scalar(0.,0., 255.));
			pointer++;
            imshow(WINDOW, frame);
            if(save){
                imwrite(name, frame);
                printf("%s saved.\n", name);
                save = false;
            }
        }
        else if(save){
            imwrite(name, frame);
            printf("%s saved.\n", name);
            save = false;
        }

        if(keyboardCallBackEval()) break;
    }

    frame.release();
    captureClose();
}
