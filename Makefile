CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib -ljpeg

SOURCE = src/magneticData.c \
	src/main.c \
	src/getLFData.c \
	src/plot.c \
	src/abelInversionDHI.c \
	src/imageAnalysisDHI.c \
	src/readJPEG.c \
	src/testing.c

all: example

example: $(SOURCE)
	$(CC) $(FLAGS) $(INCL) \
	$(SOURCE) \
	$(LIBRY) -o example

clean:
	rm -f *~ *.o example
