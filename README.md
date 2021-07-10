# libepoll utility

[![BSD license](https://img.shields.io/badge/License-BSD-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

This is a epoll utility. Programmers can easily compile to static library and use epoll into their project.

## How to use:

Git clone this repository

	# git clone https://github.com/w180112/libepoll.git
	# cd libepoll && git submodule update --init --recursive

First time we need to use GNU build system to generate configure file and makefile

	# make init

Then make to compile all the dependency library, libepoll and the examples

	# make

To remove the binary files

	# make clean
