#include <iostream>
 #include <thread>
 #include <unordered_map>
 #include <vector>
 #include <array>
 #include <cstdint>
 #include <climits>
 #include "../inc/process.hxx"
 
using u64 = uint64_t;
using u32 = uint32_t;
using u128 = __uint128_t;
using namespace std;
// Declarations
u64 binpower(u64 base, u64 e, u64 mod);
bool check_composite(u64 n, u64 a, u64 d, int s);
vector<u64> prime_modulus(u64 x, u64 y);
// Definitions
u64 binpower(u64 base, u64 e, u64 mod) {
	u64 result = 1;
	base %= mod;
	while (e) {
		if (e & 1)
			result = (u128)result * base % mod;
		base = (u128)base * base % mod;
		e >>= 1;
	}
	return result;
}

bool check_composite(u64 n, u64 a, u64 d, int s) {
	u64 x = binpower(a, d, n);
	if (x == 1 || x == n - 1)
		return false;
	for (int r = 1; r < s; r++) {
		x = (u128)x * x % n;
		if (x == n - 1)
			return false;
	}
	return true;
}

bool MillerRabin(u64 n) { // returns true if n is prime, else returns false.
	if (n < 2)
		return false;

	int r = 0;
	u64 d = n - 1;
	while ((d & 1) == 0) {
		d >>= 1;
		r++;
	}

	for (u64 a : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
		if (n == a)
			return true;
		if (check_composite(n, a, d, r))
			return false;
	}
	return true;
}

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

int function() {
	
	std::vector<uint64_t> primes;	// Referenced in Thread data block
	
	const uint64_t x = 1000000;		// 1e6
	const uint64_t y =  1000;		// 1e3
	const uint64_t n = 1000000;		// 1e6

	cout << "Calculating primes..." << endl;
	primes = prime_modulus(x,y);
	
	//~ DEBUG
	primes = {1000981};
	//~ end debug
		
	cout << primes.size() << " primes. ";
	cout << primes.front() << " => " << primes.back() << endl;
	
	
	std::vector<uint64_t> aseq;
	std::vector<uint64_t> blocks;
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
			if((i % 1000)==1){ 
				blocks.push_back(a); // push_back a[nnn001]
			}
		}while(aseq.back() != aseq.at(6));	// compare to a[7]
		
		for(auto b = aseq.begin(); b != aseq.begin() + 7; ++b)   cout << *b << " ";
		cout << "  <>  ";
		for(auto c = aseq.rbegin(); c != aseq.rbegin() + 7; ++c) cout << *c << " ";
		cout << " size: " << aseq.size() << endl;
		cout << "order:" << aseq.size() - 7 << " modulus:" << p << endl;
		cout << "blocks:" << blocks.size() << endl;
		// based on these variables find the index of a[n] 
		// contained in the finite field of size 'order'
		uint64_t order = aseq.size() - 7;
		uint64_t r = (n - 7) % order;
		uint64_t aidx = 7 + r;
		cout << "index of a[n] = " << aidx << endl;
		// recover the final answer a[100000] mod p
		// a[2001] in blocks[2]
		uint64_t bidx = aidx / 1000;
		a = blocks[bidx]; // a[2001]
		i = (bidx * 1000) + 1; // 2001
		while(i != (aidx)){
			a = (6*a*a + 10*a + 3) % p;
			++i;
		}// a now has required value
		cout << "a[1000000] = " << a << endl;
		
		// Do a simple search based on specific values to confirm the result
		// cout << simple_search(x,y,n) << endl;
	}
		
	 return 0;
 }
