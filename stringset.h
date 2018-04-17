/******************************************************************************
 * Preston Fraser, pfraser
 * stringset.h
 *
 * Interface to the string set.
 *****************************************************************************/

#ifndef __STRING_SET__
#define __STRING_SET__

#include <string>
#include <unordered_set>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

struct stringset {
	stringset();
	static unordered_set<string> set;
	static const string* intern_stringset (const char*);
	static void dump_stringset (ofstream*);
};

#endif
