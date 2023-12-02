/******************************************************************************
* FILE: adev.cxx
* DESCRIPTION:
*   Examine constructing and using Thread Data Block structures
*   Assume local group of 10 cores
* AUTHOR: M J TATE
* LAST REVISED: 01/12/23
******************************************************************************/
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

// Globals
static const int num_threads = 8;
std::vector<uint64_t> primes;	// Referenced in Thread data block

// Thread data block
typedef struct {
	int id;
	uint64_t n,result;
	vector<uint64_t> v_prime;
}tdb;

// Declarations
vector<u64> prime_modulus(u64 x, u64 y);
void thread_map_search(tdb *tdp);

// Definitions
vector<u64> prime_modulus(u64 x, u64 y){
	// approx 25 seconds for x=10^9 and y=10^7 returns 482449 values
	std::vector<u64> primes = {};
	if((x % 2)==0) x += 1; // test odd values
	for(u64 p = x; p <= x+y; p+=2) {
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
//----------------------------------------------------------------------

void thread_map_search(tdb *tdp) {
	// Calculate the value of a{n} for a range of moduli.
	// Sum each value to result variable.
	// map values of 'a' to their respective idx
	std::unordered_map<uint64_t, uint64_t> amapi;
	std::unordered_map<uint64_t, uint64_t> map_empty = {};
	// vector of sequential values of 'a' to enable recovery of the first index instance 
	std::vector<uint64_t> a_seq;
	std::vector<uint64_t> seq_empty = {};
	pair<std::unordered_map<uint64_t, uint64_t>::iterator, bool> result;	// emplace return value
	
	tdp->result = 0;
	for(size_t p = tdp->id; p < tdp->v_prime.size(); p += num_threads) {
		// Preload reverse map {imapa}
		a_seq = seq_empty;
		a_seq = {0,1,19};
		// Preload search map {amapi}
		amapi = map_empty; 
		amapi.emplace(1,1); 
		amapi.emplace(19,2);
		//~ cout << 1 << endl << 19 << endl;
		// extablish working variables
		uint64_t a = 19;  uint64_t idx = 2;
		// iterate values of 'a'
		while(1){
			a = (6*a*a + 10*a + 3) % tdp->v_prime[p];
			++idx;
			result = amapi.emplace(a,idx);
			if (get<bool>(result) == true) {
				a_seq.push_back(a);
				//~ cout << a << endl;
				continue;
			} else { //found match for 'a'
				//~ cout << a << endl;
				size_t head = get<1>( *(get<0>(result)));
				size_t order = idx - head;
				int64_t offset = (tdp->n - (head - 1)); // allow for negative offset values
				offset %= order;
				offset -= 1;
				if(offset < 0) offset += order;
				//~ cout << head+offset << endl;
				//~ for(auto i = a_seq.begin(); i != a_seq.end(); ++i) cout << *i << " ";
				//~ cout << endl;
				u64 an = a_seq[head + offset];
				// cout << "a[100000] mod " << tdp->v_prime[p] << " = " << an << "\torder: " << order << endl;
				tdp->result += an;
				goto NEXT_MODULUS;	// Jump to next prime modulus
			}		
		} // while(1)...
	NEXT_MODULUS: ;
	}	 
}
// ---------------------------------------------------------------------

//~ // Thread data block
//~ typedef struct {
	//~ size_t id;
	//~ uint64_t n,result;
//~ }tdb;

int main (int argc, char *argv[])
{
	
	int  numtasks, taskid, len, partner, message;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Datatype type[3] = {MPI_INT,MPI_INT,MPI_UINT64_T};
	primes = {2,3,5,7,11};

	vector<tdb> v_tdb;
	tdb temp = {0,0,0,primes};
	bool ready = false;
	
	if(taskid == MASTER) {
		for(int t = 0; t < numtasks; ++t){
			temp = {t,0,0,primes};
			v_tdb.push_back(temp);
		}
		ready = true;
		MPI_Bcast(&ready, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
	} else { // 1 <= NODE <= 9
		
		bool my_ready = 0;
		MPI_Bcast(&my_ready, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

	}
	return 0;	// As required by the C++ standard
}

