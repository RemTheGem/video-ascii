# Video to ASCII (Qt)

A simple application that turns local video files or your camera into ASCII!

## Features
- Load Videos (mp4, mov, mkv)
- Invert Brightness option
- Increase and decrease contrast
- Pause and Play options
- Plays the audio with the ASCII video
- Play your camera feed live!
- Play ASCII with the video's original color or grayscale

## How it works

The application takes each frame of the video file/camera and converts each pixel's brightness into ASCII characters, mapping dark pixels to dense characters and light ones to sparse characters and vice versa if inverted. For color, it scales the image to an acceptable size, gets the pixel color and formats the output ASCII accordingly.

![Demo](assets/demo.gif)

## How to use
Local Video File
1. Click "Add Video"
2. Select the video file you want to convert
3. Click Play

Camera
1. Click "Camera"
2. Enjoy!!?

## Clone the Repository

```bash
git clone https://github.com/RemTheGem/video-ascii.git
```
