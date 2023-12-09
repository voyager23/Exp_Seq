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

vector<uint64_t>primes = {	// 20 primes for 6+4+8 cores
	1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,
	1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007,1000000007};


int main (int argc, char *argv[])
{
	const int MAX_PROCS = 6+4+8+8+8+8;	// maximum number of cores available {42}
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	// Define a contiguous data_type of uint64_t (taskid, modulus, a[n]);
	MPI_Datatype ResPack;
	MPI_Type_contiguous(3, MPI_UINT64_T, &ResPack);	// (taskid, prime, An%prime)
	MPI_Type_commit(&ResPack);
	
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
		// process not actioned
		// Receive and publish

		vector<array<uint64_t,3>> v_results;
		for(int t = 1; t != numtasks; ++t){
			array<uint64_t,3> buffer;
			MPI_Recv(&buffer, 1, ResPack, t, 321, MPI_COMM_WORLD, &status);
			v_results.push_back(buffer);
			// cout << "Result Taskid: " << respack[0] << " " << respack[1] << " " << respack[2] << endl;			
		}
		for(auto &r : v_results)
			cout << "result vector " << r[0] << " " << r[1] << " " << r[2] << endl;
		
	} else { // Node
		
		MPI_Status status;
		vector<uint64_t> nodeprime;
		uint64_t respack[3];	// mirror MPI_Type
		nodeprime.resize(count[taskid]);
		int count_recv = nodeprime.size();	// set to expected receive count
		
		// Receive
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		nodeprime.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		// Process
		
		const uint64_t n = 1000000;
		uint64_t a = 1;
		uint64_t idx = 1;
		for(uint64_t &p : nodeprime) {
			while(idx < n) {
				idx += 1;
				a = (6*a*a + 10*a + 3) % p;
			} // while...
			//cout << "a["<< n << "] mod " << p << " = " << a <<endl;
			// Form the ResPack here
			respack[0] = taskid;
			respack[1] = p;
			respack[2] = a;
		} // for...
		// cout << "Taskid: " << respack[0] << " " << respack[1] << " " << respack[2] << endl;
		// send package to root;
		// int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
		MPI_Send(respack, 1, ResPack, 0, 321, MPI_COMM_WORLD);		
	}
	
	// Clean up.
	MPI_Type_free(&ResPack);
	// This step eliminates makefile error msg. "make: *** [makefile:16: run] Error 1"
	MPI_Finalize();	
	return 0;
}

