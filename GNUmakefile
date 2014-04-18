# vim: et sw=2 sts=2 ts=8

PREFIX ?=         /usr/local
BINDIR ?=         $(PREFIX)/bin
MANDIR ?=         $(PREFIX)/man
MAN1DIR ?=        $(MANDIR)/man1

BOOSTDIR ?=       $(PREFIX)

CPPFLAGS =        -I$(BOOSTDIR)/include
CXXFLAGS =        $(CXXSTD) $(CXXOPTFLAGS) $(CXXWFLAGS)
CXXSTD =          -std=c++0x -pedantic
CXXOPTFLAGS =     -g -O2
CXXWFLAGS =       -Wall -Wextra -Wfatal-errors -Wno-long-long

LD =              $(CXX)
LDFLAGS =         $(CXXOPTFLAGS) -L$(BOOSTDIR)/lib
LIBS =            $(link_mode) -lboost_date_time -lboost_regex -liniphile

RM_F ?=           rm -f
INSTALL ?=        install
INSTALL_DATA ?=   $(INSTALL) -m 0644
INSTALL_DIR ?=    $(INSTALL) -d
INSTALL_PROGRAM ?=$(INSTALL) -s
RST2HTML ?=       $(call first_in_path,rst2html.py rst2html)
GZIP ?=           gzip

UNAME :=          $(shell uname -s)

define first_in_path
  $(firstword $(wildcard \
    $(foreach p,$(1),$(addsuffix /$(p),$(subst :, ,$(PATH)))) \
  ))
endef

ifeq (static, $(LINK_MODE))
link_mode =       -Wl,-Bstatic
endif
ifeq (MINGW,$(findstring MINGW,$(UNAME)))
dot_exe =         .exe
link_mode =       -Wl,-Bstatic
endif

name =          logdemux

artifacts =     $(name)$(dot_exe) $(name).1.gz

all: $(artifacts)
	@touch .built

clean:
	$(RM_F) .built $(artifacts) $(name).o tests/*/*.actual tests/*/*.diff README.html

install: .built
	$(INSTALL_DIR) $(DESTDIR)$(BINDIR)
	$(INSTALL_DIR) $(DESTDIR)$(MAN1DIR)
	$(INSTALL_PROGRAM) $(name)$(dot_exe) $(DESTDIR)$(BINDIR)/$(name)$(dot_exe)
	$(INSTALL_DATA) $(name).1.gz $(DESTDIR)$(MAN1DIR)/$(name).1.gz

check: all
	SHELL=$(SHELL) $(SHELL) rnt/run-tests.sh tests "$$PWD/$(name)"

%.1.gz: %.1
	$(GZIP) < $< > $@

%.html: %.rest
	$(RST2HTML) $< $@

$(name)$(dot_exe): $(name).o
	$(LD) $(LDFLAGS) -o$@ $< $(LIBS)

.built:
	@printf "%s\n" '' "ERROR: run '$(MAKE) all' first." '' >&2
	@false

.PHONY: all clean install
