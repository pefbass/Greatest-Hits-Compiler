/******************************************************************************
 * Preston Fraser, pfraser
 * stringset.cpp
 *
 * Implements the functions outlined in stringset.h.
 *****************************************************************************/

#include <string>
#include <unordered_set>
#include <iomanip>

using namespace std;

#include "stringset.h"

unordered_set<string> stringset::set;

stringset::stringset() {
	set.max_load_factor(0.5);
}

stringset set;

// Insert a new string into the hash set and return a pointer to the string
// just inserted. If it is already there, nothing is inserted, and the
// previously-inserted string is returned.
const string* stringset::intern_stringset (const char* c){

	string s = c;

	auto pair = set.insert(s);
	return &(*pair.first);
}

// Dumps out the string set in debug format. In other words, print the hash
// header number followed by spaces, then the hash number and then the address
// of the string followed by the string itself.
// The input has been changed to an open file stream so a newly created file
// is usable for dumping.
void stringset::dump_stringset (ofstream* out){

	static unordered_set<string>::hasher hash_fn
				= stringset::set.hash_function();
	size_t max_bucket_size = 0;

	// Cycles through stringset buckets.
	for(unsigned int bucket = 0; bucket < set.bucket_count(); ++bucket){

		bool need_index = true;
		size_t curr_size = set.bucket_size(bucket);
		if(max_bucket_size < curr_size) max_bucket_size = curr_size;

		// Cycles through bucket, printing all strings it contains along
		// with their hash numbers, and memory address.
		for(auto i = set.cbegin(bucket); i != set.cend(bucket); ++i){

			if(need_index){
				*out << "hash[" << setw(4) << bucket << "]: ";
			} else {
				*out << "             ";
			}

			need_index = false;
			const string* s = &*i;
			*out << setw(22) << hash_fn(*s) << " " << s
				<< "->\"" << s->c_str() << "\"" << endl;
		}
	}

	// Some statistics:
	*out << "load_factor = " << set.load_factor() << endl;
	*out << "bucket_count = " << set.bucket_count() << endl;
	*out << "max_bucket_size = " << max_bucket_size << endl;
}
