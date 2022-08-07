//
// Created by ahmet on 28.06.2021.
//

#include "tracker.h"

tracker::tracker() {
    area_coeff = 0.2;
    dist_coeff = 0.01;
    orientation_coeff = 0.7;
    keypoint_coeff = 0.25;
}

tracker::tracker(double distance_c, double area_c, double orientation_c, double keypoint_c) {
    area_coeff = area_c;
    dist_coeff = distance_c;
    orientation_coeff = orientation_c;
    keypoint_coeff = keypoint_c;
}

double tracker::area_check(segment &s1, segment &s2) { //use distance of ratio of the areas to 1.
    double val;

    if (s1.area > s2.area) {
        val = -float(s1.area) / float(s2.area) - 1;
    } else {
        val = -float(s2.area) / float(s1.area) - 1;

    }
    return val * area_coeff;
}

double tracker::dist_check(segment &s1, segment &s2) { //use euclidean distance between center pixels of the segments
    double val;

    val = - std::sqrt(pow(s1.center.x - s2.center.x, 2) + pow(s1.center.y - s2.center.y, 2) );

    return val * dist_coeff;
}

double tracker::orientation_check(segment &s1, segment &s2) {
    double val;

    double angle1 = (1 / 2) * atan(2 * (s1.m.m11 / s1.m.m00 - s1.center.x * s1.center.y) /
                                ((s1.m.m20 / s1.m.m00 - s1.center.x * s1.center.x) -
                                 (s1.m.m02 / s1.m.m00 - s1.center.y * s1.center.y)));

    double angle2 = (1 / 2) * atan(2 * (s2.m.m11 / s2.m.m00 - s2.center.x * s2.center.y) /
                                   ((s2.m.m20 / s2.m.m00 - s2.center.x * s2.center.x) -
                                    (s2.m.m02 / s2.m.m00 - s2.center.y * s2.center.y)));

    val = - abs(angle2 - angle1);

    return val*orientation_coeff;

}

double tracker::keypoint_check(segment &s1, segment &s2) {
    double val;

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");

    std::vector<std::vector <cv::DMatch>> knn_matches;

    matcher->knnMatch(s1.descriptors, s2.descriptors, knn_matches, 2);


    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> good_matches;

    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }


    val = float(good_matches.size())/float(s1.keypoints.size()) -1 + float(good_matches.size())/float(s2.keypoints.size()) -1;
//    std::cout <<"good "<<val<<" "<<good_matches.size()<<" "<<s1.keypoints.size()<<"  "<<s2.keypoints.size()<<std::endl;

    return val * keypoint_coeff;

}

double tracker::match(segment &s1, segment &s2) {
    double val;

//    std::cout<<"match or "<<orientation_check(s1, s2)<<"  "<<dist_check(s1, s2)<<"  "<<area_check(s1 , s2)<<::std::endl;
//    std::cout<< s1.center.x <<"  "<< s1.center.y <<"  " <<s2.center.x <<"  "<< s2.center.y <<std::endl;
    val = orientation_check(s1, s2) + dist_check(s1, s2) + area_check(s1 , s2) +keypoint_check(s1, s2);

   return val;
}