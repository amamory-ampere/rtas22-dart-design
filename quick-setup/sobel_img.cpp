// sobel_img.cpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char** argv ){

    cv::Mat img, gray_img;
    int w, h; // width, height

    // read image
    img = cv::imread("./lena.jpg");
    //cv::imshow("Lego color", img);

    // width and height of image
    w = img.size().width;
    h = img.size().height;
    cout << "image size: " << h << " " << w << endl;


    cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
    //cv::imshow("Lego ", gray_img);

    cv::Mat gX, gY;
    //  compute gradients along the X and Y axis, respectively
    cv::Sobel(gray_img, gX, CV_64F, 1, 0);
    cv::Sobel(gray_img, gY, CV_64F, 0, 1);
    // gX value after sobel conversion -52.0
    cout << "gX value after sobel conversion: " << (int)gX.at<double>(100, 200) << endl;


    // gX and gY are decimal number with +/- values
    // change these values to +ve integer format
    cv::convertScaleAbs(gX, gX);
    // gX value after Absolute scaling 52
    cv::convertScaleAbs(gY, gY);
    cout << "gX value after Absolute scaling: " << (int)gX.at<uchar>(100, 200) << endl;


    cv::Mat sobelCombined;
     cv::addWeighted(gX, 0.5, gY, 0.5, 0, sobelCombined);

    // show the output images
    //cv::imshow("Sobel X", gX);
    //cv::imshow("Sobel Y", gY);
    //cv::imshow("Sobel Combined", sobelCombined);
    cv::imwrite("sobel.jpg", sobelCombined);

    cv::waitKey(0);
    return 0;
}
