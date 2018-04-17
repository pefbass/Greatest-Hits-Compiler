// $Id: oclib.c,v 1.71 2018-04-03 17:28:38-07 - - $

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __OCLIB_C__
#include "oclib.h"

void* xcalloc (int nelem, int size) {
   void* result = calloc (nelem, size);
   assert (result != NULL);
   return result;
}

char* scan (int (*skipover) (int), int (*stopat) (int)) {
   int byte;
   do {
      byte = getchar();
      if (byte == EOF) return NULL;
   } while (skipover (byte));
   char buffer[0x1000];
   char* end = buffer;
   do {
      *end++ = byte;
      assert (end < &buffer[sizeof buffer]);
      *end = '\0';
      byte = getchar();
   }while (byte != EOF && ! stopat (byte));
   char* result = strdup (buffer);
   assert (result != NULL);
   return result;
}

int isfalse (int byte)      { (void) byte; return 0; } 
int isnl (int byte)         { return byte == '\n'; }
void putint (int val)       { printf ("%d", val); }
void putstr (const char* s) { puts (s); }
char* getword (void)        { return scan (isspace, isspace); }
char* getln (void)          { return scan (isfalse, isnl); } 
