# Makefile for exp_seq folder

.PHONY: run

bdev: bdev.cxx
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive bdev.cxx -o ./bdev
	./sync_wdir.sh

adev: adev.cxx
	mpic++ -std=c++17 -Wall -Wno-unused-variable -g -fpermissive adev.cxx -o ./adev
	./sync_wdir.sh

run:
	mpirun -v -np 10 -H Aorus39:6,inteli7:4 ./bdev
