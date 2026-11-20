#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// 并查集：查找根标签，并进行路径压缩。
int findRoot(std::vector<int>& parent, int label) {
    if (parent[label] != label) parent[label] = findRoot(parent, parent[label]);
    return parent[label];
}

// 8 邻域两遍扫描。输入为 CV_8UC1，输出为 CV_32SC1，返回前景连通域数量。
int labelTwoPass(const cv::Mat& binary, cv::Mat& labels) {
    CV_Assert(binary.empty() || (binary.dims == 2 && binary.type() == CV_8UC1));
    labels = cv::Mat::zeros(binary.size(), CV_32SC1);
    std::vector<int> parent{0};  // 标签 0 留给背景。

    // 第一遍：分配临时标签，合并等价标签。
    for (int y = 0; y < binary.rows; ++y) {
        const unsigned char* src = binary.ptr<unsigned char>(y);
        int* row = labels.ptr<int>(y);
        const int* above = y > 0 ? labels.ptr<int>(y - 1) : nullptr;
        for (int x = 0; x < binary.cols; ++x) {
            if (src[x] == 0) continue;
            const int neighbors[4] = {
                x > 0 ? row[x - 1] : 0,
                above ? above[x] : 0,
                above && x > 0 ? above[x - 1] : 0,
                above && x + 1 < binary.cols ? above[x + 1] : 0
            };
            int label = 0;
            for (int neighbor : neighbors) {
                if (neighbor > 0 && (label == 0 || neighbor < label)) label = neighbor;
            }
            if (label == 0) {
                label = static_cast<int>(parent.size());
                parent.push_back(label);
            } else {
                for (int neighbor : neighbors) {
                    if (neighbor == 0) continue;
                    const int a = findRoot(parent, label);
                    const int b = findRoot(parent, neighbor);
                    parent[std::max(a, b)] = std::min(a, b);
                }
            }
            row[x] = label;
        }
    }

    // 第二遍：统一等价标签，连续编号为 1、2、3……
    int count = 0;
    std::vector<int> finalLabel(parent.size(), 0);
    for (int y = 0; y < labels.rows; ++y) {
        int* row = labels.ptr<int>(y);
        for (int x = 0; x < labels.cols; ++x) {
            if (row[x] == 0) continue;
            const int root = findRoot(parent, row[x]);
            if (finalLabel[root] == 0) finalLabel[root] = ++count;
            row[x] = finalLabel[root];
        }
    }
    return count;
}

int main() {
    const std::string folder = "/Users/mac/OpenCV_coding/OpenCV_test03/";
    const cv::Mat gray = cv::imread(folder + "2182.png", cv::IMREAD_GRAYSCALE);
    if (gray.empty()) {
        std::cerr << "无法读取 2182.png\n";
        return 1;
    }

    // 平滑纹理，再反向二值化，使较暗的小孔成为白色前景。
    cv::Mat smooth, binary, labels;
    cv::GaussianBlur(gray, smooth, cv::Size(5, 5), 0);
    cv::threshold(smooth, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    const int components = labelTwoPass(binary, labels);

    // 统计各连通域的面积和外接矩形。
    std::vector<int> areas(components + 1, 0);
    std::vector<cv::Rect> boxes(components + 1);
    for (int y = 0; y < labels.rows; ++y) {
        const int* row = labels.ptr<int>(y);
        for (int x = 0; x < labels.cols; ++x) {
            const int label = row[x];
            if (label == 0) continue;
            ++areas[label];
            boxes[label] |= cv::Rect(x, y, 1, 1);
        }
    }

    cv::Mat result;
    cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
    int count = 0;
    const int minArea = 500, maxArea = 10000;  // 排除文字噪点及大孔、大片阴影。
    for (int label = 1; label <= components; ++label) {
        const cv::Rect& box = boxes[label];
        if (areas[label] < minArea || areas[label] > maxArea) continue;
        const double ratio = static_cast<double>(box.width) / box.height;
        const double fill = static_cast<double>(areas[label]) / box.area();
        const bool touchesEdge = box.x == 0 || box.y == 0 ||
            box.x + box.width == gray.cols || box.y + box.height == gray.rows;
        // 圆孔的外接矩形接近正方形，面积约占矩形的 pi/4。
        if (touchesEdge || ratio < 0.8 || ratio > 1.25 || fill < 0.65) continue;
        ++count;
        const cv::Point center(box.x + box.width / 2, box.y + box.height / 2);
        cv::circle(result, center, (box.width + box.height) / 4, cv::Scalar(0, 180, 0), 2);
        cv::putText(result, std::to_string(count),
                    cv::Point(box.x, std::max(20, box.y - 6)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.65, cv::Scalar(0, 0, 255), 2);
    }
    cv::putText(result, "Count: " + std::to_string(count), cv::Point(15, 28),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    const std::string output = folder + "2182_counted.png";
    if (!cv::imwrite(output, result)) {
        std::cerr << "无法保存结果图\n";
        return 1;
    }
    std::cout << "小孔数量：" << count << '\n'
              << "结果图：" << output << '\n';
    return 0;
}
