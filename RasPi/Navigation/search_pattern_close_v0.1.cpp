#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img; Mat templ; Mat templ_rsz;
int match_method;
float dist = 0;

/// Function Headers
void MatchingMethod( int, void* );

/** @function main */
int main( int argc, char** argv )
{
	float distance = 0;
	distance = atof(argv[1]);
	dist = distance; 
	/// Load image and template
	img = imread( "tempImg.jpg", 1 );
	templ = imread( "template_close.jpg", 1 );
	
	if (distance == 3) {
		resize(templ, templ_rsz, Size(), 0.4, 0.4);
	}
	else if (distance == 2.5) {
		resize(templ, templ_rsz, Size(), 0.5, 0.5);
	}
	else if (distance == 2) {
		resize(templ, templ_rsz, Size(), 0.67, 0.67);
	}
	else if (distance == 1.5) {
		resize(templ, templ_rsz, Size(), 0.8, 0.8);
	}
	else if (distance == 1) {
		resize(templ, templ_rsz, Size(), 1.0, 1.0);
	}
	else return(-1);
	
	MatchingMethod( 0, 0 );
	return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
	Mat result;
	FILE *fp;
	char buff[20];
	
	/// Create the result matrix
	int result_cols =  img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	
	result.create( result_rows, result_cols, CV_32FC1 );

	/// Do the Matching and Normalize
	matchTemplate( img, templ_rsz, result, CV_TM_CCOEFF_NORMED);
	normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

	matchLoc = maxLoc;
	
	fp=fopen("image_search_op.txt", "w");
	if (maxVal > 0.7) {
		sprintf(buff, "%d,%d,%1.1f", matchLoc.x, matchLoc.y, dist);
	}
	else {
		sprintf(buff, "-1");
	}
	
	fprintf(fp, buff); 
	fclose(fp);
	
	//printf("x = %d\n", matchLoc.x);
	//printf("y = %d\n", matchLoc.y);
	return;
}
