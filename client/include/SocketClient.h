#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <vector>
#include <iostream>
#include "Socket.h"
#include "TrackingData.h"

namespace avt {
    class SocketClient {
	public:	
		SocketClient();
        ~SocketClient();
        void connect(const char* server = "localhost", int port = 5056);

        bool send2Maya(const TrackingDataf& data);
        bool send2Maya(const std::vector<cv::Point2f> motion);
        bool send2Avatar(const TrackingDataf& data);
        bool send2Avatar(const cv::Mat& quaternion);
        bool sendRotMat2Avatar(const cv::Mat& rotationMatrix);
        bool sendModelMatrix2Avatar(const cv::Mat& modelMatrix);
        bool send2Avatar3d(std::vector<cv::Point3f> points);

        void close();

	private:

        Socket socket;
        bool connected;
        long frameCount;
    };
}
#endif 	/* SOCKETCLIENT_H */
