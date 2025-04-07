#
# Març 2021
# Modificacions per a poder coexistir en OpencV 4.2 i 3.2
#
# Març 2019
# Per si no està instalat pkg-config
# OpenAL
# openal --modversion
# 1.18.2
# pkg-config openal --cflags
# -I/usr/include/AL --> al.h i alc.h
# pkg-config openal --libs
# -lopenal
# ALUT 1.1 ff
# en /usr/include/AL
# Tammé alure, efx* i alext

CC=g++

#no depurar, por defecto
CCFLAGS=
#para depurar
#CCFLAGS=-g 

#CFLAGS=`pkg-config openal--cflags` 
#CFLAGS=`pkg-config --cflags opencv --libs opencv`
#LIBS=`pkg-config openal --libs` -lalut

OPENCV_VERSION = `pkg-config --modversion opencv4`
ifeq ($(OPENCV_VERSION), )
	CFLAGS=`pkg-config --cflags freealut opencv --cflags`
	LIBS=`pkg-config freealut opencv --libs`
else
	CFLAGS=`pkg-config --cflags freealut opencv4 --cflags`
	LIBS=`pkg-config freealut opencv4 --libs`
endif


BINARI=discoCam

all: discoCam

discoCam: $(BINARI).cpp
	$(CC) $(CCFLAGS) $(BINARI).cpp -o $(BINARI) $(CFLAGS) $(LIBS)


clean:
	rm instrumentVirtual.zip $(BINARIS)

zip:
	zip -r instrumentVirtual.zip Makefile discoCam.cpp 

