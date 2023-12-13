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

using namespace std;
//~ using u64=uint64_t;

// Declaration
bool MillerRabin(uint64_t n);



// Definitions
vector<uint64_t> prime_modulus(uint64_t x, uint64_t y){
	// approx 25 seconds for x=10^9 and y=10^7 returns 482449 values
	std::vector<uint64_t> primes = {};
	if((x % 2)==0) x += 1; // test odd values
	for(uint64_t p = x; p <= x+y; p+=2) {
		if (MillerRabin(p)) {
			if(p <= UINT_MAX){
				primes.push_back(p);
			} else {
			cout << "Warning: prime_modulus overflow" << endl;
			}
		}
	}
	return primes;
}

//======================================================================

int main (int argc, char *argv[])
{
	// Constants common to all nodes
	const int MAX_PROCS = 6 + 4 + 8 + (8 + 8 + 8 + 8);	// maximum number of cores available {50}
	const uint64_t n = 1e15;		// maximum number of iteration to calc. Example 1 n=10^3
	const uint64_t y = 1e3;
	const uint64_t x = 1e9;
	std::vector<uint64_t> primes;	// Referenced in Thread data block
	
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	int count[MAX_PROCS];
	int displace[MAX_PROCS];
	
	// setup vector of primes
	primes = prime_modulus(x,y);	// Expect 49 primes for example 2
	// distribution -> 6+7+6+6+6+6+6+6
	
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
			
		// master node process primes
		uint64_t root_B = 0;
	
		// ----------Cyclic Root search begins here----------
		
		const uint64_t block_size = 1000;
		std::vector<uint64_t> aseq;
		std::vector<uint64_t> blocks;
		// Main loop
		for(auto p : primes) {
			blocks = {1}; //a[1] = 1. Special case
			aseq = {1,19,2359,33412879};
			uint64_t a = 33412879;
			uint64_t i = 4;
			// calc and push a[5]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);
			// calc and push a[6]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);
			// calc and push a[7]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);	// Using a[7] as block start value
			do {
				a = (6*a*a + 10*a + 3) % p;
				aseq.push_back(a);
				i++;
				if((i % block_size)==1){ 
					blocks.push_back(a); // push_back a[nnn001]
				}
			}while(aseq.back() != aseq.at(6));	// compare to a[7]
			
			//~ for(auto b = aseq.begin(); b != aseq.begin() + 7; ++b)   cout << *b << " ";
			//~ cout << "  <>  ";
			//~ for(auto c = aseq.rbegin(); c != aseq.rbegin() + 7; ++c) cout << *c << " ";
			
			cout << "size: " << aseq.size();
			cout << "  order:" << aseq.size() - 7 << "  modulus:" << p;
			cout << "  blocks:" << blocks.size() << endl;
			// based on these variables find the index of a[n] 
			// contained in the finite field of size 'order'
			uint64_t order = aseq.size() - 7;
			uint64_t r = (n - 7) % order;
			uint64_t aidx = 7 + r;
			cout << "index of a[n] = " << aidx << endl;
			// recover the final answer a[n] mod p
			uint64_t bidx = aidx / block_size;
			a = blocks[bidx]; 
			i = (bidx * block_size) + 1;
			while(i != (aidx)){
				a = (6*a*a + 10*a + 3) % p;
				++i;
			}// a now has required value
			root_B += a;
			cout << "Result a[1000000000] = " << a << endl << endl;
		} // for p in primes
		
		// ----------Cyclic search ends here----------
	

		
		// Reduce_Sum the local sums
		uint64_t B = 0;

		MPI_Reduce( &root_B, &B, 1, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
		
		// Show final sum B;
		cout << "Final result B = " << B << endl;
		
	} else { // Node
		// Nodes expect to receive a vector of primes which is stored in nodeprime
		vector<uint64_t> nodeprime;
		nodeprime.resize(count[taskid]);	// make space for the primes
		int count_recv = count[taskid];		// set to expected receive count
		
		// Receive primes
		MPI_Scatterv(primes.data(), count, displace, MPI_UINT64_T,
		nodeprime.data(), count_recv,
		MPI_UINT64_T, 0, MPI_COMM_WORLD);
		
		// Work Node Process primes
		uint64_t local_B = 0;
	
		// ----------Cyclic Node search begins here----------
		
		const uint64_t block_size = 1000;
		std::vector<uint64_t> aseq;
		std::vector<uint64_t> blocks;
		// Main loop
		for(auto p : nodeprime) {
			blocks = {1}; //a[1] = 1. Special case
			aseq = {1,19,2359,33412879};
			uint64_t a = 33412879;
			uint64_t i = 4;
			// calc and push a[5]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);
			// calc and push a[6]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);
			// calc and push a[7]
			a = (6*a*a + 10*a + 3) % p;
			++i;
			aseq.push_back(a);	// Using a[7] as block start value
			do {
				a = (6*a*a + 10*a + 3) % p;
				aseq.push_back(a);
				i++;
				if((i % block_size)==1){ 
					blocks.push_back(a); // push_back a[nnn001]
				}
			}while(aseq.back() != aseq.at(6));	// compare to a[7]

			cout << "size: " << aseq.size();
			cout << "  order:" << aseq.size() - 7 << "  modulus:" << p;
			cout << "  blocks:" << blocks.size() << endl;
			// based on these variables find the index of a[n] 
			// contained in the finite field of size 'order'
			uint64_t order = aseq.size() - 7;
			uint64_t r = (n - 7) % order;
			uint64_t aidx = 7 + r;
			cout << "index of a[n] = " << aidx << endl;
			// recover the final answer a[n] mod p
			uint64_t bidx = aidx / block_size;
			a = blocks[bidx]; 
			i = (bidx * block_size) + 1;
			while(i != (aidx)){
				a = (6*a*a + 10*a + 3) % p;
				++i;
			}// a now has required value
			local_B += a;
			cout << "Result a[1000000000] = " << a << endl << endl;
		} // for p in primes
		
		// ----------Cyclic search ends here----------
	
		// Reduce_Sum the result package for this node
		MPI_Reduce( &local_B, NULL, 1, MPI_UINT64_T, MPI_SUM, 0, MPI_COMM_WORLD);
					
	} // else Node...
	
	// Required clean up.
	MPI_Finalize();
	// Required by C++ standard
	return 0;
}

