/*
 * cdev.cxx
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
#include <unistd.h>
#include "../../C_C++/ToolKit/MillerRabin//MillerRabin.hxx"
 
#define  MASTER		0
#define  NODE		1

using namespace std;



//~ vector<uint64_t>primes = {
//~ 100000007,100000037,100000039,100000049,100000073,100000081,100000123,100000127,100000193,100000213,\
//~ 100000217,100000223,100000231,100000237,100000259,100000267,100000279,100000357,100000379,100000393,\
//~ 100000399,100000421,100000429,100000463,100000469,100000471,100000493,100000541,100000543,100000561,\
//~ 100000567};

vector<uint64_t>primes = {	// pad vector with (NP - r) zeros
1000000007,1000000009,1000000021,1000000033,1000000087,1000000093,1000000097,1000000103,1000000123,1000000181,\
1000000207,1000000223,1000000241,1000000271,1000000289,1000000297,1000000321,1000000349,1000000363,1000000403,\
1000000409,1000000411,1000000427,1000000433,1000000439,1000000447,1000000453,1000000459,1000000483,1000000513,\
1000000531,1000000579,1000000607,1000000613,1000000637,1000000663,1000000711,1000000753,1000000787,1000000801,\
1000000829,1000000861,1000000871,1000000891,1000000901,1000000919,1000000931,1000000933,1000000993,1000001011,\
1000001021,1000001053,1000001087,1000001099,1000001137,1000001161,1000001203,1000001213,1000001237,1000001263,\
1000001269,1000001273,1000001279,1000001311,1000001329,1000001333,1000001351,1000001371,1000001393,1000001413,\
1000001447,1000001449,1000001491,1000001501,1000001531,1000001537,1000001539,1000001581,1000001617,1000001621,\
1000001633,1000001647,1000001663,1000001677,1000001699,1000001759,1000001773,1000001789,1000001791,1000001801,\
1000001803,1000001819,1000001857,1000001887,1000001917,1000001927,1000001957,1000001963,1000001969,1000002043,\
1000002089,1000002103,1000002139,1000002149,1000002161,1000002173,1000002187,1000002193,1000002233,1000002239,\
1000002277,1000002307,1000002359,1000002361,1000002431,1000002449,1000002457,1000002499,1000002571,1000002581,\
1000002607,1000002631,1000002637,1000002649,1000002667,1000002727,1000002791,1000002803,1000002821,1000002823,\
1000002827,1000002907,1000002937,1000002989,1000003009,1000003013,1000003051,1000003057,1000003097,1000003111,\
1000003133,1000003153,1000003157,1000003163,1000003211,1000003241,1000003247,1000003253,1000003267,1000003271,\
1000003273,1000003283,1000003309,1000003337,1000003351,1000003357,1000003373,1000003379,1000003397,1000003469,\
1000003471,1000003513,1000003519,1000003559,1000003577,1000003579,1000003601,1000003621,1000003643,1000003651,\
1000003663,1000003679,1000003709,1000003747,1000003751,1000003769,1000003777,1000003787,1000003793,1000003843,\
1000003853,1000003871,1000003889,1000003891,1000003909,1000003919,1000003931,1000003951,1000003957,1000003967,\
1000003987,1000003999,1000004023,1000004059,1000004099,1000004119,1000004123,1000004207,1000004233,0};

//~ ,1000004249

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
		
		// debug printout
		sleep(2);
		cout << taskid << ") local primes. " << c << " " << r << " ";
		for(auto i : local_p) cout << i << " ";
		cout << endl;
			
	} else { // Node
		MPI_Status status;
		vector<uint64_t> nodeprime;
		nodeprime.resize(primes.size() / numtasks);
		MPI_Scatter(primes.data(), primes.size() / numtasks, MPI_UINT64_T,\
		nodeprime.data(), primes.size() / numtasks, MPI_UINT64_T, 0, MPI_COMM_WORLD);		
		cout << taskid << ") received data.";
		for(auto i : nodeprime) cout << i << " ";
		cout << endl;		
	}
	MPI_Finalize();	// Eliminates -> make: *** [makefile:16: run] Error 1
	return 0;
}

