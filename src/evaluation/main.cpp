#include <sstream>
#include <iostream>

#include "eval.hpp"
#include "viewer.hpp"

/**
*	Usar: executável    <tipoVideo> <caminhoEntrada> <caminhoBB>
*                       <caminhoGT> <mostrar?> <repetir?>
*
*   tipoVideo = 1 para video, 2 para lista de imagens
*	caminhoBB = caminho das bounding boxes do método
*	caminhoEntrada = caminho do vídeo ou da lista de imagens.
*	caminhoGT = caminho para o ground truth.
*   mostrar? = mostrar vídeo. 0 para falso e 1 para verdadeiro.
*   repetir? = repetir vídeo. 0 para falso e 1 para verdadeiro.
*
**/

using namespace std;

int main(int argc, char* argv[]){
    VIDEO_TYPE  video_type;
    char        *bb_path, *input_path, *gt_path, *report_path;
    bool        show, repeat;

	if(argc != 8){
		cout << "Usage: \n\t  <video type: 1 = video / 2 = image list>\
		\n\t<inputPath>\
		\n\t<bbPath> \
		\n\t<gtPath> \
		\n\t<reportPath> \
		\n\t<show:  0 = false / 1 = true>\
		\n\t<repeat:  0 = false / 1 = true>" << endl;
		return EXIT_FAILURE;
	}

	video_type 	= (VIDEO_TYPE) atoi(argv[1]);
	input_path 	= argv[2];
	bb_path 	= argv[3];
	gt_path 	= argv[4];
	report_path	= argv[5];
	show 		= atoi(argv[6]);
	repeat 		= atoi(argv[7]);

    eval(video_type, input_path, gt_path, bb_path, show, repeat, true, report_path);

    return EXIT_SUCCESS;
}
