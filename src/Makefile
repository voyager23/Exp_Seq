# Makefile for Exp_Seq/src folder

TARGET=asoln

$(TARGET): $(TARGET).cxx
	# Compile
	mpic++ -std=c++17 -Wall -Wno-unused-variable -Wno-comment -g -fpermissive $(TARGET).cxx -o ./$(TARGET)
	# Syncronize
	# ./sync_wdir.sh

nusrch : new_search.cxx
	g++ -std=c++17 -g new_search.cxx -o ./nusrch

.PHONY: run db

run :
	mpirun -np 8 -H localhost:8 ./$(TARGET)
	
db :
	gdb $(EXE)



