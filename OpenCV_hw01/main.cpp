#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
    Mat srcMat = imread("/Users/mac/OpenCV_coding/OpenCV_hw01/gqwy.jpeg");

    int height = srcMat.rows;
    int width = srcMat.cols;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            srcMat.at<Vec3b>(j, i)[0] = srcMat.at<Vec3b>(j, i)[0] / 2;
            srcMat.at<Vec3b>(j, i)[1] = srcMat.at<Vec3b>(j, i)[1] / 2;
            srcMat.at<Vec3b>(j, i)[2] = srcMat.at<Vec3b>(j, i)[2] / 2;
        }
    }

    imshow("test", srcMat);
    waitKey(0);

    return 0;
}