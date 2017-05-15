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
* CMake

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

All executables are installed in `build/bin/`

#### Run FaceViewer

```
./FaceViewer face.obj shaders/ 5055
```
* Arguments: `./FaceViewer [obj file] [shaders dirctory] [port number]`

Or directly run a default version (same as the above one)

```
./FaceViewer
```

This will

* Start rendering a face mask.
* Start a separate thread running a server. It will transform the face model using the pose data sent from the capture client.

#### Run Capture Client
Open another terminal and run the client for capturing head pose.

```
./HeadPoseEstimation
```

* By default the above command it reads in the pose model file located in current directory, and connected to localhost:5055. You can customize it by running: 
```
./HeadPoseEstimation shape_predictor_68_face_landmarks.dat [ip-address] [port number]
```

This will

* Open a camera (by default connected to device 0), capture facial landmark on your face and estimate head pose in realtime.

* Press key `s` to start streaming pose data to server so you can see your avatar mimicking your head pose.
