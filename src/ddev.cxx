/*
 * ddev.cxx
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

vector<uint64_t>primes = {
100000007,100000037,100000039,100000049,100000073,100000081,100000123,100000127,100000193,100000213,\
100000217,100000223,100000231,100000237,100000259,100000267,100000279,100000357,100000379,100000393,\
100000399,100000421,100000429,100000463,100000469,100000471,100000493,100000541,100000543,100000561,\
100000567};

int main (int argc, char *argv[])
{
	
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	// This scatter method leaves the 'residue' elements, at the end of primes' un-scattered
	
	if(taskid == 0) {
		// setup scatter
		vector<uint64_t> local_p;

		int c = primes.size() / numtasks;
		int r = primes.size() % numtasks;
		// extend the receive buffer to include residue elements
		local_p.resize(c+r);
		// now add the residue elements
		for(auto i = 0; i != r; i++) local_p[i] = primes[i]; 	
		// finall scatter the remaining elements
		MPI_Scatter(primes.data()+r, primes.size() / numtasks, MPI_UINT64_T,\
		local_p.data()+r, primes.size() / numtasks, MPI_UINT64_T, 0, MPI_COMM_WORLD);
		
		//~ // debug printout
		//~ cout << taskid << ") received data. ";
		//~ for(auto i : local_p) cout << i << " ";
		//~ cout << endl;
		
		// using the received data calc the value of a[n]
		
	} else { // Node
		MPI_Status status;
		vector<uint64_t> nodeprime;
		nodeprime.resize(primes.size() / numtasks);
		MPI_Scatter(primes.data(), primes.size() / numtasks, MPI_UINT64_T,\
		nodeprime.data(), primes.size() / numtasks, MPI_UINT64_T, 0, MPI_COMM_WORLD);
				
		//~ cout << taskid << ") received data.";
		//~ for(auto i : nodeprime) cout << i << " ";
		//~ cout << endl;
		
		// using the received data calc the value of a[n]
		
	}
	MPI_Finalize();	// Eliminates -> make: *** [makefile:16: run] Error 1
	return 0;
}

