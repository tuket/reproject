This is a simple command line program that changes the projection of textures.

You can transform from cubemap to latlong (also know as equirectangular), and viceversa.

![reproject](data/reproject.png)

It uses OpenGL 3.3.

### Download the executable
You can [download](https://github.com/tuket/reproject/releases) the tool as a single executable (Linux and Windows).

### Usage

There are 2 kinds of command:
```
reproject latlongToCubemap <inputFile> <outputFile> <outputFaceResolution> [numSamples]
reproject cubemapToLatlong <inputFile> <outputFile> <outputWidth>x<outputHeight> [numSamples]
```

The number of samples is optional. The more, the better quality (at the expense of more computation cost). The default value is 128, which is usually more that enough.

Examples:

```
$ ./reproject latlongToCubemap myLatlong.hdr newCubemap.hdr 512

$ ./reproject cubemapToLatlong myCubemap.png newLatlong.png 2048x1024 1000
```

### Supported formats
This project uses [stb](https://github.com/nothings/stb) for loading and saving images.
- [Formats you can load](https://github.com/tuket/reproject/blob/e1550e9a591ef4e679c74727c86f45fa73a90a08/libs/stb/stb_image.h#L19)
- [Formats you can save](https://github.com/tuket/reproject/blob/e1550e9a591ef4e679c74727c86f45fa73a90a08/libs/stb/stb_image_write.h#L45)

### Compiling

This compiles like any normal CMake project.

This is how you can compile in Linux:

```
cd reproject
mkdir build
cd build
cmake ..
ninja
```

You might need to install some of these libraries:
```
sudo apt install libx11-dev libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev libopengl-dev
```

### License
[MIT](LICENSE.txt)
