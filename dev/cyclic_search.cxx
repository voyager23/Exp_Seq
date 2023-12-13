	
	//~ Input parameter vector of primes, x, y, n
	
	//~ Output TBN
	
	// ----------Cyclic search begins here----------
	
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
		cout << "Result a[1000000000] = " << a << endl << endl;
	} // for p in primes
	
	// ----------Cyclic search ends here----------
	
