# MpvSlideshow

Opens all photos and videos in a directory and shows them as a slideshow with a simple black crossfade.<br>
Uses [mpv](https://mpv.io/) as backend.<br>
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
* [ ] Add audio fade in/out for videos
* [ ] Fade to black on image/video end, then fade in from black for the next image/video
* [ ] Hide video seek bar when no video is displayed
* [ ] Hide controls and cursor when mouse cursor is not moved for a while
* [ ] Add keyboard shortcuts
* [ ] Add help dialog
* [x] Rotate images according to their EXIF data
* [x] Do not throw away the playlist when it ends

