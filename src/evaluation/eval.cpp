#include "eval.hpp"
#include <stdio.h>
#include <stdlib.h>

void eval(VIDEO_TYPE type, char* video_path, char *gt_path, char *method_path, bool show, bool repeat, bool until_fail, char *report_path){
    //validação dos parâmetros
    if(!gt_path || !method_path || !report_path || (type != VIDEO && type != IMAGE_LIST)){
        printf("init\n");
        return;
    }

    FILE *gt = fopen(gt_path, "r");
	if(gt == NULL) {
        printf("gt\n");
        return;
	}

	FILE *method = fopen(method_path, "r");
	if(method == NULL) {
        printf("method\n");
		fclose (gt);
		return;
	}

	//Inicialização
	int number_frames, frame_pointer = 0, correct = 0;
	float _overlap, *overlap_vector;
	bool *correct_vector, reading, found_failure = false;
	BoundingBox *gt_bb, *method_bb;

	fscanf(gt, "%d\n", &number_frames);
    overlap_vector 	= (float*) malloc(sizeof(float)*number_frames);
    correct_vector 	= (bool*) malloc(sizeof(bool)*number_frames);
    gt_bb 			= (BoundingBox*) malloc(sizeof(BoundingBox)*number_frames);
    method_bb 		= (BoundingBox*) malloc(sizeof(BoundingBox)*number_frames);

    //Leitura e avaliação
    while(frame_pointer < number_frames){
		fscanf(gt, "%f,%f,%f,%f\n", &gt_bb[frame_pointer][0], &gt_bb[frame_pointer][1], &gt_bb[frame_pointer][2], &gt_bb[frame_pointer][3]);
		fscanf(method, "%f,%f,%f,%f\n", &method_bb[frame_pointer][0], &method_bb[frame_pointer][1], &method_bb[frame_pointer][2], &method_bb[frame_pointer][3]);

		_overlap = overlap(gt_bb[frame_pointer], method_bb[frame_pointer]);
		overlap_vector[frame_pointer] = _overlap;

        if(_overlap >= OVERLAP_THRESHOLD && !found_failure){
			correct_vector[frame_pointer] = true;
			correct++;
		}
		else{
			if(until_fail) found_failure = true;
			correct_vector[frame_pointer] = false;
		}

        frame_pointer++;
    }

    fclose(gt);
    fclose(method);

    //Outras métricas
    float precision = (float) correct / number_frames;

	//Visualização
    if(show) viewerEvaluation(type, repeat, video_path, gt_bb, method_bb, correct_vector, overlap_vector, number_frames);

    //Reporta
    FILE *report = fopen(report_path, "w");
	if(report == NULL) {
        printf("report\n");
        return;
	}

	fprintf(report, "Overlap\t\tResult\n");
    fprintf(report, "--------------------------\n");
	for(int i = 0; i < number_frames; i++){
        if(correct_vector[i]) fprintf(report, "%f\tSUCCESS\n", overlap_vector[i]);
        else fprintf(report, "%f\tFAILURE\n", overlap_vector[i]);
	}
	fprintf(report, "\n--------------------------\n");
	fprintf(report, "Frames: %d\n", number_frames);
	fprintf(report, "Correct: %d\n", correct);
	fprintf(report, "Precision: %f\n", precision);
	fprintf(report, "--------------------------\n");
	fclose(report);

    free(overlap_vector);
    free(correct_vector);
    free(gt_bb);
    free(method_bb);
}
