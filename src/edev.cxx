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

// Global Costants

const int MAX_PROCS = 6+4+8 + 8 + 8 + 8 + 8;	// maximum number of cores available {50}

const uint64_t n = 1000;		// maximum number of iteration to calc. Example 1 n=10^3

const vector<uint64_t>primes = {	// 49 primes for 6+4 cores => 10*4 + 9 -> (4,5,5,5,5,5,5,5,5,5)
1000000007,1000000009,1000000021,1000000033,1000000087,1000000093,1000000097,1000000103,1000000123,1000000181,
1000000207,1000000223,1000000241,1000000271,1000000289,1000000297,1000000321,1000000349,1000000363,1000000403,
1000000409,1000000411,1000000427,1000000433,1000000439,1000000447,1000000453,1000000459,1000000483,1000000513,
1000000531,1000000579,1000000607,1000000613,1000000637,1000000663,1000000711,1000000753,1000000787,1000000801,
1000000829,1000000861,1000000871,1000000891,1000000901,1000000919,1000000931,1000000933,1000000993};

void verify_results(vector<array<uint64_t,3>> &vr);
void verify_results(vector<array<uint64_t,3>> &vr){
	// each result contains taskid, modulus and a[n]
	// output line taskid, modulus a[n] {no-match} calculated
	uint64_t a,idx;
	for(array<uint64_t,3> &r : vr) {
		a = 1;	idx = 1;
		while(idx < n) {
			idx += 1;
			a = (6*a*a + 10*a + 3) % r[1];
		} // while...		
		cout << "a["<< n << "] mod " << r[1] << " = " << a;
		if(a == r[2]){
			cout << " match " << endl;;
		} else {
			cout << "no-match " << endl;
		}
	}
}


//======================================================================

int main (int argc, char *argv[])
{
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
		cout << "n = " << n << endl;
		cout << "Tasks/node: ";
		for(auto i=0; i != numtasks; ++i) cout << count[i] << " ";
		cout << endl;
		cout << "Displacements: ";
		for(auto i=0; i != numtasks; ++i) cout << displace[i] << " ";
		cout << endl;	
		// END DEBUG
		
		#// Local result storage
		vector<array<uint64_t,3>> v_results;
		// Scatter
		local_p.resize(count[taskid]);
		int count_recv = local_p.size();
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		local_p.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		// process
		array<uint64_t,3> buffer;
		buffer[0] = taskid;
		buffer[1] = 0;	// prime not used
		buffer[2] = 0;
		
		for(int i = 0; i != count[0]; ++i) {
			uint64_t a = 1;
			uint64_t idx = 1;
			while(idx < n) {
				idx += 1;
				a = (6*a*a + 10*a + 3) % primes[i];
			} // while...
			buffer[2] += a;
		} // for(i ...		
		
		// Reduce_Sum the process sums
		uint64_t B = 0;

		MPI_Reduce( &(buffer[2]), &B, 1, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
		
		// Show final sum B;
		cout << "B = " << B << endl;
		
	} else { // Node
		#define BUFSIZE (count[taskid] * 3 * sizeof(MPI_UINT64_T))
		uint64_t buf[BUFSIZE];
		MPI_Buffer_attach(buf,BUFSIZE);
		MPI_Status status;
		vector<uint64_t> nodeprime;
		uint64_t respack[3];	// mirror MPI_Type
		nodeprime.resize(count[taskid]);
		int count_recv = nodeprime.size();	// set to expected receive count
		
		// Receive primes
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		nodeprime.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		// Process
		respack[0] = taskid;	// Single send/gather for each node
		respack[1] = 0;			// prime not used
		respack[2] = 0;			// local sum
		for(uint64_t &p : nodeprime) {
			uint64_t a = 1;
			uint64_t idx = 1;
			while(idx < n) {
				idx += 1;
				a = (6*a*a + 10*a + 3) % p;
			} // while...
			// Update the ResPack here
			respack[2] += a;
		} // for...
		// Reduce_Sum the result package for this node
		MPI_Reduce( &(respack[2]), NULL, 1, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);			
	} // else...
	
	// Clean up.
	MPI_Type_free(&ResPack);
	// This step eliminates makefile error msg. "make: *** [makefile:16: run] Error 1"
	MPI_Finalize();	
	return 0;
}

