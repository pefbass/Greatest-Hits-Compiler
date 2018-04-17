/******************************************************************************
 * Preston Fraser, pfraser
 * oc.cpp
 *
 * Compiler
 *****************************************************************************/

#include <assert.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "string.h"
#include "stringset.h"
#include "auxlib.h"

using namespace std;

const string CPP = "/usr/bin/cpp ";
constexpr size_t LINESIZE = 1024;

/*
// Switch to debug mode for yylex().
void debug_yylex(){
	yy_flex_debug = 1;
}

// Switch to debug mode for yyparse().
void debug_yyparse(){
	yydebug = 1;
}
*/

// Chomp the last character from a buffer if it is delim.
void chomp(char* c, char delim){

	size_t len = strlen(c);
	if(len == 0) return;
	char* nl_pos = c + len - 1;
	if(*nl_pos == delim) *nl_pos = '\0';

}

// Run cpp against the lines of the file.
void cpplines(FILE* pipe, const char* filename){

	int line_num = 1;
	char input[LINESIZE];
	strcpy(input, filename);
	for(;;){
		char buffer[LINESIZE];
		char* fcontents = fgets(buffer, LINESIZE, pipe);
		if(fcontents == nullptr) break;
		chomp(buffer, '\n');	// Cuts fcontents into line segments.
		char* savepos = nullptr;
		char* bufptr = buffer;
		for(int tokenct = 1;; ++tokenct){

			char* token = strtok_r(bufptr, " \t\n", &savepos);
			bufptr = nullptr;
			if(token == nullptr) break;
			stringset::intern_stringset(token);

		}
		++line_num;

	}
}

int main (int argc, char** argv) {

	// Command: "oc [-ly] [-@flag ...] [-D string] program.oc"
	const string execname = basename (argv[0]);
	string compflag = "";
	int any_flags = 0;

	// Setting default debug modes: OFF
	int yy_flex_debug = 0;
	int yydebug = 0;

	while((any_flags = getopt(argc, argv, "@:ly:D:")) != -1){
		switch(any_flags){
			case '@':	set_debugflags(optarg);						break;
			case 'l':	yy_flex_debug = 1;							break;
			case 'y':	yydebug = 1;								break;
			case 'D':	compflag = "-D " + (string) optarg + " ";	break;
			default:
				fprintf(stderr, "Flag not recognized: %c.\n", optopt);
				exit(EXIT_FAILURE);
		}
	}

	// Verifying a file is present.
	if(optind >= argc){
		fprintf(stderr, "No file found.\n");
		exit(EXIT_FAILURE);
	}

	string filename = argv[optind];

	// Verifying the file is of type ".oc".
	if(filename.substr(filename.find_last_of(".") + 1) != "oc"){
		fprintf(stderr, "File is not of type '.oc'.\n");
		exit(EXIT_FAILURE);
	}

	// Opening pipe with CPP compiler command.
	string cmd = CPP + compflag + filename;
	FILE* pipe = popen(cmd.c_str(), "r");
	if(pipe == NULL){
		fprintf(stderr, "%s: %s\n", cmd.c_str(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Turn -ly flag on, if included.
	if(yy_flex_debug || yydebug)
		fprintf(stderr, "-- popen(%s), fileno(pipe) = %d\n",
					cmd.c_str(), fileno(pipe));


	// Read file and build stringset for it.
	int f_len = filename.length();
	char c_filename[f_len + 1];
	strcpy(c_filename, filename.c_str());
	cpplines(pipe, c_filename);

	// Dump stringset into trace file with extension: .str
	string rawname = filename.substr(0, filename.size() - 3);
	string trace = rawname + ".str";
	ofstream trace_out;
	trace_out.open(trace);
	stringset::dump_stringset(&trace_out);
	trace_out.close();

	// Closing the pipe.
	if(pclose(pipe) != 0){
		fprintf(stderr, "Failed to close pipe.\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}