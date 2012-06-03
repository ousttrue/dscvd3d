#pragma once
#include <deque>
#include <memory>
#include <opencv2/opencv.hpp>


class Image;
class OpenCV
{
    const cv::Size m_patternSize;
    std::vector<std::shared_ptr<Image> > m_images;
    cv::vector<cv::Mat> m_checkerImgs;
    cv::vector<cv::vector<cv::Point2f> > m_imagePoints;
    cv::vector<cv::vector<cv::Point3f> > m_worldPoints;
    cv::Mat m_cameraMatrix;
    cv::Mat m_distCoeffs;
    cv::vector<cv::Mat> m_rotationVectors;
    cv::vector<cv::Mat> m_translationVectors;

public:
    OpenCV();
    void Process(unsigned char *data, int w, int h);

    void Push(std::shared_ptr<Image> image);
};

