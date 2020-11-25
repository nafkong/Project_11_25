#include<iostream>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int calcHOG(Mat srcMat,float * hist, int nAngle, int cellSize)
{
	int nX = srcMat.cols / cellSize;
	int nY = srcMat.rows / cellSize;

	Mat gx, gy;
	Mat mag, angle;
	Sobel(srcMat, gx, CV_32F, 1, 0, 1);
	Sobel(srcMat, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);
	for (int i = 0; i < nY; i++) {
		for (int j = 0; j < nX; j++) {
			Mat roiMat, roiMag, roiAgl;
			Rect roi;
			roi.height = 16;
			roi.width = 16;
			roi.x = j * cellSize;
			roi.y = i * cellSize;
			roiMat = srcMat(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);
			int head = (i * nX + j) * nAngle;

			for (int n = 0; n < roiMag.rows; n++) {
				for (int m = 0; m < roiMag.cols; m++) {
					//hist[head + (int)((roiAgl.at<float>(n, m)*360) / 45)]++;
					int pos = (int)(roiAgl.at<float>(n,m) / 45); 
					hist[head + pos] += roiMag.at<float>(n, m);
				}
			}
		}
	}
	return 0;
}
int normL2(float* hist1, float* hist2, int bins)
{
	int distance;
	int result = 0;
	for (int i = 0; i < bins; i++) {
		result = pow((hist1[i]-hist2[i]),2);
		result += result;
	}
	distance = sqrt(result);
	return distance;
}
int main()
{
	int cellSize = 16;
	int nAngle = 8;
	Mat refMat = imread("hogTemplate.jpg", 0);
	Mat plMat = imread("img1.jpg", 0);
	Mat bgMat = imread("img2.jpg", 0);
	int bins = (refMat.cols / cellSize) * (refMat.rows / cellSize) * nAngle;
	float* ref_hist = new float[bins];
	memset(ref_hist, 0, sizeof(float) * bins);
	float* pl_hist = new float[bins];
	memset(pl_hist, 0, sizeof(float) * bins);
	float* bg_hist = new float[bins];
	memset(bg_hist, 1, sizeof(float) * bins);
	int reCode = 0;

	reCode = calcHOG(refMat, ref_hist, nAngle, cellSize);
	reCode = calcHOG(plMat, pl_hist, nAngle, cellSize);
	reCode = calcHOG(bgMat, bg_hist, nAngle, cellSize);
	if (reCode != 0) {
		return -1;
	}
	float dis1 = normL2(ref_hist, pl_hist, bins);
	float dis2 = normL2(ref_hist, bg_hist, bins);
	cout <<"人:"<< dis1 << endl;
	cout <<"墙:"<< dis2 << endl;
	if (dis1 < dis2)
		cout << "imag1为人" << endl;
	else
		cout << "imag2为人" << endl;

	delete[] ref_hist;
	delete[] pl_hist;
	delete[] bg_hist;
}

