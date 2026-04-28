#include "../Libs/headers.h"

cv::Mat Q1();
cv::Mat Q2();
cv::Mat Q3();
cv::Mat Q4(std::uint8_t slider);
cv::Mat Q5(cv::Point p1, cv::Point p2);
bool Q6(cv::VideoCapture& vid, const cv::Mat& bg_gray, cv::Mat& frame_saida);
cv::Mat Q7(std::string& stats_out);
