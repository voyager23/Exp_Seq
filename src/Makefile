# Makefile for Exp_Seq/src folder

TARGET=asoln

$(TARGET): $(TARGET).cxx
	# Compile
	mpic++ -std=c++17 -Wall -Wno-unused-variable -Wno-comment -g -fpermissive $(TARGET).cxx millerrabin.o -o ./$(TARGET)
	# Syncronize
	# ./sync_wdir.sh

millerrabin.o : millerrabin.cxx
	g++ -c millerrabin.cxx -o millerrabin.o


.PHONY: run db

run :
	mpirun -np 1 -H localhost:8 ./$(TARGET)
	
db :
	gdb $(EXE)



