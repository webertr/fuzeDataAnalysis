CC = gcc
INCL = -Iinclude -I$(MDSPLUS_DIR)/include
FLAGS = -g -Wall
LIBRY = -lgsl -lgslcblas -lm -L$(MDSPLUS_DIR)/lib -lMdsLib

all: example

example: src/dataAnalysis.c src/main.c
	$(CC) $(FLAGS) $(INCL) \
	src/dataAnalysis.c src/main.c \
	$(LIBRY) -o example

clean:
	rm -f *~ *.o example
