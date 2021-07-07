# libepoll utility

[![BSD license](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This is a epoll utility. Programmers can easily compile to static library and use epoll into their project.

## How to use:

Git clone this repository

	# git clone https://github.com/w180112/libepoll.git

Use GNU build system to generate configure file and makefile to compile dependency library

	# cd libepoll/lib/libutil && git submodule update --init --recursive
	# autoreconf --install 
	# ./configure && make

Use the same way to compile libepoll

	# cd ../../src
	# autoreconf --install
	# ./configure
	# make

Then you can find there is a static library in the same directory.

There is a eample usage in example folder, just type following to compile

	# gcc epoll-server-example.c -I../src -I../lib/libutil -L../src -lepoll -L../lib/libutil -lutil -lpthread -o epoll

To remove the binary files

	# make clean
