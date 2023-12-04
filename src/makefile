# Makefile for exp_seq folder

TARGET=ddev

.PHONY: run

$(TARGET): $(TARGET).cxx process.o
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive $(TARGET).cxx process.o -o ./$(TARGET)
	./sync_wdir.sh
	
process.o: process.cxx
	mpic++ -std=c++17 -Wall -c process.cxx

adev: adev.cxx
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive adev.cxx -o ./adev
	./sync_wdir.sh

run:
	mpirun -v -np 10 -H Aorus39:6,inteli7:4 ./$(TARGET)
