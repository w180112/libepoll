#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(libepoll, 1.0, w180112@gmail.com)
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([epoll.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
# Checks for libraries.
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_RESTRICT
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
