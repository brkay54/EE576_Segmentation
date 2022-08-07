#include "include/common.h"



int main() {
// Question 1
    std::vector <std::vector <cv::Mat>> frames;
    common::read_frames(frames);
    common::segmentation(frames);
    return 0;
}
