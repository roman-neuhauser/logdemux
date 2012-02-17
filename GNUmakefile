# vim: sw=2 sts=2 ts=8

CPPFLAGS=-I/usr/local/include
CXXFLAGS=-std=gnu++0x
LD=$(CXX)
LDFLAGS=-L/usr/local/lib
LIBS=-lboost_date_time

UNAME:=$(shell uname -s)

ifeq (MINGW,$(findstring MINGW,$(UNAME)))
dot_exe=.exe
else
dot_exe=
endif

all: logdemux$(dot_exe)
logdemux$(dot_exe): logdemux.o
	$(LD) $(LDFLAGS) -o$@ $< $(LIBS)
logdemux.o: logdemux.cpp
