#ifndef VIEWER_HPP_INCLUDED
#define VIEWER_HPP_INCLUDED

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../common/utils.hpp"

/**
*	Visualizador do resultado do rastreador
*	@in type: Tipo de captura: video ou lista de imagens
*	@in repeat: Repetir vídeo. Será automaticamente falso se show for falso.
*	@in video_path: Caminho para o vídeo ou lista de imagens. Formato lista: N\n caminhoVideo1\n ... caminhoVideoN. NULL para webcam
*	@in gt_bb_vector: Vetor de bouding boxes do ground truth
*	@in method_bb_vector: Vetor de bouding boxes do método
*	@in correct_vector: vetor indicando quais bb estão corretas
*	@in overlap_vector: vetor de overlap
*	@in bb_count: número de tamanho dos 3 vetores anteriores.
**/
void viewerEvaluation(VIDEO_TYPE type, bool repeat, char *video_path, BoundingBox *gt_bb_vector,
        BoundingBox *method_bb_vector, bool *correct_vector, float *overlap_vector, int bb_count);
#endif // VIEWER_HPP_INCLUDED
