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

#include "string.h"
#include "stringset.h"
//#include "auxlib.h"


#ifdef NDEBUG
// Do not generate any code.
#define DEBUGF(FLAG,...)   /**/
#define DEBUGSTMT(FLAG,STMTS) /**/
#else
// Generate debugging code.
void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...);
#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __PRETTY_FUNCTION__, \
                       __VA_ARGS__)
#define DEBUGSTMT(FLAG,STMTS) \
        if (is_debugflag (FLAG)) { DEBUGF (FLAG, "\n"); STMTS }
#endif

using namespace std;

const string CPP = "/usr/bin/cpp ";
constexpr size_t LINESIZE = 1024;
const char* debugflags = "";
bool alldebugflags = false;


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

void set_debugflags (const char* flags) {
   debugflags = flags;
   assert (debugflags != nullptr);
   if (strchr (debugflags, '@') != nullptr) alldebugflags = true;
   DEBUGF ('x', "Debugflags = \"%s\", all = %d\n",
           debugflags, alldebugflags);
}

bool is_debugflag (char flag) {
   return alldebugflags or strchr (debugflags, flag) != nullptr;
}

void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...) {
   va_list args;
   if (not is_debugflag (flag)) return;
   fflush (nullptr);
   va_start (args, format);
   fprintf (stderr, "DEBUGF(%c): %s[%d] %s():\n",
             flag, file, line, func);
   vfprintf (stderr, format, args);
   va_end (args);
   fflush (nullptr);
}


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

	while((any_flags = getopt(argc, argv, "D:@:ly:")) != -1){
		fprintf(stdout, "flag: %d\n", any_flags);
		if(any_flags == EOF) break;
		switch(any_flags){
			case 'D':	compflag = "-D " + (string) optarg + " ";	break;
			case '@':	set_debugflags(optarg);						break;
			case 'l':	yy_flex_debug = 1;							break;
			case 'y':	yydebug = 1;								break;
			default:
				fprintf(stderr, "Flag not recognized: %c.\n", optopt);
				exit(EXIT_FAILURE);
		}
	}

	// Verifying a file is present.pr
	if(optind > argc){
		fprintf(stderr, "No file found.", optind);
		exit(EXIT_FAILURE);
	}

	if(yydebug) optind--;
	string filename = argv[optind];
	fprintf(stdout, "filename = %s\n", filename.c_str());

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
	if(yy_flex_debug) fprintf(stderr, "Debug mode for yylex() is on.\n");
	if(yydebug) fprintf(stderr, "Debug mode for yyparse() is on.\n");


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