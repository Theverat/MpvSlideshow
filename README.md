# MpvSlideshow

Opens all photos and videos in a directory and shows them as a slideshow with a simple crossfade.  
Uses [mpv](https://mpv.io/) as backend.  
This project was started with the 
[libmpv qt_opengl example](https://github.com/mpv-player/mpv-examples/tree/master/libmpv/qt_opengl) 
as base.

## How to compile

* Install Qt (at least 5.6) and QtCreator
* Install mpv with [libmpv](https://github.com/mpv-player/mpv-build#building-libmpv)
* Open project in QtCreator and compile

I successfully compiled on Linux and Windows.

## Todo

* [x] Fix bug where images are skipped after videos
* [x] Add audio fade in/out for videos
* [x] Instead of fade to black: fade from current to next image
* [x] Hide video seek bar when no video is displayed
* [x] Hide controls and cursor when mouse cursor is outside the controls
* [x] Add keyboard shortcuts
* [ ] Add help dialog
* [x] Rotate images according to their EXIF data
* [x] Do not throw away the playlist when it ends
* [x] Load next file in background and blend to it when user/timer triggers "next"
