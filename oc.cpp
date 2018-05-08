/******************************************************************************
 * Preston Fraser, pfraser
 * oc.cpp
 *
 * oc Compiler for CMPS-104A
 *****************************************************************************/

#include <assert.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdarg.h>

#include "stringset.h"
#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"
#include "string.h"

using namespace std;

const string CPP = "/usr/bin/cpp ";
constexpr size_t LINESIZE = 1024;

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

// Creating stringset dump file.
void fdump_stringset(string rawname){
	string trace = rawname + ".str";
	ofstream trace_out;
	trace_out.open(trace);
	stringset::dump_stringset(&trace_out);
	trace_out.close();
}

// Creating scanned token file. 
void fscan_tok(string rawname, string cmd){
	string fileout = rawname + ".tok";
	out = fopen(fileout.c_str(), "w");
	lexer::newfilename(cmd);
	while(yylex() != YYEOF) {
		yylval->dump_node(out);
	}
}

int main (int argc, char** argv) {

	// Command: "oc [-ly] [-@flag ...] [-D string] program.oc"
	exec::execname = basename (argv[0]);
	string compflag = "";
	int any_flags = 0;

	// Setting default debug modes: OFF
	yy_flex_debug = 0;
	yydebug = 0;

	while((any_flags = getopt(argc, argv, "D:@:ly:")) != -1){
		// fprintf(stdout, "flag: %d\n", any_flags);
		if(any_flags == EOF) break;
		switch(any_flags){
			case 'D':	compflag = "-D " + (string) optarg + " ";	break;
			case '@':	set_debugflags(optarg);						break;
			case 'l':	yy_flex_debug = 1;							break;
			case 'y':	yydebug = 1;								break;
			default:
				fprintf(stderr, "Flag not recognized: %c\n", optopt);
				exit(EXIT_FAILURE);
		}
	}

	// Verifying a file is present.
	if(optind > argc){
		fprintf(stderr, "No file found.\n");
		exit(EXIT_FAILURE);
	}

	if(yydebug) optind--;
	string filename = argv[optind];
	// fprintf(stdout, "filename = %s\n", filename.c_str());

	// Verifying the file is of type ".oc".
	if(filename.substr(filename.find_last_of(".") + 1) != "oc"){
		fprintf(stderr, "File is not of type '.oc'.\n");
		exit(EXIT_FAILURE);
	}

	// Opening external pipe, yyin, with CPP compiler command.
	string cmd = CPP + compflag + filename;
	yyin = popen(cmd.c_str(), "r");
	if(yyin == NULL){
		fprintf(stderr, "%s: %s\n", cmd.c_str(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Turn -ly flag on, if included.
	if (yy_flex_debug) {
		fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
				cmd.c_str(), fileno (yyin));
	}
	if(yydebug) fprintf(stderr, "Debug mode for yyparse() is on.\n");

	// Read file and build stringset for it.
	int f_len = filename.length();
	char c_filename[f_len + 1];
	strcpy(c_filename, filename.c_str());
//	cpplines(yyin, c_filename);

	// Drop extension from filename.
	string rawname = filename.substr(0, filename.size() - 3);

	fscan_tok(rawname, cmd);
	fdump_stringset(rawname);

	// Closing the pipe.
	if(pclose(yyin) != 0){
		fprintf(stderr, "Failed to close pipe.\n");
		exit(EXIT_FAILURE);
	}

	yylex_destroy();

	exit(EXIT_SUCCESS);
}