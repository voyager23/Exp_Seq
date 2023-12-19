/*
 * finite_field.cxx
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


#include "finite_field.hxx"

// Dummy values
const uint64_t n = 1e15;
const uint64_t taskid = 8;

uint64_t finite_field(std::vector<uint64_t> &nodeprime)
{
	// Version: 16.12.2023 17:12:33
	
	uint64_t a6, a7, a, idx, local_b=0;
	const uint64_t block_size = 1000;	// record progress every 1000 values
	std::unordered_map<uint64_t,uint64_t> progress;
	std::unordered_map<uint64_t,uint64_t>::iterator iter;
	
	for(uint64_t p : nodeprime) {
		// Given values for a[3]
		a = 2359; idx = 3;
		// calc a[4]
		a = (6*a*a + 10*a + 3) % p;
		++idx;
		// calc a[5]
		a = (6*a*a + 10*a + 3) % p;
		++idx;
		
		// calc and save a[6]
		a6 = (6*a*a + 10*a + 3) % p;
		++idx;
		// calc and save a[7]
		a7 = (6*a6*a6 + 10*a6 + 3) % p;
		++idx;
		
		// index and a7 valid. Recursively calc values for (a mod p) until a match is found.
		a = a7;
		progress.clear();
		progress.emplace(3,2359);
		
		do {
			a = (6*a*a + 10*a + 3) % p;
			if (((++idx)% block_size) == 0) progress.emplace(idx,a); //  progress has (4000, a) etc
		} while ((a != a7)&&(idx <= n));
		
		if(idx > n){
			std::cout<<"Error: idx > n in taskid: " << taskid << endl;
			exit(1);
		} else { // Match: a[idx] => a7
			uint64_t order = idx - 7;
			uint64_t residue = (n - 6) % order;
			int64_t offset = residue - 1;
			if(offset < 0) offset += order;
			uint64_t req_idx = 7 + offset;
			
			// refer to progress map
			idx = (req_idx / block_size) * block_size;			
			if(idx==0) {	// special case
				idx = 3;	// progress has (3,2359)
			} else {
				iter = progress.find(idx);
				if(iter == progress.end()) {
					cout<<"entry not found in progress"<<endl;
					exit(1);
				}

			}
			// reset value of a from progress map			
			a = (*iter).second;			
			// ----------------
			while(idx != req_idx){
				a = (6*a*a + 10*a + 3) % p;
				++idx;
			}
			// a now has the value of a{n}
			local_b += a;
			cout << "a[" << n << "] mod " << p << " = " << a << endl;
		} // Match: a[idx] => a7
		
	} // for prime:nodename
	cout << "local_b = " << local_b << endl;
	return local_b;			
}

int main()
{
	
	std::vector<uint64_t> primes = 
	{ 1000000007,1000000009,1000000021,1000000033,1000000087,1000000093,1000000097,1000000103,1000000123,1000000181,
	1000000207,1000000223,1000000241,1000000271,1000000289,1000000297,1000000321,1000000349,1000000363,1000000403,
	1000000409,1000000411,1000000427,1000000433,1000000439,1000000447,1000000453,1000000459,1000000483,1000000513,
	1000000531,1000000579,1000000607,1000000613,1000000637,1000000663,1000000711,1000000753,1000000787,1000000801,
	1000000829,1000000861,1000000871,1000000891,1000000901,1000000919,1000000931,1000000933,1000000993 };
	
	primes = {1000000007};	//debug value only
	
	cout << finite_field(primes) << endl;
	
}
