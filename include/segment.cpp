//
// Created by ahmet on 28.06.2021.
//

#include "segment.h"

segment::segment() {}

segment::segment(std::vector<cv::Point> c, std::vector<cv::KeyPoint> &keyp, cv::Mat &gray_image) {
    contour = c;
    find_area();
    find_moments();
    find_keypoints_in_segment(keyp, gray_image);
    number_of_occurance = 0;
}

void segment::find_area() {
    area = cv::contourArea(contour);
}

void segment::find_moments() {
    m = cv::moments(contour);

    center.x = int(m.m10/m.m00);
    center.y = int(m.m01/m.m00);
}

void segment::find_keypoints_in_segment(std::vector<cv::KeyPoint> &keyp, cv::Mat &gray_image) {

    for(int i=0; i<keyp.size(); i++){

        cv::Point2f position =keyp[i].pt;  //position of the keypoint

        if( cv::pointPolygonTest(contour, position, false) ==1 ) { //keypoint is inside the segment contour
        keypoints.push_back(keyp[i]);
        }
    }

    cv::Ptr<cv::DescriptorExtractor> extractor = cv::SiftDescriptorExtractor::create();

    extractor->compute(gray_image, keypoints, descriptors);


}