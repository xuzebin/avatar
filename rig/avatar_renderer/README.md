## Avatar Renderer
The file `AvatarRenderer.cpp` currently is messy as it integrates different stuff all together.

It has two functions:

* Render a face model and transform it using the model matrix sent from the client.
* Render point cloud data sent from the client.

I will separate these functions as well as the renderer and server later.

### Compile
```
mkdir build
cd build
cmake ..
make
make install
```
