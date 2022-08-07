//
// Created by ahmet on 28.06.2021.
//

#ifndef INC_576FINAL_TRACKER_H
#define INC_576FINAL_TRACKER_H

#include "segment.h"

class tracker {
public:

    double dist_coeff, area_coeff, orientation_coeff, keypoint_coeff;


    tracker();
    tracker(double distance_c, double area_c, double orientation_c, double keypoint_c);

    double dist_check(segment &s1, segment &s2);
    double area_check(segment &s1, segment &s2);
    double orientation_check(segment &s1, segment &s2);
    double keypoint_check(segment &s1, segment &s2);
    double match(segment &s1, segment &s2);


};


#endif //INC_576FINAL_TRACKER_H
