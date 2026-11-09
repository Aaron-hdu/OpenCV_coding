#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "OpenCV version: "
              << CV_VERSION << std::endl;

    cv::Mat image(400, 600, CV_8UC3,
                  cv::Scalar(60, 60, 60));

    cv::putText(image, "Hello, OpenCV!",
                cv::Point(100, 210),
                cv::FONT_HERSHEY_SIMPLEX,
                1.2, cv::Scalar(0, 255, 0), 2);

    cv::imshow("OpenCV Test", image);
    cv::waitKey(0);
    return 0;
}