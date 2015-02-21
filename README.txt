For building I use cmake, so make sure that you have it, and it added to your path

Build for Windows:
Assume you have Microsoft Visual Studio compiler and cygwin console
1) Download cygwin:
2) add path msvc IDE to your PATH, typically is:
    C:\Program Files (x86)\<Microsoft Visual Studio version > \Common7\IDE
3) open cygwin console
4) cd <path to repo>
5) ./build.sh
To run:
1) run.sh #this script adds paths to dll's to PATH and run program