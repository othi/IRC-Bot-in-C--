#############################################################################
# Makefile for building: ircbot
# Generated by qmake (2.01a) (Qt 4.7.0) on: Fri May 13 15:33:36 2011
# Project:  ircbot.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile ircbot.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES) -g
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES) -g
LINK          = g++
LFLAGS        = -Wl,-O1 -g
LIBS          = $(SUBLIBS)  -L/usr/lib -lpthread 
AR            = ar cqs
RANLIB        = 
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ircbot.cpp \
		main.cpp 
OBJECTS       = ircbot.o \
		main.o
DESTDIR       = 
TARGET        = ircbot

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

clean: 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

####### Compile

ircbot.o: ircbot.cpp ircbot.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ircbot.o ircbot.cpp

main.o: main.cpp main.h \
		ircbot.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

