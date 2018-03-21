#include <iostream>
#include <opencv2/opencv.hpp>

#include "TLD.hpp"

using namespace std;

int main(int argc, char* argv[]){
	if(argc != 2) {
		cout << "Usage: <parameters_path>" << endl;
		return EXIT_FAILURE;
	}

	TLD(argv[1]);

	return EXIT_SUCCESS;
}
