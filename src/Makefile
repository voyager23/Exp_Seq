# Makefile for OpenMP folder

#TARGET=mpi_search
TARGET=edev

$(TARGET): $(TARGET).cxx
	# Compile
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive $(TARGET).cxx -o ./$(TARGET)
	# Syncronize
	./sync_wdir.sh

nusrch : new_search.cxx
	g++ -std=c++17 -g new_search.cxx -o ./nusrch

.PHONY: run db

run :
	mpirun -np 10 -H Aorus39:6,Inteli7:4 ./$(TARGET)
	
db :
	gdb $(EXE)



