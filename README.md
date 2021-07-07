# libepoll utility

[![BSD license](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This is a epoll utility. Programmers can easily compile to static library and use epoll into their project.

## How to use:

Git clone this repository

	# git clone https://github.com/w180112/libepoll.git

Type

	# cd libepoll/src

Use GNU build system to generate configure file and makefile

	# autoreconf --install
	# ./configure

Type make to compile

	# make

Then you can find there is a static library in the same directory.

There is a eample usage in example folder, just type following to compile

	# gcc epoll-server-example.c -Ilib/libepoll -Ilib/libutil -Llib/libepoll -lepoll -Llib/libutil -lutil -lpthread -o epoll

To remove the binary files

	# make clean

