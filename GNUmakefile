# vim: sw=2 sts=2 ts=8

DESTDIR?=
PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin

CPPFLAGS=-I/usr/local/include
CXXFLAGS=-std=gnu++0x
LD=$(CXX)
LDFLAGS=-L/usr/local/lib
LIBS=-lboost_date_time -lboost_regex -liniphile

RM_F?=rm -f
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL) -s

UNAME:=$(shell uname -s)

ifeq (MINGW,$(findstring MINGW,$(UNAME)))
dot_exe=.exe
else
dot_exe=
endif

all: logdemux$(dot_exe)

clean:
	$(RM_F) logdemux.o logdemux.exe

install: all
	$(INSTALL_PROGRAM) logdemux$(dot_exe) $(DESTDIR)$(BINDIR)/logdemux$(dot_exe)

logdemux$(dot_exe): logdemux.o
	$(LD) $(LDFLAGS) -o$@ $< $(LIBS)

logdemux.o: logdemux.cpp

.PHONY: all clean install
