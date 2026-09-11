all := libqsys qsys_dlopen_test

LDLIBS-libqsys-Unix := -ldl
LDLIBS-qsys_dlopen_test := -lqsys

-include ../mk/include.mk

test: all
	LD_LIBRARY_PATH=./lib ./bin/qsys_dlopen_test
