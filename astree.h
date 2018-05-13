// $Id: astree.h,v 1.7 2016-10-06 16:13:39-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node

   // Functions.
   astree(int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt(astree* child1, astree* child2 = nullptr);
   astree* adopt_children(astree* a);
   astree* adopt_front(astree* a, astree* trash);

   static astree* make_root(int symbol_);
   static astree* make_fn_tree(astree* identdecl, astree* params, astree* block, int prototype, int fn);

   // Handling structs.
   astree* struct_no_arg(astree* ident, astree* lbracket, astree* rbracket, int symbol_);
   astree* struct_arg(astree* ident, astree* lbracket, astree* statement, astree* scolon, astree* rbracket, int symbol_);
   astree* struct_args(astree* ident, astree* lbracket, astree* fdeclarray, astree* rbracket, int symbol_);

   // Adopt/Destroy operations.
   astree* fn(astree* lparen, astree* rparen, astree* block, int symbol_, int prototype, int fn);
   astree* adopt_sym(astree* child, int symbol_, astree* child2 = nullptr);
   astree* adopt_child_sym(int symbol_, astree* ldelim, astree* rdelim, astree* child1, astree* child2 = nullptr);
   astree* adopt2_child_sym(int symbol_, astree* child1, astree* child2);
   astree* destroy_adopt(astree* trash1, astree* child1, astree* child2 = nullptr, astree* trash2 = nullptr);
   astree* destroy_adopt_sym(astree* trash, int symbol_, astree* child1, astree* child2 = nullptr);
   astree* destroy3_adopt3_sym(astree* trash1, astree* trash2, astree* trash3, int symbol_, astree* child1, astree* child2, astree* child3);
   astree* destroy_paren(astree* lparen, astree* rparen);

   void dump_node(FILE*);
   void dump_tree(FILE*, int depth = 0);
   static void dump(FILE* outfile, astree* tree);
   static void print(FILE* outfile, astree* tree, int depth = 0);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif
