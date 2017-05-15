#include "SocketServer.h"

namespace avt {
    SocketServer::SocketServer(std::shared_ptr<BufferData> bufferData) : bufferData(bufferData)
    {
    }

    SocketServer::~SocketServer()
    {
    }

    void SocketServer::start(int port) {
        socket.bind(port);
        socket.listen(5);
        std::cout << "waiting to connect..." << std::endl;
        Socket newSocket = socket.accept();
        std::cout << "connected to port " << port << std::endl;
        while (true) {
            const char* buffer = newSocket.recv(MAX_BUFFER_LENGTH, 0);
            if (buffer == NULL) {
                std::cout << "client disconnected." << std::endl;
                break;
            }
            bufferData->decodeBuffer(buffer);
        }
        newSocket.close();
    }
}

