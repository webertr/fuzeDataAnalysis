# C Definitions
CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lxml2 -lm -L$(MDSPLUS_DIR)/lib -lMdsLib -lMdsShr -ljpeg


# C++ Definitions
CCP = g++

INCL_CPP = -Iinclude -I$(MDSPLUS_DIR)/include -I/usr/include/libxml2 -I$(EPICS_BASE)/include \
	-I$(EPICS_BASE)/include/os/Linux -I$(EPICS_BASE)/include/compiler/gcc
FLAGS_CPP = -g -Wall
LIBRY_CPP = -lgsl -lgslcblas -lxml2 -L$(MDSPLUS_DIR)/lib -lMdsObjectsCppShr \
	-L$(EPICS_BASE)/lib/$(EPICS_HOST_ARCH) -lCom -lca -lpqxx -lpq


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

SOURCE_CPP := src/cpp/main.cpp \
	src/cpp/plot.cpp \
	src/cpp/mdsplusAccess.cpp \
	src/cpp/analysis.cpp \
	src/cpp/Kirana.cpp \
	src/cpp/magnetic.cpp \
	src/cpp/LightField.cpp \
	src/cpp/epicsCA.cpp \
	src/cpp/psqlAccess.cpp \
	src/cpp/interferometry.cpp \
	src/cpp/test.cpp

SOURCE_SPECTROSCOPY_CPP := src/cpp/spectroscopyMonitor.cpp \
	src/cpp/mdsplusAccess.cpp \
	src/cpp/epicsCA.cpp \
	src/cpp/psqlAccess.cpp \
	src/cpp/plot.cpp \
	src/cpp/LightField.cpp

OBJECT := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCE))

OBJECT_CPP := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_CPP))

OBJECT_SPECTROSCOPY_CPP := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCE_SPECTROSCOPY_CPP))

PROD := exec \
	run \
	spectroscopy

all: $(PROD)

run: $(OBJECT)
	$(CC) $(FLAGS) $(INCL) \
	$(OBJECT) $(LIBRY) -o run

exec: $(OBJECT_CPP)
	$(CCP) $(FLAGS_CPP) $(INCL_CPP) \
	$(OBJECT_CPP) $(LIBRY_CPP) -o exec

spectroscopy: $(OBJECT_SPECTROSCOPY_CPP)
	$(CCP) $(FLAGS_CPP) $(INCL_CPP) \
	$(OBJECT_SPECTROSCOPY_CPP) $(LIBRY_CPP) -o spectroscopy

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCL_DIR)/%.h
	@$(MKDIR) -p $(@D)
	$(CC) $(FLAGS) -c $(INCL) -o $@ $< 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCL_DIR)/%.h
	@$(MKDIR) -p $(@D)
	$(CCP) $(FLAGS) -c $(INCL_CPP) -o $@ $< 

clean:
	rm -rf *~ *.o data/* $(PROD) ngspice/thyristorBank/*.txt \
	ngspice/thyristorBank/a.out obj script/temp*

install: 
	install spectroscopy /usr/local/bin

uninstall: 
	rm /usr/local/bin/spectroscopy

.PHONY: clean all
