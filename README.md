# fendemo
A simple graphics demo written in C using SDL2

Just a simple graphic demo that demonstrates loading and manipulating image data using SDL2.

This can be compiled with the command:
```
make
```
In addition, the Makefile also has a target to deploy to the web with emscripten. First make sure that emscripten is installed and configured on your computer, then run the following command to generate an html and data file that can be used to deploy the app on the web:
```
make fendemo-web
```

A live demo of the emscripten version is available to view [here](https://fenteale.com/fendemo.html).