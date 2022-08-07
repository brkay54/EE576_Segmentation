//
// Created by berkay on 6.04.2021.
//

#include "common.h"
#include <random>
#include "image.h"
#include "misc.h"
#include "pnmfile.h"
#include "segment-image.h"
#include "segment.h"
#include "tracker.h"

//for natural sorting https://www.titanwolf.org/Network/q/751eec3e-3828-4e55-bc27-178e50d7a532/y
std::string toUpper(std::string s) {
    for (int i = 0; i < (int) s.length(); i++) { s[i] = toupper(s[i]); }
    return s;
}

bool compareNat(const std::string &a, const std::string &b) {
    if (a.empty())
        return true;
    if (b.empty())
        return false;
    if (std::isdigit(a[0]) && !std::isdigit(b[0]))
        return true;
    if (!std::isdigit(a[0]) && std::isdigit(b[0]))
        return false;
    if (!std::isdigit(a[0]) && !std::isdigit(b[0])) {
        if (a[0] == b[0])
            return compareNat(a.substr(1), b.substr(1));
        return (toUpper(a) < toUpper(b));
        //toUpper() is a function to convert a std::string to uppercase.
    }

    // Both strings begin with digit --> parse both numbers
    std::istringstream issa(a);
    std::istringstream issb(b);
    int ia, ib;
    issa >> ia;
    issb >> ib;
    if (ia != ib)
        return ia < ib;

    // Numbers are the same --> remove numbers and recurse
    std::string anew, bnew;
    std::getline(issa, anew);
    std::getline(issb, bnew);
    return (compareNat(anew, bnew));
}


void common::read_frames(std::vector<std::vector<cv::Mat>> &frames) {
    DIR *dir;
    struct dirent *diread;
    std::vector<std::string> classes;


    std::string relative_path = "../data/"; // relative path of project

    if ((dir = opendir("../data")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            classes.push_back(relative_path + diread->d_name); //file list of directory
        }
        closedir(dir);
    } else {
        std::cout << "Could not read the name of images from file" << std::endl;
    }
    std::sort(classes.begin(), classes.end(), compareNat); // to sort the string vector
    std::vector<std::string> classnew;

    for (int j = 0; j < classes.size(); j++) {
        size_t found = classes[j].find("/."); // find the hidden files in directory we do not need them.

        if (found == std::string::npos) {
            classnew.push_back(classes[j]);
        }
    }
    classes.clear();
    for (int j = 0; j < classnew.size(); j++) {
        std::cout << "Class - " << j << " = " << classnew[j] << std::endl;
        std::vector<std::string> files;
        if ((dir = opendir(classnew[j].c_str())) != nullptr) {
            while ((diread = readdir(dir)) != nullptr) {
                files.push_back(classnew[j] + "/" + diread->d_name); //file list of directory
            }
            closedir(dir);
            std::sort(files.begin(), files.end(), compareNat); // to sort the string vector
            frames.emplace_back();
            for (int i = 0; i < files.size(); i++) {
                //std::cout << "asd" << std::endl;
                size_t found = files[i].find("/."); // find the hidden files in directory we do not need them.

                if (found == std::string::npos) {
                    frames[j].push_back(imread(files[i], cv::IMREAD_COLOR));
                    std::cout << "image found: " << files[i] << std::endl;
                }

            }
        }

//    for(int k = 0; k < test_frames.size(); k++){
//        for(int i = 0; i < test_frames[k].size(); i++){
//            cv::imshow("test",test_frames[k][i]);
//            cv::waitKey();
//
//        }
//
//    }

    }

}

cv::Mat common::combine_frames(std::vector<cv::Mat> &frames) {
    cv::Mat combined_frames;
    cv::hconcat(frames, combined_frames); //To combine frames side by side.
    const double resolution_width = 960;
    double scale_constant = resolution_width / combined_frames.size().width;
    cv::Mat resized_img;
    cv::resize(combined_frames, resized_img, cv::Size(0, 0), scale_constant, scale_constant);

    return resized_img;
}

image<rgb> *convertMatToNativeImage(const cv::Mat &input) {
    int w = input.cols;
    int h = input.rows;
    image<rgb> *im = new image<rgb>(w, h);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            rgb curr;
            cv::Vec3b intensity = input.at<cv::Vec3b>(i, j);
            curr.b = intensity.val[0];
            curr.g = intensity.val[1];
            curr.r = intensity.val[2];
            im->data[i * w + j] = curr;
        }
    }
    return im;
}

cv::Mat convertNativeToMat(image<rgb> *input) {
    int w = input->width();
    int h = input->height();
    cv::Mat output(cv::Size(w, h), CV_8UC3);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            rgb curr = input->data[i * w + j];
            output.at<cv::Vec3b>(i, j)[0] = curr.b;
            output.at<cv::Vec3b>(i, j)[1] = curr.g;
            output.at<cv::Vec3b>(i, j)[2] = curr.r;
        }
    }

    return output;
}


void common::find_contours(cv::Mat &segmented_img, std::vector<std::vector<cv::Point>> &contours) {
    cv::cvtColor(segmented_img, segmented_img, cv::COLOR_BGR2GRAY);
    cv::Canny(segmented_img, segmented_img, 1, 2, 3, true);
    cv::blur(segmented_img, segmented_img, cv::Size(3, 3));

    cv::findContours(segmented_img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
   // std::cout << contours.size() << std::endl;
    cv::Mat a(segmented_img.size(), CV_64FC1, cv::Scalar(0));

    for (int k = 0; k < contours.size(); k++) {
        cv::drawContours(a,contours, k, cv::Scalar(255, 255, 255), 1);
    }
    cv::imshow("Segmented Image with Contours", a);
    cv::waitKey();

}

void common::get_segmented(cv::Mat &frame, cv::Mat &segmented) {
    int num_ccs;
    image<rgb> *nativeImage = convertMatToNativeImage(frame);
    image<rgb> *segmentedImage = segment_image(nativeImage, 0.5, 500, 200, &num_ccs);
    segmented = convertNativeToMat(segmentedImage);
    cv::imshow("Segmented Image", segmented);
    cv::waitKey();
}

std::vector<cv::KeyPoint> common::detect_keypoints(cv::Mat &c_frame) {

    std::vector<cv::KeyPoint> kp;
    int minHessian = 400;
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create(minHessian);

    detector->detect(c_frame, kp);

    return kp;

}

void common::segmentation(std::vector<std::vector<cv::Mat>> &frames) {

    const int N = 8;
    for (int i = 0; i < frames.size(); i++) {

        tracker tracker;
        cv::Mat c_segmented, p_segmented;
        std::vector<segment> main_segments, c_segments, p_segments;
        std::vector<cv::KeyPoint> c_keypoints, p_keypoints;
        std::vector<std::vector<cv::Point>> c_contours, p_contours;
        common::get_segmented(frames[i][0], p_segmented);
        common::find_contours(p_segmented, p_contours);
        p_keypoints = common::detect_keypoints(frames[i][0]);

        p_segments.reserve(p_contours.size());
        for (int k = 0; k < p_contours.size(); k++) {

            segment segment(p_contours[k], p_keypoints, frames[i][0]);
            p_segments.push_back(segment);
        }

        sort(p_segments.begin(), p_segments.end(), [](auto &&lhs, auto &&rhs) {
            return lhs.area > rhs.area;
        });

        for (int k = 0; k < N; k++) {
            main_segments.push_back(p_segments[k]);
            //std::cout << "main area" << main_segments[k].area << "  " << main_segments[k].keypoints.size() << std::endl;
        }

        for (int j = 1; j < frames[i].size(); j++) {

            //std::cout << j << std::endl;
            c_keypoints = common::detect_keypoints(frames[i][j]);
            common::get_segmented(frames[i][j], c_segmented);
            common::find_contours(c_segmented, c_contours);

            c_segments.reserve(c_contours.size());
            for (int k = 0; k < c_contours.size(); k++) {

                segment segment(c_contours[k], c_keypoints, frames[i][j]);
                c_segments.push_back(segment);

//                std::cout << "c area" << c_segments[k].area << "  " << c_segments[k].keypoints.size() << "  "
//                          << c_segments.size() << std::endl;
            }

            sort(main_segments.begin(), main_segments.end(), [](auto &&lhs, auto &&rhs) {
                return lhs.area > rhs.area;
            });


            sort(c_segments.begin(), c_segments.end(), [](auto &&lhs, auto &&rhs) {
                return lhs.area > rhs.area;
            });


            if (main_segments.size() > N) {
                main_segments.erase(main_segments.begin() + N, main_segments.end());
            }
            for (int m = 0; m < main_segments.size(); m++) {

                main_segments[m].current_visibility = false;
            }

            for (int k = 0; k < N; k++) {

                for (int l = 0; l < N; l++) {
                    //std::cout << " k l " << k << "  " << l << "  " << main_segments[k].keypoints.size() << std::endl;
                    if (tracker.match(c_segments[l], main_segments[k]) > -1) {
                        if (!main_segments[k].current_visibility) {
                            if (main_segments[k].number_of_occurance == 0) {

                            }
                            point temp = main_segments[k].center;

                            c_segments[l].number_of_occurance = main_segments[k].number_of_occurance + 1;
                            main_segments[k] = c_segments[l];
                            main_segments[k].current_visibility = true;
                            main_segments[k].last_seen = l;
                            main_segments[k].previous_pose = temp;
                            if(main_segments[k].number_of_occurance==1){
                                main_segments[k].initial_pose = main_segments[k].previous_pose;
                            }
                        }

                    } else {
                        c_segments[l].current_visibility = true;
                        c_segments[l].last_seen = l;

                        main_segments.push_back(c_segments[l]);
                        main_segments[main_segments.size() - 1].initial_pose = main_segments[main_segments.size() -
                                                                                             1].center;
                        main_segments[main_segments.size() - 1].previous_pose = main_segments[main_segments.size() -
                                                                                              1].initial_pose;
                    }
                }
            }

            if (main_segments.size() > N) {
                main_segments.erase(main_segments.begin() + N, main_segments.end());
            }

            for(int l=0; l< N; l++){
                if(main_segments[l].number_of_occurance==0){
                    main_segments[l].initial_pose = main_segments[l].center;
                    main_segments[l].previous_pose = main_segments[l].initial_pose;
                }


            }

            for (int l = 0; l < main_segments.size(); l++) {
                std::cout << "Segment " << l << "  current visibility: " << main_segments[l].current_visibility
                          << "  number of occurance: "
                          << main_segments[l].number_of_occurance << "  x:" << main_segments[l].center.x << "  y:"
                          << main_segments[l].center.y << "  previous x:" << main_segments[l].previous_pose.x
                          << "  previous y:" << main_segments[l].previous_pose.y << "  initial x:"
                        << main_segments[l].initial_pose.x
                        << "  initial y:" << main_segments[l].initial_pose.y << std::endl;
            }


            for(int l=0; l<N; l++){
                if(main_segments[l].current_visibility){

                    cv::arrowedLine(frames[i][j],cv::Point(main_segments[l].previous_pose.x,main_segments[l].previous_pose.y),cv::Point (main_segments[l].center.x,main_segments[l].center.y)
                             ,cv::Scalar(0,255,0),1);
                    cv::drawMarker(frames[i][j], cv::Point(main_segments[l].center.x,main_segments[l].center.y),  cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 10, 1);

                    for (int k = 0; k < N; k++) {
                        cv::polylines(frames[i][j], main_segments[l].contour, false, cv::Scalar(255, 0, 0), 1);
                    }
                }
            }

            cv::resize(frames[i][j], frames[i][j], cv::Size(), 3, 3);
            imshow("Current Position", frames[i][j]);

            cv::waitKey();

            p_segments = c_segments;
            p_contours = c_contours;
            p_keypoints = c_keypoints;
            c_segments.clear();
            c_contours.clear();
            c_keypoints.clear();
        }
    }
}