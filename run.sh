#!/bin/bash

if [ $OS == "Windows_NT" ]; then
	root=$(pwd | sed 's/\/cygdrive\/\(.\)\//\1:\\/g')
	PathToGlew="$root\third_party\glew\bin\Release\Win32"
	PathToLua="$root\third_party\lua"
	PathToAssimp="$root\third_party\assimp"
	PathToFreeImage="$root\third_party\FreeImage"
	PathToGlfw="$root\third_party\glfw\lib-vc2012"
	PATH="$PATH;$PathToGlew;$PathToLua;$PathToAssimp;$PathToFreeImage;$PathToGlfw" && ./_build/Release/Engine.exe
fi 