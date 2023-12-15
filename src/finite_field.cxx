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
const uint64_t n = 1e6;
const uint64_t taskid = 8;

void finite_field(std::vector<uint64_t> &nodeprime)
{
	// Version: 14.12.2023 18:22:21
	//TEST
	typedef std::pair< uint64_t *, bool> Rval;
	
	// END
	
	uint64_t a6, a7, a, idx, local_b=0;
	const uint64_t block_size = 1000;	// record progress every 1000 values
	std::unordered_map<uint64_t,uint64_t> progress;
	
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
			uint64_t residue = (n - 6) % p;
			int64_t offset = residue - 1;
			if(offset < 0) offset += order;
			uint64_t req_idx = 7 + offset;
			// refer to progress map
			idx = (req_idx / block_size) * block_size;
			if(idx==0) {
				idx = 3;  // progress has (3,2359)
			} else {
				a = (*(progress.find(idx))).second;
			}
			while(idx != req_idx){
				a = (6*a*a + 10*a + 3) % p;
				++idx;
			}
			// a now has the value of a{n}
			local_b += a;
			cout << "a[" << n << "] mod " << p << " = a " << endl;
		} // Match: a[idx] => a7
		
	} // for prime:nodename				
}

int main()
{
	std::vector<uint64_t> primes = {10007,17623,10891,21589,99901,99991};
	finite_field(primes);
	
}
