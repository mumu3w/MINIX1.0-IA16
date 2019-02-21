#!/bin/bash

function clean {
cd tools
make clean 1>>/dev/null 2>&1
cd ..

cd lib
make clean 1>>/dev/null 2>&1
cd ..

cd kernel
make clean 1>>/dev/null 2>&1
cd ..

cd fs
make clean 1>>/dev/null 2>&1
cd ..

cd mm
make clean 1>>/dev/null 2>&1
cd ..

cd tools
make clean 1>>/dev/null 2>&1
cd ..

cd test
make clean 1>>/dev/null 2>&1
cd ..

cd cmds
make clean 1>>/dev/null 2>&1
cd ..

rm -fr log.txt 1>>/dev/null 2>&1
}

clean
echo "OK!"