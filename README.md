# Pony

A Go library for building desktop applications by combining Javascript, a web
browser and native GUI components.

For now only Windows is supported, MacOS will be added at some point.

For a demo and a scaffold project see [Saddle](https://github.com/mat007/saddle).

## Prerequisites

The following components are required for building the project:

1. [Go](http://golang.org)
2. [MinGW](http://mingw.org)

## Building

1. Open a MinGW terminal
2. Run `build.bat`

## Chromium Embedded Framework

Current [CEF](https://bitbucket.org/chromiumembedded/cef) version is 3.3359.1768.g8e7c5d6

To upgrade it follow these steps:
1. Download a "Minimal Distribution" archive from [CEF builds](http://opensource.spotify.com/cefbuilds/index.html)
3. Extract `cef_binary*/Release/*` to `cef/Release`
4. Extract `cef_binary*/Resources/*` to `cef/Resources`
5. Extract `cef_binary*/include/*` to `cef/include`
6. Download a "Sample Application" archive from [CEF builds](http://opensource.spotify.com/cefbuilds/index.html)
7. Extract `cef_binary*/Release/cefclient.exe` to `cef/Release`
