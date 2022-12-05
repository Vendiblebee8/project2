COURSE = cs320
SEMESTER = fall2022

# Update the CP_NUMBER if you adapt this makefile for future assignments
CP_NUMBER = 2

# Change the following to your own information
LASTNAME = Cataneo
GITUSERID = Vendiblebee8

# The name of the executable
EXE = cache-sim

# Name files according to CS240 naming conventions
REPODIR = ../$(COURSE)-$(SEMESTER)-cp$(CP_NUMBER)-$(GITUSERID)
TARFILE = CP$(CP_NUMBER)_$(LASTNAME)_$(USER)_$(GITUSERID).tar

FLAGS = -Wall -Wextra -g
# FLAGS = -Wall -O3
CC = g++
OBJ = obj


all: $(EXE)

run: t1 t2 t3

t1: cache-sim
	./cache-sim ./project2/traces/trace1.txt output1.txt

t2: cache-sim
	./cache-sim ./project2/traces/trace2.txt output2.txt

t3: cache-sim
	./cache-sim ./project2/traces/trace3.txt output3.txt

$(EXE): $(OBJ)/cache.o $(OBJ)/main.o
	$(CC) $(FLAGS) $(OBJ)/cache.o $(OBJ)/main.o -o $@

$(OBJ)/main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp -o $@

$(OBJ)/cache.o: cache.cpp cache.h
	$(CC) $(FLAGS) -c cache.cpp -o $@

clean:
	rm -f $(OBJ)/*.o $(EXE) *.tar.gz