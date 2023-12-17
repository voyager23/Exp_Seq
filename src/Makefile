# Makefile for Exp_Seq/src folder

TARGET=mpi_solution
PROCS ?= 12

$(TARGET): $(TARGET).cxx
	# Compile
	mpic++ -std=c++17 -Wall -Wno-unused-variable -Wno-comment -g -fpermissive $(TARGET).cxx millerrabin.o -o ./$(TARGET)
	# Syncronize
	# ./sync_wdir.sh

millerrabin.o : millerrabin.cxx
	g++ -c millerrabin.cxx -o millerrabin.o

.PHONY: run db

run :
	mpirun -np $(PROCS) -H localhost:12 ./$(TARGET)
	
db :
	gdb $(EXE)



