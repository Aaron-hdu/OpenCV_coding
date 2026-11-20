# 机器视觉技术及应用 · OpenCV 实验作业

本仓库收录使用 C++17 和 OpenCV 完成的课程练习，包括图像读写、像素操作，以及用连通域标记识别和统计图像中的目标。

## 实验目录

| 源码 | 实验内容 | 输入与结果 |
| --- | --- | --- |
| [OpenCV_demo](OpenCV_demo/main.cpp)、[demo2](demo2/main.cpp) | 输出 OpenCV 版本，在窗口中显示 `Hello, OpenCV!` | 程序生成示例图像 |
| [OpenCV_hw01](OpenCV_hw01/main.cpp) | 逐像素遍历彩色图像，将 B、G、R 三个通道的值分别减半 | [原图](OpenCV_hw01/gqwy.jpeg)，处理结果在窗口中显示 |
| [OpenCV_test01](OpenCV_test01/main.cpp) | 识别暗色回形针，按面积和边界位置过滤噪点与黑边 | [输入图](OpenCV_test01/2181.png) · [结果图](OpenCV_test01/2181_counted.png) · **13 个回形针** |
| [OpenCV_test02](OpenCV_test02/main.cpp) | 高斯平滑后提取亮色硬币，按面积过滤细小亮斑 | [输入图](OpenCV_test02/2183.png) · [结果图](OpenCV_test02/2183_counted.png) · **10 枚硬币** |
| [OpenCV_test03](OpenCV_test03/main.cpp) | 提取暗色区域，通过面积、宽高比、填充率和边界位置筛选小圆孔 | [输入图](OpenCV_test03/2182.png) · [结果图](OpenCV_test03/2182_counted.png) · **4 个小孔** |

## 连通域标记

三个识别实验先读取灰度图，再用 Otsu 法二值化。硬币比背景亮，使用普通二值化。回形针和小孔比背景暗，使用反向二值化。

标记采用手写的 **8 邻域两遍扫描法**。第一遍逐行扫描前景像素，检查已经扫描过的左、左上、上、右上四个位置，分配临时标签并用并查集记录等价关系。第二遍将等价标签统一并重新编号。随后按实验需要筛选连通域，统计数量，并把编号和检测标记保存到对应的 `*_counted.png` 中。

## 编译与运行

实验使用 macOS、Clang、C++17 和 Homebrew 安装的 OpenCV 5。每个目录中的 `main.cpp` 都是独立程序，请进入对应目录编译。以硬币识别为例，从仓库根目录运行：

```bash
cd OpenCV_test02
clang++ -std=c++17 main.cpp -o opencv_demo.out $(pkg-config --cflags --libs opencv5)
./opencv_demo.out
```

如果将仓库克隆到其他路径，运行读图实验前，请将 `main.cpp` 中的 `folder` 改为对应实验目录的实际路径（末尾保留 `/`）。`OpenCV_hw01` 则需要修改 `imread` 中的图片路径。若系统的 `pkg-config` 包名为 `opencv4`，编译命令中的 `opencv5` 可相应替换。

在配置了相同 Homebrew 路径的 VS Code 中，也可以打开要运行的 `main.cpp`，选择 **编译并运行当前 OpenCV 实验** 任务。
