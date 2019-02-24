#!/bin/bash

function lib {
	cd lib
	echo "Start compiling lib"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile lib"
		exit
	fi
	cd ..
}

function kernel {
	cd kernel
	echo "Start compiling kernel"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile kernel"
		exit
	fi
	cd ..
}

function mm {
	cd mm
	echo "Start compiling mm"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile mm"
		exit
	fi
	cd ..
}

function fs {
	cd fs
	echo "Start compiling fs"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile fs"
		exit
	fi
	cd ..
}

function init_fsck {
	cd tools
	echo "Start compiling fsck"
	make fsck.out fsck.sep 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile fsck"
		exit
	fi
	echo "Start compiling init"
	make init.out init.sep 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile init"
		exit
	fi
	cd ..
}

function test {
	cd test
	echo "Start compiling test"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile test"
		exit
	fi
	cd ..
}

function cmds {
	cd cmds
	echo "Start compiling cmds"
	make all 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to compile cmds"
		exit
	fi
	cd ..
}

function tools {
	cd tools
	echo "Start build Image"
	make Image.sep Image.out 1>>../log.txt 2>&1
	if [ $? -ne 0 ]; then
		echo "Failed to build Image"
		exit
	fi
	cd ..
}

echo "The MINIX documentation is contained in the appendices of the following book:"
echo "	Title:     Operating Systems: Design and Implementation"
echo "	Author:    Andrew S. Tanenbaum"
echo "	Publisher: Prentice-Hall (1987)"
echo " "
rm -fr log.txt
lib
kernel
mm
fs
init_fsck
test
cmds
tools
echo "OK!"