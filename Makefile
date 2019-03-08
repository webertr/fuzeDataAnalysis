# C Definitions
CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib -lMdsShr -ljpeg

SRC_DIR = src
OBJ_DIR = obj
INCL_DIR = include

# C++ Definitions
CCP = g++


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
	src/DHI/neutronProduction.c \
	src/plasmaParam.c \
	src/analysis.c \
	src/save.c \
	src/fit.c \
	src/pinchAnalysis.c \
	src/magneticModel.c \
	src/regExpSub.c \
	src/accelTrack.c \
	src/mcnp/readMeshTally.c \
	src/bankCheck.c

SOURCE_CPP := src/cpp/main.cpp

OBJECT := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCE))

OBJECT_CPP := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_CPP))

PROD := exec \
	run

all: $(PROD)

run: $(OBJECT)
	$(CC) $(FLAGS) $(INCL) \
	$(OBJECT) $(LIBRY) -o run

exec: $(OBJECT_CPP)
	$(CCP) $(OBJECT_CPP) -o exec

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCL_DIR)/%.h
	@$(MKDIR) -p $(@D)
	$(CC) $(FLAGS) -c $(INCL) -o $@ $< 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCL_DIR)/%.h
	@$(MKDIR) -p $(@D)
	$(CCP) $(FLAGS) -c $(INCL) -o $@ $< 


clean:
	rm -rf *~ *.o data/* $(PROD) ngspice/thyristorBank/*.txt \
	ngspice/thyristorBank/a.out obj script/temp*

.PHONY: clean all
