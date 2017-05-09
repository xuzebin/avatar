# avatar
Capture facial motion without markers using monocular or binocular cameras and render a real-time avatar.

This project is still under development.

## Dependencies
* OpenCV (>=3.2.0)
* dlib (included)
* OpenGL
* GLUT
* GFrame (included)

## How to Compile
This will build all the libraries and executables.

```
mkdir build
cd build
cmake .. -DUSE_AVX_INSTRUCTIONS=1
make -j8
make install
```
## How to Run
First, start the server
(Currently, I hardcode it to listen to port 5055).

```
./AvatarServer
```
And then, run the client

```
./HeadPoseEstimation
```
