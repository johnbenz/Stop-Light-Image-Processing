#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#define H_MIN1 0
#define H_MAX1 40
#define H_MIN2 350
#define H_MAX2 360
#define S_THRESHOLD .13
#define I_THRESHOLD 100
#define PI 3.14159265
#define BASE_SIZE 500

uchar Intensity(uchar r, uchar g, uchar b)
{
	return r + g + b;
}

unsigned int Hue(int r, int g, int b)
{
	
	int theta = (180.0/PI)*acos((.5*(r - g + r - b))/(pow(pow(r - g, 2) + (r - b)*(g - b), .5)));
	if (b <= g) return theta;
	return 360 - theta;
}

unsigned int colorMin(uchar r, uchar g, uchar b)
{
	uchar min = r;
	if (g < min) min = g;
	if (b < min) min = b;
	return min;
}

float Saturation(int r, int g, int b)
{
	return (1.0 - (float)(3.0/(float)(r + g + b))*(float)colorMin(r, g, b));
}

void ColorScreening(IplImage *src, IplImage *dst)
{
	for (int i = 0; i < src->height; i++) {
		uchar* srcp = (uchar*)(src->imageData + i*src->widthStep);
		uchar* dstp = (uchar*)(dst->imageData + i*src->widthStep);
		for (int j = 0; j < src->width; j++) {
			unsigned int b = srcp[3*j];
			unsigned int g = srcp[3*j+1];
			unsigned int r = srcp[3*j+2];
			
			unsigned int hue = Hue(r, g, b);
			float sat = Saturation(r, g, b);
			unsigned int ins = Intensity(r, g, b);
			//printf("src: %d %d %d\n", r, g, b);
			//printf("cal: %d %f %d\n", hue, sat, ins);
			if (((hue >= H_MIN1 && hue <= H_MAX1) || (hue >= H_MIN2 && hue <= H_MAX2)) && sat >= S_THRESHOLD && ins >= I_THRESHOLD){
				dstp[3*j] = 0;
				dstp[3*j+1] = 0;
				dstp[3*j+2] = r;
				//printf("sto: %d %d %d\n", dstp[3*j], dstp[3*j+1], dstp[3*j+2]);
			} else {
				dstp[3*j] = 0;
				dstp[3*j+1] = 0;
				dstp[3*j+2] = 0;
				//printf("sto: %d %d %d\n", dstp[3*j], dstp[3*j+1], dstp[3*j+2]);
			}
		}
	}
}

void blobUnion(int *labels)
{
	
}

void LabelRegions(IplImage* src, IplImage* dst)
{
	int *labels = malloc(sizeof(int *)*BASE_SIZE);
	uchar count = 1;
	for (int i = 1; i < src->height; i++) {
		uchar* sp = (uchar*)(src->imageData + i*src->widthStep);
		uchar* dp = (uchar*)(dst->imageData + i*src->widthStep);
		uchar* dp_a = (uchar*)(dst->imageData + (i-1)*src->widthStep);
		for (int j = 1; j < src->width; j++) {
			unsigned int r = sp[3*j+2];
			if (r > 0) {
				dp[3*j+2] = dp[3*(j-1)+2];
				if (dp_a[3*j+2] != 0) {
					if (dp[3*j+2] == 0) {
						dp[3*j+2] = dp[3*(j-1)+2];
					} else {
						//UNION
					}
				} else {
					if (dp[3*j+2] == 0) {
						dp[3*j+2] = count;
						count++;
					}
				}
				
			} else {
				//dp[3*j] = 0;
				//dp[3*j+1] = 0;
				//dp[3*j+2] = 0;
			}
		}
	}
}

int main (int argc, const char * argv[]) {
	IplImage* redLight = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
	IplImage* greenLight = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);
	CvSize sz = cvGetSize(redLight);
	IplImage* redScreened = cvCreateImage(sz, redLight->depth, redLight->nChannels);
	IplImage* redErode = cvCreateImage(sz, redLight->depth, redLight->nChannels);
	IplImage* redDilate = cvCreateImage(sz, redLight->depth, redLight->nChannels);
	
	ColorScreening(redLight, redScreened);
	cvDilate(redScreened, redDilate, cvCreateStructuringElementEx(13, 13, 6, 6, CV_SHAPE_RECT, NULL), 1);
	cvErode(redDilate, redErode, cvCreateStructuringElementEx(13, 13, 6, 6, CV_SHAPE_RECT, NULL), 1);
	
//	cvDilate(redScreened, redDilate, NULL, 1);
//	cvErode(redDilate, redErode, NULL, 1);
	
	cvNamedWindow("Light Detector",CV_WINDOW_AUTOSIZE); 
	cvShowImage("Light Detector", redLight);
	cvWaitKey(0);
	cvShowImage("Light Detector",redScreened); 
	cvWaitKey(0); 
	cvShowImage("Light Detector", redDilate);
	cvWaitKey(0);
	cvShowImage("Light Detector", redErode);
	cvWaitKey(0);
	cvShowImage("Light Detector", greenLight);
	cvWaitKey(0);
	
	ColorScreening(greenLight, redScreened);
	cvDilate(redScreened, redDilate, cvCreateStructuringElementEx(13, 13, 6, 6, CV_SHAPE_RECT, NULL), 1);
	cvErode(redDilate, redErode, cvCreateStructuringElementEx(13, 13, 6, 6, CV_SHAPE_RECT, NULL), 1);
	cvShowImage("Light Detector", redErode);

	cvWaitKey(0);
	
	cvReleaseImage(&greenLight);
	cvReleaseImage(&redLight);
	cvReleaseImage(&redScreened);
	cvReleaseImage(&redErode);
	cvReleaseImage(&redDilate);
	cvDestroyWindow("Light Detector");
}
