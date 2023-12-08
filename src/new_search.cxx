/*
 * new_search.cxx
 * 
 * Copyright 2023 Mike <mike@fedora38-2.home>
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


#include <iostream>
#include <vector>
#include <unordered_map>
#include <omp.h>
#include <iterator>
#include <utility>
#include "../inc/MillerRabin.hxx"

using u64 = uint64_t;
using u32 = uint32_t;
using namespace std;

// Definitions
vector<u64>prime_modulus(u64 x, u64 y){
	// approx 25 seconds for x=10^9 and y=10^7 returns 482449 values
	vector<u64> primes = {};
	if((x % 2)==0) x += 1; // test odd values
	for(u64 p = x; p <= x+y; p+=2) {
		if (MillerRabin(p)) {
				primes.push_back(p);
		}
	}
	return primes;
}

//----------------------------------------------------------------------

u64 simple_search(u64 x, u64 y, u64 n, u64 modulus = 0) {
	vector<u64> primes;
	if(modulus == 0) {
		primes = {\
		1000000007,1000000009,1000000021,1000000033,1000000087,1000000093,1000000097,1000000103,1000000123,1000000181,\
		1000000207,1000000223,1000000241,1000000271,1000000289,1000000297,1000000321,1000000349,1000000363,1000000403,\
		1000000409,1000000411,1000000427,1000000433,1000000439,1000000447,1000000453,1000000459,1000000483,1000000513,\
		1000000531,1000000579,1000000607,1000000613,1000000637,1000000663,1000000711,1000000753,1000000787,1000000801,\
		1000000829,1000000861,1000000871,1000000891,1000000901,1000000919,1000000931,1000000933,1000000993,1000001011,\
		1000001021,1000001053,1000001087,1000001099,1000001137,1000001161,1000001203,1000001213,1000001237,1000001263,\
		1000001269,1000001273,1000001279,1000001311,1000001329,1000001333,1000001351,1000001371,1000001393,1000001413,\
		1000001447,1000001449,1000001491,1000001501,1000001531,1000001537,1000001539,1000001581,1000001617,1000001621,\
		1000001633,1000001647,1000001663,1000001677,1000001699,1000001759,1000001773,1000001789,1000001791,1000001801,\
		1000001803,1000001819,1000001857,1000001887,1000001917,1000001927,1000001957,1000001963,1000001969,1000002043 };
	} else {
		primes = {modulus};
	}
	cout << "Simple search. Primes has " << primes.size() << " values." << endl;
	cout << primes.front() << " -> " << primes.back() << endl;
	
	u64 B = 0;
	for(u64 &p : primes) {
		u64 a = 1;	u64 idx = 1;
		while(idx < n) {
			idx += 1;
			a = (6*a*a + 10*a + 3) % p;
			//cout << idx << ": " << a << endl;
		} // while...
		cout << "a["<< n << "] mod " << p << " = " << a <<endl;
		B = (B + a);
	} // for...
	return B;
}


int main(int argc, char **argv)
{
	cout << simple_search(1e9, 1e7, 2e9, 0) << endl;
	
	return 0;
}

