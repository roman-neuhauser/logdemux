# vim: sw=2 sts=2 ts=8

DESTDIR?=
PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin

CPPFLAGS=-I/usr/local/include
CXXFLAGS=$(CXXSTD) $(CXXOPTFLAGS) $(CXXWFLAGS)
CXXSTD=-std=gnu++0x -pedantic
CXXOPTFLAGS=-g -O2
CXXWFLAGS=-Wall -Wextra -Wfatal-errors -Wno-long-long

LD=$(CXX)
LDFLAGS=$(CXXOPTFLAGS) -L/usr/local/lib
LIBS=$(link_mode) -lboost_date_time -lboost_regex -liniphile

RM_F?=rm -f
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL) -s

UNAME:=$(shell uname -s)

ifeq (MINGW,$(findstring MINGW,$(UNAME)))
dot_exe=.exe
link_mode=-Wl,-Bstatic
else
dot_exe=
endif
ifeq (static, $(LINK_MODE))
link_mode=-Wl,-Bstatic
endif

all: logdemux$(dot_exe) README.html

clean:
	$(RM_F) logdemux.o logdemux$(dot_exe) tests/*/*.actual tests/*/*.diff

install: all
	$(INSTALL_PROGRAM) logdemux$(dot_exe) $(DESTDIR)$(BINDIR)/logdemux$(dot_exe)

check: all
	$(SHELL) run-tests.sh tests

%.html: %.rest
	rst2html.py $< $@

logdemux$(dot_exe): logdemux.o
	$(LD) $(LDFLAGS) -o$@ $< $(LIBS)

logdemux.o: logdemux.cpp

.PHONY: all clean install
