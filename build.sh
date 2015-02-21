#!/bin/bash
rm -rf _build &2>/dev/null
mkdir _build
cd ./_build
cmake ../

if [ $OS == "Windows_NT" ]; then
	devenv.com tinyEngine.sln /Build Release
else
	make
fi

cd -