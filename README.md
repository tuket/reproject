# reproject
This is a simple command line program that changes the projection of textures.

You can transform from cubemap to latlong (also know as equirectangular), and viceversa.

![reproject](data/reproject.png)

## Download the executable

## Usage

There two king of commands:
```
reproject latlongToCubemap <inputFile> <outputFile> <outputFaceResolution> [numSamples]
reproject cubemapToLatlong <inputFile> <outputFile> <outputWidth>x<outputHeight> [numSamples]
```

The number of samples is optional. The more the better quality (at the expense of more computation cost). The default value is 128, which is usually more that enough.

Examples:

```
$ ./reproject latlongToCubemap myLatlong.hdr newCubemap.hdr 512

$ ./reproject cubemapToLatlong myCubemap.png newLatlong.png 2048x1024 1000
```
