#include<RealSense\SampleReader.h>
#include<RealSense\SenseManager.h>
#include<iostream>
#include<opencv2\opencv.hpp>
#include"RealSenseAPI.h"
#include<vector>

using namespace cv;
using std::vector;

int main()
{
	Mat irImage16U(480, 640, CV_16UC1, Scalar(0)); // IR受け取り用、グレースケール、1ch
	Mat irImage8U(480, 640, CV_8UC1, Scalar(0)); // 8bit変換用、グレースケール、1ch
	Mat irImage8UC3(480, 640, CV_8UC3, Scalar(25, 50, 200)); // グレースケール、3ch、表示用
	Mat binImage(480, 640, CV_8UC1, Scalar(0)); // 2値、輪郭抽出用

	Mat colorImage8U(480, 640, CV_8UC3, Scalar(0, 0, 0)); // Color受け取り用、カラー、3ch、表示用
	Mat colorImage8UG(480, 640, CV_8UC3, Scalar(25, 50, 200)); // グレースケール、3ch、特徴抽出用

	VideoWriter writer("input.avi", CV_FOURCC_DEFAULT, 30, cv::Size(640,480), true); // 動画出力用（仮）
	
	RealSenseAPI realSense;
	realSense.initialize();

	namedWindow("bin");
	namedWindow("ir", WINDOW_AUTOSIZE);
	namedWindow("color", WINDOW_AUTOSIZE);
	waitKey(1);
	int loopCount = 0;
	int thresh = 100;
	int top = 70; // 70
	int right = 70; // 162
	int bottom = 70; // 108
	int left = 70; // 63

	while (1)
	{
		vector<vector<Point>> contours;
		vector<Rect> leyes, reyes;
		char key = waitKey(1);

		// IR for gaze detection
		realSense.queryImage(irImage16U, ResponseType::IR);
		irImage16U.convertTo(irImage8U, CV_8UC1); // 16bit -> 8bit
		//irImage16U.convertTo(irImage8UC3, CV_8UC3);
		cvtColor(irImage8U, irImage8UC3, CV_GRAY2BGR); // 1ch(Y) -> 3ch(Y, Y, Y)

		irImage8UC3 = irImage8UC3(Rect(left, top, 640 - right, 480 - bottom)); // 70 -> 70px zoom
		resize(irImage8UC3, irImage8UC3, Size(640, 480));

		char str[16];
		sprintf_s(str, "%d-%d-%d-%d", top, right, bottom, left);
		putText(irImage8UC3, str, Point(30, 50), 0, 2, Scalar(255, 0, 0), 2);

		irImage8U = ~irImage8U; // Invert Black and white
		threshold(irImage8U, binImage, thresh, 255, CV_THRESH_BINARY);
		Mat element(3, 3, CV_8UC1); // フィルタサイズ
		erode(binImage, binImage, element); // 収縮(ノイズ除去)、対象ピクセルの近傍のうち最大
		erode(binImage, binImage, element);
		dilate(binImage, binImage, element); // 膨張（強調）、対象ピクセルの近傍のうち最小
		dilate(binImage, binImage, element);
		dilate(binImage, binImage, element);

		// Color for face/eye detection
		realSense.queryImage(colorImage8U, ResponseType::COLOR);
		cvtColor(colorImage8U, colorImage8UG, CV_BGR2GRAY);

		cv::imshow("color", colorImage8UG);
		cv::imshow("ir", irImage8UC3);
		cv::imshow("bin", binImage);

		std::cout << ".";
		loopCount++;
		if (loopCount >40)
		{
			loopCount = 0;
			std::cout << std::endl;
		}
		if (key == 'q')
		{
			break;
		}

		/*
		if ('s' == key)
		{
			std::cout << "thresh" << std::endl;
			std::cin >> thresh;
		}
		*/


		if ('H' == key) {
			left--;
		}
		if ('h' == key) {
			left++;
		}
		if ('J' == key) {
			right--;
		}
		if ('j' == key) {
			right++;
		}
		if ('U' == key) {
			top--;
		}
		if ('u' == key) {
			top++;
		}
		if ('N' == key) {
			bottom--;
		}
		if ('n' == key) {
			bottom++;
		}

		//writer << irImage8UC3;
		writer << colorImage8UG;
	}

	cv::destroyAllWindows();

	return 0;

}
