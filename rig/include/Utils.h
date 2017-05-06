#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

void overlayHorizontalLines(cv::Mat& img) {
    static const int LINES_NUM = 10;
    int interval = img.rows / LINES_NUM;
    int col = img.cols;
    for (int i = 1; i <= LINES_NUM; i++) {
        cv::Point p1(0, i * interval);
        cv::Point p2(col, i * interval);
        cv::Scalar color(0, 255, 0);
        cv::line(img, p1, p2, color, 1);
    }
}

// t start time
// img the image to overlay text on
void overlayFps(double t, cv::Mat& img) {  
    //Calculate fps
    double fps;
    char string[10];
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    fps = 1.0 / t;

    //Save it in a string
    sprintf(string, "%.2f", fps);      
    std::string fpsString("FPS:");
    fpsString += string; 
                  
    //output
    putText(img, // matrix
            fpsString,                  // string
            cv::Point(5, 20),           // coordinate
            cv::FONT_HERSHEY_SIMPLEX,   // font
            0.5, // size
            cv::Scalar(0, 0, 255));       // color
}

// Checks if a matrix is a valid rotation matrix.
bool isRotationMatrix(const cv::Mat &R) {
    cv::Mat Rt;
    transpose(R, Rt);
    cv::Mat shouldBeIdentity = Rt * R;
    cv::Mat I = cv::Mat::eye(3,3, shouldBeIdentity.type());
     
    return  norm(I, shouldBeIdentity) < 1e-6;
}
 
// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
void rotationMatrixToEulerAngles(const cv::Mat &R, float&x, float& y, float& z) {
    assert(isRotationMatrix(R));
     
    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0));
 
    bool singular = sy < 1e-6;
 
    //    float x, y, z;
    if (!singular) {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    } else {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    x = x * 180.0 / 3.1415926;
    y = y * 180.0 / 3.1415926;
    z = z * 180.0 / 3.1415926;
    std::cout << "euler angle: " << x << "," << y << "," << z << std::endl;
}

// Converts a given Rotation Matrix to Euler angles
cv::Mat rot2euler(const cv::Mat & rotationMatrix)
{
    cv::Mat euler(3,1,CV_64F);

    double m00 = rotationMatrix.at<double>(0,0);
    double m02 = rotationMatrix.at<double>(0,2);
    double m10 = rotationMatrix.at<double>(1,0);
    double m11 = rotationMatrix.at<double>(1,1);
    double m12 = rotationMatrix.at<double>(1,2);
    double m20 = rotationMatrix.at<double>(2,0);
    double m22 = rotationMatrix.at<double>(2,2);

    double x, y, z;

    // Assuming the angles are in radians.
    if (m10 > 0.998) { // singularity at north pole
        x = 0;
        y = CV_PI/2;
        z = atan2(m02,m22);
    }
    else if (m10 < -0.998) { // singularity at south pole
        x = 0;
        y = -CV_PI/2;
        z = atan2(m02,m22);
    }
    else
        {
            x = atan2(-m12,m11);
            y = asin(m10);
            z = atan2(-m20,m00);
        }

    euler.at<double>(0) = x;
    euler.at<double>(1) = y;
    euler.at<double>(2) = z;

    return euler;
}

// Converts a given Euler angles to Rotation Matrix
cv::Mat euler2rot(const cv::Mat & euler)
{
    cv::Mat rotationMatrix(3,3,CV_64F);

    double x = euler.at<double>(0);
    double y = euler.at<double>(1);
    double z = euler.at<double>(2);

    // Assuming the angles are in radians.
    double ch = cos(z);
    double sh = sin(z);
    double ca = cos(y);
    double sa = sin(y);
    double cb = cos(x);
    double sb = sin(x);

    double m00, m01, m02, m10, m11, m12, m20, m21, m22;

    m00 = ch * ca;
    m01 = sh*sb - ch*sa*cb;
    m02 = ch*sa*sb + sh*cb;
    m10 = sa;
    m11 = ca*cb;
    m12 = -ca*sb;
    m20 = -sh*ca;
    m21 = sh*sa*cb + ch*sb;
    m22 = -sh*sa*sb + ch*cb;

    rotationMatrix.at<double>(0,0) = m00;
    rotationMatrix.at<double>(0,1) = m01;
    rotationMatrix.at<double>(0,2) = m02;
    rotationMatrix.at<double>(1,0) = m10;
    rotationMatrix.at<double>(1,1) = m11;
    rotationMatrix.at<double>(1,2) = m12;
    rotationMatrix.at<double>(2,0) = m20;
    rotationMatrix.at<double>(2,1) = m21;
    rotationMatrix.at<double>(2,2) = m22;

    return rotationMatrix;
}




#endif
