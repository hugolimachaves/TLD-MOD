#ifndef EVAL_HPP_INCLUDED
#define EVAL_HPP_INCLUDED

#include <stdio.h>
#include <iostream>
#include "viewer.hpp"
#include "../common/utils.hpp"


/**
*   Compara bb do ground truth com as do método
*	@in type: Tipo de captura: webcam, video ou lista de imagens
*	@in video_path: Caminho para o vídeo ou lista de imagens. Formato lista: N\n caminhoVideo1\n ... caminhoVideoN.
*	@in gt_path: Arquivo com as bouding boxes do ground truth
*	@in method_path: Arquivo com as bouding boxes do método
*	@in report_path: Arquivo para geração de relatório
*	@in show: Visualizar rastreamento.
*	@in repeat: Repetir vídeo. Será automaticamente falso se show for falso.
*	@in until_fail: Se verdadeiro, considera desempenho até o primeiro erro.
*
**/
void eval(VIDEO_TYPE type, char* video_path, char *gt_path, char *method_path, bool show, bool repeat, bool until_fail, char *report_path);



#endif // EVAL_HPP_INCLUDED
