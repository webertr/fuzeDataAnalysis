CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib -ljpeg
HEADERS = include/holoParam.h

SOURCE = src/magnetic.c \
	src/main.c \
	src/getLFData.c \
	src/plot.c \
	src/DHI/abelInversion.c \
	src/DHI/imageAnalysis.c \
	src/DHI/readJPEG.c \
	src/plasmaParam.c \
	src/analysis.c \
	src/save.c \
	src/fit.c \
	src/accelTrack.c

PROD = run

all: $(PROD)

run: $(SOURCE) $(HEADERS)
	$(CC) $(FLAGS) $(INCL) \
	$(SOURCE) $(LIBRY) -o $(PROD)

clean:
	rm -f *~ *.o data/* $(PROD) ngspice/thyristorBank/*.txt \
	ngspice/thyristorBank/a.out
