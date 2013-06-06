# vim: sw=2 sts=2 ts=8

DESTDIR?=
PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
MANDIR?=$(PREFIX)/man
MAN1DIR?=$(MANDIR)/man1

BOOSTDIR?=$(PREFIX)

CPPFLAGS=-I$(BOOSTDIR)/include
CXXFLAGS=$(CXXSTD) $(CXXOPTFLAGS) $(CXXWFLAGS)
CXXSTD=-std=c++0x -pedantic
CXXOPTFLAGS=-g -O2
CXXWFLAGS=-Wall -Wextra -Wfatal-errors -Wno-long-long

LD=$(CXX)
LDFLAGS=$(CXXOPTFLAGS) -L$(BOOSTDIR)/lib
LIBS=$(link_mode) -lboost_date_time -lboost_regex -liniphile

RM_F?=rm -f
INSTALL?=install
INSTALL_DIR?=$(INSTALL) -d
INSTALL_PROGRAM?=$(INSTALL) -s
RST2HTML?=$(call first_in_path,rst2html.py rst2html)

UNAME:=$(shell uname -s)

define first_in_path
  $(firstword $(wildcard \
    $(foreach p,$(1),$(addsuffix /$(p),$(subst :, ,$(PATH)))) \
  ))
endef

ifeq (MINGW,$(findstring MINGW,$(UNAME)))
dot_exe=.exe
link_mode=-Wl,-Bstatic
else
dot_exe=
endif
ifeq (static, $(LINK_MODE))
link_mode=-Wl,-Bstatic
endif

all: logdemux$(dot_exe)

clean:
	$(RM_F) logdemux.o logdemux$(dot_exe) tests/*/*.actual tests/*/*.diff README.html

install: all
	$(INSTALL_DIR) $(DESTDIR)$(BINDIR)
	$(INSTALL_DIR) $(DESTDIR)$(MAN1DIR)
	$(INSTALL_PROGRAM) logdemux$(dot_exe) $(DESTDIR)$(BINDIR)/logdemux$(dot_exe)
	$(INSTALL) logdemux.1 $(DESTDIR)$(MAN1DIR)/logdemux.1

check: all
	SHELL=$(SHELL) $(SHELL) rnt/run-tests.sh tests "$$PWD/logdemux"

%.html: %.rest
	$(RST2HTML) $< $@

logdemux$(dot_exe): logdemux.o
	$(LD) $(LDFLAGS) -o$@ $< $(LIBS)

logdemux.o: logdemux.cpp

.PHONY: all clean install
