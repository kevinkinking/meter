//
// Created by xukaiwe on 18-3-12.
//

#ifndef OPENCV_SSD_UTOOLS_H
#define OPENCV_SSD_UTOOLS_H

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

void rects_sort(const vector<Rect> rects, vector<int> &sorted_indexs, const int sort_index = 0);

#endif //OPENCV_SSD_UTOOLS_H
