# Face Landmark Detection
Face landmark detection example using dlib.

## Dependencies
* OpenCV (>=2.4.3)
* dlib

## Usage
### Compile

```
mkdir build
cd build
cmake .. -DUSE_AVX_INSTRUCTIONS=1
make -j8
```

### Run
```
./face_landmark_detection ../../data/shape_predictor_68_face_landmarks.dat
```

## Face Landmarks positions
![](../../res/face_landmark.jpg)
