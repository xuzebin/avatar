#include "SocketClient.h"
#include <string>
#include "opencv2/core/core.hpp"
#include <sstream>
#include <iomanip>

#define AVT_PI 3.14159265358979323846264338327950288
#define RADIAN_TO_ANGLE(r)  (r * 180 / AVT_PI)

namespace avt {

    SocketClient::SocketClient() {
        this->frameCount = 0;
    }

    SocketClient::~SocketClient() {

    }

    void SocketClient::connect(const char* server, int port) {
        try {
            socket.connect(server, port);
            std::cout << "Socket client connected to " << server << ":" << port << std::endl;
        } catch (int e) {
            std::cerr << "Socket client failed to connect to " << server << ":" << port << std::endl;
        }
    }

    void SocketClient::close() {
        socket.close();
    }

    inline std::string encodeMayaTranslation(const std::string& name, const avt::Point2f& p) {
        float scale = 3.5;
        return "setAttr " + name + ".translateX " + std::to_string(p.x * scale) + 
            ";\n setAttr " + name + ".translateY " + std::to_string(-p.y * scale) + ";\n";
    }

    inline std::string encodeMayaTranslation(const std::string& name, const cv::Point2f& pp) {
        cv::Point2f p(pp);
        double precision = 0.01;  // i.e. round to nearest one-hundreth
        p.x = floor(p.x / precision + 0.5) * precision;
        p.y = floor(p.y / precision + 0.5) * precision;

        float scale = 3.5;
        return "setAttr " + name + ".translateX " + std::to_string(-p.x * scale) + 
            ";\n setAttr " + name + ".translateY " + std::to_string(-p.y * scale) + ";\n";
    }

    inline std::string encodePose(const cv::Point3f& eulerAngle) {
        return ("setAttr cluster1Handle.rotateX " + std::to_string(RADIAN_TO_ANGLE(eulerAngle.x)) + ";\n" + "setAttr cluster1Handle.rotateY " + std::to_string(RADIAN_TO_ANGLE(eulerAngle.y)) + ";\n" + "setAttr cluster1Handle.rotateZ " + std::to_string(RADIAN_TO_ANGLE(eulerAngle.z)) + ";\n");
    }

    bool SocketClient::send2Maya(const TrackingDataf& data) {
        std::string cmd = "";
        cmd += "currentTime (`currentTime -query` + 1);\n";
        cmd += encodeMayaTranslation("lob", data.markers[17]);
        cmd += encodeMayaTranslation("lib", data.markers[21]);
        cmd += encodeMayaTranslation("rib", data.markers[22]);
        cmd += encodeMayaTranslation("rob", data.markers[26]);
        cmd += encodeMayaTranslation("ln", data.markers[31]);
        cmd += encodeMayaTranslation("rn", data.markers[35]);
        cmd += encodeMayaTranslation("ul", data.markers[51]);
        cmd += encodeMayaTranslation("lm", data.markers[48]);
        cmd += encodeMayaTranslation("rm", data.markers[54]);
        cmd += encodeMayaTranslation("ll", data.markers[57]);
        cmd += "setKeyframe lob lib rib rob ln rn lm ul rm ll;\n";

        frameCount++;
        std::cout << cmd << std::endl;

        try {
            if (socket.send(cmd) < cmd.length()) {
                std::cout << "full message not sent!" << std::endl;
            }
            return true;
        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "end" << std::endl;
        return true;
    }

    bool SocketClient::send2Maya(const std::vector<cv::Point2f> motion) {
        std::string cmd = "";
        cmd += "currentTime (`currentTime -query` + 1);\n";
        cmd += encodeMayaTranslation("lob", motion[17]);
        cmd += encodeMayaTranslation("lib", motion[21]);
        cmd += encodeMayaTranslation("rib", motion[22]);
        cmd += encodeMayaTranslation("rob", motion[26]);
        cmd += encodeMayaTranslation("ln", motion[31]);
        cmd += encodeMayaTranslation("rn", motion[35]);
        cmd += encodeMayaTranslation("ul", motion[51]);
        cmd += encodeMayaTranslation("lm", motion[48]);
        cmd += encodeMayaTranslation("rm", motion[54]);
        cmd += encodeMayaTranslation("ll", motion[57]);
        cmd += "setKeyframe lob lib rib rob ln rn lm ul rm ll;\n";

        frameCount++;
        std::cout << cmd << std::endl;

        try {
            if (socket.send(cmd) < cmd.length()) {
                std::cout << "full message not sent!" << std::endl;
            }
            return true;
        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "end" << std::endl;
        return true;
    }

    bool SocketClient::send2Avatar(const TrackingDataf& data) {
        std::string cmd = "";
        double scaling = 2.5;
        for (int i = 0; i < 68; ++i) {
            cmd += (std::to_string(i)
                    + "," + std::to_string(data.markers[i].x * scaling)
                    + "," + std::to_string(-data.markers[i].y * scaling)
                    + "\n");
        }

        frameCount++;
        std::cout << cmd << std::endl;

        try {
            if (socket.send(cmd) < cmd.length()) {
                std::cout << "full message not sent!" << std::endl;
            }

        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "message sent" << std::endl;
        return true;
    }

    bool SocketClient::send2Avatar(const cv::Mat& quaternion) {
        std::ostringstream strs;
        strs << std::setprecision(6)
             << quaternion.at<double>(0) << " "
             << quaternion.at<double>(1) << " " 
             << quaternion.at<double>(2) << " " 
             << quaternion.at<double>(3);
        std::string cmd = strs.str();

        frameCount++;
        std::cout << cmd << std::endl;
        try {
            if (socket.send(cmd) < cmd.length()) {
                std::cout << "full message not sent!" << std::endl;
            }

        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "message sent" << std::endl;
        return true;
    }

    // Send 3d points to avatar server
    bool SocketClient::send2Avatar3d(std::vector<cv::Point3f> points) {
        assert(points.size() == 68);
        std::string cmd = std::to_string(frameCount) + " ";
        for (int i = 0; i < points.size(); ++i) {
            cmd += std::to_string(points[i].x) + " " + std::to_string(points[i].y) + " " + std::to_string(points[i].z) + " ";
        }
        frameCount++;
        //        std::cout << cmd << std::endl;
        try {
            int n = socket.send(cmd);
            std::cout << n << "bytes sent" << std::endl;
        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "message sent" << std::endl;
        return true;
    }

    // Send rotation Matrix 3x3 to avatar server
    bool SocketClient::sendRotMat2Avatar(const cv::Mat& r) {
        std::ostringstream strs;
        assert(r.rows == 3 && r.cols == 3);
        strs << frameCount << " ";
        for (int i = 0; i < r.rows; ++i) {
            strs << std::setprecision(6)
                 << r.at<double>(i, 0) << " "
                 << r.at<double>(i, 1) << " " 
                 << r.at<double>(i, 2) << " ";
        }
        std::string cmd = strs.str();
        frameCount++;
        std::cout << cmd << std::endl;
        try {
            int n = socket.send(cmd);
            std::cout << n << "bytes sent" << std::endl;
        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "message sent" << std::endl;
        return true;
    }


    // Send model matrix 4x4 to avatar server
    bool SocketClient::sendModelMatrix2Avatar(const cv::Mat& r) {
        std::ostringstream strs;
        assert(r.rows == 4 && r.cols == 4);
        strs << frameCount << " ";
        // Row major
        for (int i = 0; i < r.rows; ++i) {
            strs << std::setprecision(6)
                 << r.at<double>(i, 0) << " "
                 << r.at<double>(i, 1) << " " 
                 << r.at<double>(i, 2) << " "
                 << r.at<double>(i, 3) << " ";
        }
        std::string cmd = strs.str();
        frameCount++;
        std::cout << cmd << std::endl;
        try {
            int n = socket.send(cmd);
            std::cout << n << "bytes sent" << std::endl;
        } catch (int e) {
            std::cout << "not connected!" << std::endl;
            connect();
            return false;
        }
        std::cout << "message sent" << std::endl;
        return true;
    }

}
