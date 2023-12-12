/*
 * bdev.cxx
 * 
 * Copyright 2023 mike <mike@Fedora37>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <array>
#include <unordered_map>
#include <climits>
#include "../../C_C++/ToolKit/MillerRabin//MillerRabin.hxx"
 
#define  MASTER		0
#define  NODE		1

using namespace std;

struct Tdstruct {
	int id;
	uint64_t n,result;
};

int main (int argc, char *argv[])
{
	
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	
	struct Tdstruct tdb[10];
	MPI_Datatype Tdtype;	// distinguish from structure
	MPI_Datatype type[3] = {MPI_INT,MPI_UINT64_T,MPI_UINT64_T};
	int blocklen[3] = {1,1,1};
	MPI_Aint disp[3] = {0,4,12};
	
	MPI_Init(&argc, &argv);
		
	MPI_Type_create_struct(3, blocklen, disp, type, &Tdtype);
	MPI_Type_commit(&Tdtype); 
	
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	if(taskid == 0) {
		// setup the thread data blocks
		for(uint64_t i = 1; i != 10; ++i) {
			tdb[i] = {(int)i, i*10, 0};
			MPI_Send(&tdb[0]+i, 1, Tdtype, i, 123, MPI_COMM_WORLD);
		}
		
		
	} else { // Node
		MPI_Status status;
		Tdstruct local;
		MPI_Recv(&local, 1, Tdtype, 0, 123, MPI_COMM_WORLD, &status);
		local.result = local.n + taskid;
		cout << taskid << ") received data." << " result:" << local.result << endl;
		
	}
	MPI_Finalize();	// Eliminates -> make: *** [makefile:16: run] Error 1
	return 0;
}

