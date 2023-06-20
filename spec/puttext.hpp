#ifndef PUTTEXT_HPP_INCLUDED
#define PUTTEXT_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

struct sizeWord{
	int width;
	int maxHeigh;	
};

int readAscii(int , int);

sizeWord getSizeWord(const string&, const vector<vector<vector<int>>>& , int);

void writeText(Mat& , const string&, const string&,const vector<vector<vector<int>>>& , int);

vector<vector<vector<int>>> loadMatrix(const string&);


#endif // PUTTEXT_HPP_INCLUDED
