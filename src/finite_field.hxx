/*
 * finite_field.hxx
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
//~ Called by all Nodes. The node is responsible for receiving a vector of primes 
//~ via MPI_scatterv and a limiting value for n as a global variable. 
//~ Calculates a[n] mod p for each prime and sums the result to a local value of B.
//~ Finally, returns the local sum B via MPI_Gather(sum) to the root node for
//~ final summation.

#ifndef __finite_field__
#define __finite_field__

	#include <iostream>
	#include <vector>
	#include <array>
	#include <unordered_map>
	#include <cstdint>
	#include <utility>
	using namespace std;
	uint64_t finite_field(std::vector<uint64_t> &nodeprime);
	
#endif

