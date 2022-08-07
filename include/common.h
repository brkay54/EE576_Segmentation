//
// Created by berkay on 6.04.2021.
//

#ifndef UNTITLED_COMMON_H
#define UNTITLED_COMMON_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <numeric>
#include <iostream>
#include <string>
#include <array>
#include <dirent.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "segment.h"

class common {

public:
    static void read_frames(std::vector <std::vector <cv::Mat>> &frames);
    static cv::Mat combine_frames(std::vector<cv::Mat> &frames);
    static void segmentation(std::vector<std::vector<cv::Mat>> &frames);
    static void find_contours(cv::Mat &segmented_img, std::vector<std::vector<cv::Point>> &contours);
    static void get_segmented(cv::Mat &frame,cv::Mat &segmented);
    static std::vector<cv::KeyPoint> detect_keypoints(cv::Mat &c_frame);



};


#endif //UNTITLED_COMMON_H
