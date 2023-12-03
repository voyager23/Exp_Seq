# Makefile for exp_seq folder

TARGET=cdev

.PHONY: run

$(TARGET): $(TARGET).cxx
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive $(TARGET).cxx -o ./$(TARGET)
	./sync_wdir.sh

adev: adev.cxx
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive adev.cxx -o ./adev
	./sync_wdir.sh

run:
	mpirun -v -np 10 -H Aorus39:6,inteli7:4 ./$(TARGET)
