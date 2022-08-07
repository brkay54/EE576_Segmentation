cmake_minimum_required(VERSION 3.17)
project(Hw6_KaramanB)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 14)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(main main.cpp include/common.cpp include/common.h include/convolve.h include/disjoint-set.h include/filter.h
        include/image.h include/imconv.h include/imutil.h include/misc.h include/pnmfile.h
        include/segment-graph.h include/segment-image.h include/segment.h include/segment.cpp
        include/tracker.cpp include/tracker.h)
target_link_libraries(main ${OpenCV_LIBS} )
