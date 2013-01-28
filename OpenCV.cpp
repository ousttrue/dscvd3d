#include "OpenCV.h"
#include "Image.h"
#include "HUD.h"


OpenCV::OpenCV()
: m_patternSize(10, 7)
{
}

void OpenCV::Process(unsigned char *data, int w, int h)
{
    if(m_images.empty()){
        return;
    }
    cv::Mat dst(h, w, CV_8UC4, data);

    cv::Mat src(h, w, CV_8UC4);
    memcpy(src.ptr(), data, w*h*4);

    // 処理
    cv::undistort(src, dst, m_cameraMatrix, m_distCoeffs);
}

void OpenCV::Push(std::shared_ptr<Image> image)
{
    cv::Mat checkerImg(image->GetHeight(), image->GetWidth(), CV_8UC4, image->GetSample());
    cv::vector<cv::Point2f> imagePoint;

    // 交点
    if(!cv::findChessboardCorners(checkerImg, m_patternSize, imagePoint)){
        // 交点検出失敗
        return;
    }

    cv::drawChessboardCorners(checkerImg, m_patternSize, (cv::Mat)(imagePoint), true);            
    m_images.push_back(image);
    m_checkerImgs.push_back(checkerImg);
    m_imagePoints.push_back(imagePoint);
    m_worldPoints.push_back(cv::vector<cv::Point3f>());
    for(int j=0; j<m_patternSize.area(); ++j){
        m_worldPoints.back().push_back(cv::Point3f(
                    static_cast<float>(j%m_patternSize.width*10),
                    static_cast<float>(j/m_patternSize.width*10),
                    0.0));
    }

    if(m_imagePoints.size()>8){
        m_images.erase(m_images.begin());
        m_checkerImgs.erase(m_checkerImgs.begin());
        m_imagePoints.erase(m_imagePoints.begin());
        m_worldPoints.erase(m_worldPoints.begin());
    }

    // calibration
    m_rotationVectors.clear();
    m_translationVectors.clear();
    cv::calibrateCamera(m_worldPoints, m_imagePoints, m_checkerImgs[0].size(),
            m_cameraMatrix, m_distCoeffs, m_rotationVectors, m_translationVectors);

	// (7)XMLファイルへの書き出し
	CvFileStorage *fs;
	fs = cvOpenFileStorage ("camera.xml", 0, CV_STORAGE_WRITE);
	cvWrite (fs, "intrinsic", intrinsic);
	cvWrite (fs, "rotation", rotation);
	cvWrite (fs, "translation", translation);
	cvWrite (fs, "distortion", distortion);
	cvReleaseFileStorage (&fs);
}

