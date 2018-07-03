CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib -lMdsShr -ljpeg

SRC_DIR = src
OBJ_DIR = obj
INCL_DIR = include

MKDIR = mkdir

SOURCE := src/magnetic.c \
	src/main.c \
	src/getLFData.c \
	src/plot.c \
	src/DHI/abelInversion.c \
	src/DHI/imageAnalysis.c \
	src/DHI/readJPEG.c \
	src/DHI/forceBalance.c \
	src/DHI/mdsplusAccess.c \
	src/plasmaParam.c \
	src/analysis.c \
	src/save.c \
	src/fit.c \
	src/accelTrack.c

OBJECT := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCE))
PROD = run

all: $(PROD)

run: $(OBJECT)
	$(CC) $(FLAGS) $(INCL) \
	$(OBJECT) $(LIBRY) -o $(PROD)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCL_DIR)/%.h
	@$(MKDIR) -p $(@D)
	$(CC) $(FLAGS) -c $(INCL) -o $@ $< 

clean:
	rm -rf *~ *.o data/* $(PROD) ngspice/thyristorBank/*.txt \
	ngspice/thyristorBank/a.out obj script/temp*

.PHONY: clean all
