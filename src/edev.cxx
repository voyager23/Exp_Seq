/*
 * edev.cxx
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
#include <cstdint>
 #include "../inc/process.hxx"

#define  MASTER		0
#define  NODE		1

using namespace std;

vector<uint64_t>primes = {
100000007,100000037,100000039,100000049,100000073,100000081,100000123,100000127,100000193,100000213};


//~ 100000217,100000223,100000231,100000237,100000259,100000267,100000279,100000357,100000379,100000393,\
//~ 1000000409,1000000411,1000000427,1000000433,1000000439,1000000447,1000000453,1000000459,1000000483,1000000513,\
//~ 1000000531,1000000579,1000000607,1000000613,1000000637,1000000663,1000000711,\
//~ 100000399,100000421,100000429,100000463,100000469,100000471,100000493,100000541,100000543};

//~ Different way to distribute the primes using MPI_Scatterv
//~ int MPI_Scatterv(const void* buffer_send,
                 //~ const int counts_send[],
                 //~ const int displacements[],
                 //~ MPI_Datatype datatype_send,
                 
                 //~ void* buffer_recv,
                 //~ int count_recv,
                 
                 //~ MPI_Datatype datatype_recv,
                 //~ int root,
                 //~ MPI_Comm communicator);
int main (int argc, char *argv[])
{
	const int MAX_PROCS = 6+4+8+8+8+8;	// maximum number of cores available {42}
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	int count[MAX_PROCS];
	int displace[MAX_PROCS];
	
	// setup scatterv
	vector<uint64_t> local_p;
	int c = primes.size() / numtasks;
	int r = primes.size() % numtasks;
	// count array
	for(auto i = 0; i != numtasks; ++i) count[i] = c;	// basic counts for each process
	for(auto i = 1, residue = r; residue != 0; ++i, --residue) count[i] += 1; // add residues
	// displace array;
	displace[0] = 0;
	for(auto i = 0, j = 1; i != numtasks; ++i,++j) displace[j] = count[i] + displace[j-1];
	
	if(taskid == 0) {	
		// DEBUG
		cout << primes.size() << " primes." << endl;
		for(auto i=0; i != numtasks; ++i) cout << count[i] << " ";
		cout << endl;
		for(auto i=0; i != numtasks; ++i) cout << displace[i] << " ";
		cout << endl;	
		// END DEBUG
		
		// Scatter
		local_p.resize(count[taskid]);
		int count_recv = local_p.size();
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		local_p.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		// process
		
	} else { // Node
		
		MPI_Status status;
		vector<uint64_t> nodeprime;
		nodeprime.resize(count[taskid]);
		int count_recv = nodeprime.size();
		
		// Receive
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		nodeprime.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		// Process
		
	}
	
	MPI_Finalize();	// This eliminates makefile error msg. "make: *** [makefile:16: run] Error 1"
	return 0;
}

