# avatar
Capture facial motion without markers using monocular or binocular cameras and render a real-time avatar.

This project is still under development.

## Features
* Head pose mimicked by a real-time face mask.
* (TODO) Facial motion mimicked by a real-time avatar.

## Dependencies
* OpenCV (>=3.2.0)
* OpenGL
* GLUT
* dlib (included)
* GFrame (included)

## How to Compile
This will build all the libraries and executables.

```
mkdir build
cd build
cmake .. -DUSE_AVX_INSTRUCTIONS=1
make -j
make install
```

## Running Head Pose Avatar

#### Run FaceViewer

```
./FaceViewer
```
This will:

* Start rendering the face mask.
* Start a server listening to port 5055 by default, or you can change it by adding the port number after the `./FaceViewer`)

#### Run capture client
Open another terminal and run the client for capturing head pose.

```
./HeadPoseEstimation
```

This will:

* By default it is connected to localhost:5055, or you can customize it by running: 
```
./HeadPoseEstimation shape_predictor_68_face_landmarks.dat [ip-address] [port number]
```
* Open a camera (by default connected to device 0), capture facial landmark and estimate your head pose in realtime.

* Press key `s` to start streaming pose data to server so you can see your avatar mimicking your head pose.
