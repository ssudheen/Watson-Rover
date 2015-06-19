// autocrop.cpp:
// Perform auto-cropping image with arbitrary background color

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

#define DEBUG 0

/**
 * Function to check if the color of the given image
 * is the same as the given color
 *
 * Parameters:
 *   edge        The source image
 *   color   The color to check
 */

int THRES = 0;

bool is_border(cv::Mat& edge, cv::Vec3b color)
{

    cv::Mat im = edge.clone().reshape(0,1);
    cv::Vec3b colorP;

    bool res = true;
    for (int i = 0; i < im.cols; ++i) {
		colorP = im.at<cv::Vec3b>(0,i);
		if (DEBUG) printf("Current point: B=%d, G=%d, R=%d\n", colorP[0], colorP[1], colorP[2]);
		if ((colorP[0] > (color[0]+THRES)) || (colorP[0] < (color[0]-THRES))) res = false;
		else if ((colorP[1] > (color[1]+THRES)) || (colorP[1] < (color[1]-THRES))) res = false;
		else if ((colorP[2] > (color[2]+THRES)) || (colorP[2] < (color[2]-THRES))) res = false;
        //res &= (color == im.at<cv::Vec3b>(0,i));
	}
	if (DEBUG) printf("is_border = %d\n",res);
    return res;
}

/**
 * Function to auto-cropping image
 *
 * Parameters:
 *   src   The source image
 *   dst   The destination image
 */
void autocrop(cv::Mat& src, cv::Mat& dst)
{
    cv::Rect win(0, 0, src.cols, src.rows);

    std::vector<cv::Rect> edges;
    edges.push_back(cv::Rect(0, 0, src.cols, 1));
    edges.push_back(cv::Rect(src.cols-2, 0, 1, src.rows));
    edges.push_back(cv::Rect(0, src.rows-2, src.cols, 1));
    edges.push_back(cv::Rect(0, 0, 1, src.rows));

    cv::Mat edge;
    int nborder = 0;
    cv::Vec3b colorTL = src.at<cv::Vec3b>(20,20);
    cv::Vec3b colorTR = src.at<cv::Vec3b>(20,(src.cols-20));
    cv::Vec3b colorBL = src.at<cv::Vec3b>((src.rows-20),20);
    cv::Vec3b colorBR = src.at<cv::Vec3b>((src.rows-20),(src.cols-20));
    if (DEBUG) {
		printf("TL Ref color: B = %d, G = %d, R = %d\n",colorTL[0],colorTL[1],colorTL[2]);
		printf("TR Ref color: B = %d, G = %d, R = %d\n",colorTR[0],colorTR[1],colorTR[2]);
		printf("BL Ref color: B = %d, G = %d, R = %d\n",colorBL[0],colorBL[1],colorBL[2]);
		printf("BR Ref color: B = %d, G = %d, R = %d\n",colorBR[0],colorBR[1],colorBR[2]);
	}

	nborder = 4;
    bool next;

	if (DEBUG) printf("Entering first do-while\n");
	if (DEBUG) printf("x=%d, y=%d, h=%d, w=%d\n", win.x, win.y, win.height, win.width);
    do {
        edge = src(cv::Rect(win.x, win.height-2, win.width, 1));
        if (next = is_border(edge, colorBR))
            win.height--;
    }
    while (next && win.height > 0);
	if (DEBUG) printf("final height = %d\n", win.height);

	if (DEBUG) printf("Entering second do-while\n");
	if (DEBUG) printf("x=%d, y=%d, h=%d, w=%d\n", win.x, win.y, win.height, win.width);
    do {
        edge = src(cv::Rect(win.width-2, win.y, 1, win.height));
        if (next = is_border(edge, colorBL))
            win.width--;
    }
    while (next && win.width > 0);
	if (DEBUG) printf("final width = %d\n", win.width);

	if (DEBUG) printf("Entering third do-while\n");
	if (DEBUG) printf("x=%d, y=%d, h=%d, w=%d\n", win.x, win.y, win.height, win.width);
    do {
        edge = src(cv::Rect(win.x, win.y, win.width, 1));
        if (next = is_border(edge, colorTR))
            win.y++, win.height--;
    }
    while (next && win.y <= src.rows);
    if (DEBUG) printf("final top y = %d\n", win.y);

	if (DEBUG) printf("Entering fourth do-while\n");
	if (DEBUG) printf("x=%d, y=%d, h=%d, w=%d\n", win.x, win.y, win.height, win.width);
    do {
        edge = src(cv::Rect(win.x, win.y, 1, win.height));
        if (next = is_border(edge, colorTL))
            win.x++, win.width--;
    }
    while (next && win.x <= src.cols);
	if (DEBUG) printf("final x = %d\n", win.x);

    dst = src(win);
}

/**
 * Test the autocrop() function above
 */
int main(int argc, char *argv[])
{
	char *inFile;
	int threshold = 0;

	inFile = argv[1];
	threshold = atoi(argv[2]);
	THRES = threshold;

    cv::Mat src = cv::imread(inFile);
    if (!src.data)
        return -1;

    cv::Mat dst;
    autocrop(src, dst);

    //cv::imshow("src", src);
    //cv::imshow("dst", dst);
    //cv::waitKey();
    // prepare O/P file name
    char *opFile = (char *) malloc((sizeof(inFile)+10));
    strcpy(opFile, "OP_");
    strcat(opFile, inFile);
    cv::imwrite(opFile, dst);
    return 0;
}
