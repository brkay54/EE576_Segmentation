//
// Created by ahmet on 28.06.2021.
//

#ifndef INC_576FINAL_SEGMENT_H
#define INC_576FINAL_SEGMENT_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/core/types.hpp>

struct point {
    int x,y;
};

class segment {
public:
    point center;
    point initial_pose, previous_pose;
    double area;
    int number_of_occurance;
    int last_seen;
    bool current_visibility;
    cv::Moments m;
    std::vector<cv::Point> contour;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;



    segment();
    segment(std::vector<cv::Point> c, std::vector<cv::KeyPoint> &keyp, cv::Mat &gray_image);


    void find_area();
    void find_moments();
    void find_keypoints_in_segment(std::vector<cv::KeyPoint> &keyp, cv::Mat &gray_image);




};


/*
vector<vector<Point> > contours;
    findContours( canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );
 */

#endif //INC_576FINAL_SEGMENT_H
