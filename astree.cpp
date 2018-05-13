// $Id: astree.cpp,v 1.9 2017-10-04 15:59:50-07 - - $

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"

astree::astree (int symbol_, const location& lloc_, const char* info) {
   symbol = symbol_;
   lloc = lloc_;
   lexinfo = stringset::intern_stringset (info);
   // vector defaults to empty -- no children
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      astree::dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

astree* astree::adopt (astree* child1, astree* child2) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   return this;
}

// Optional to supply second child.
astree* astree::adopt_sym (astree* child1, int symbol_, astree* child2) {
   symbol = symbol_;
   return adopt (child1, child2);
}

astree* astree::adopt_children(astree* a){

   for(auto c: a->children) children.push_back(c);
   return this;
}

astree* astree::adopt_front(astree* a, astree* trash){

   destroy(trash);
   children.insert(children.begin(), a);
   return this;
}

astree* astree::make_root(int symbol_){

   return new astree(symbol_, {0,0,0}, "");
}

astree* astree::make_fn_tree(astree* identdecl, astree* params, astree* block, int prototype, int fn){

   astree* fn_tree;

   // Checking if a prototype.
   if(block->symbol == ';'){
      fn_tree = new astree(prototype, identdecl->lloc, "");
      return fn_tree->adopt(identdecl, params);

   // Otherwise it's a function.
   } else {
      fn_tree = new astree(fn, identdecl->lloc, "");
      fn_tree->adopt(identdecl, params);
      return fn_tree->adopt(block);
   }
}

// Handling structs. ----------------------------

astree* astree::struct_no_arg(astree* ident, astree* lbracket, astree* rbracket, int symbol_){

   destroy(lbracket, rbracket);
   ident->symbol = symbol_;
   return adopt(ident);
}

astree* astree::struct_arg(astree* ident, astree* lbracket, astree* statement, astree* scolon, astree* rbracket, int symbol_){

   destroy(lbracket, scolon);
   destroy(rbracket);
   ident->symbol = symbol_;
   return adopt(ident, statement);
}

astree* astree::struct_args(astree* ident, astree* lbracket, astree* fdeclarray, astree* rbracket, int symbol_){

   destroy(lbracket, rbracket);
   ident->symbol = symbol_;
   adopt(ident);
   adopt_children(fdeclarray);
   destroy(fdeclarray);
   return this;
}

// ---------------------------------------

astree* astree::fn(astree* lparen, astree* rparen, astree* block, int symbol_, int prototype, int fn){

   destroy(rparen);
   lparen->symbol = symbol_;
   return astree::make_fn_tree(this, lparen, block, prototype, fn);
}

astree* astree::adopt_child_sym(int symbol_, astree* ldelim, astree* rdelim, astree* child1, astree* child2){

   destroy(ldelim, rdelim);
   child1->symbol = symbol_;
   return adopt(child1, child2);
}

// Optional to supply second trash and child.
astree* astree::destroy_adopt(astree* trash1, astree* child1, astree* child2, astree* trash2){

   destroy(trash1, trash2);
   return adopt(child1, child2);
}

astree* astree::destroy_adopt_sym(astree* trash, int symbol_, astree* child1, astree* child2){

   symbol = symbol_;
   return destroy_adopt(trash, child1, child2);
}

astree* astree::destroy3_adopt3_sym(astree* trash1, astree* trash2, astree* trash3, int symbol_, astree* child1, astree* child2, astree* child3){

   destroy(trash1, trash2);
   destroy(trash3);
   symbol = symbol_;
   adopt(child1, child2);
   return adopt(child3);
}

astree* astree::adopt2_child_sym(int symbol_, astree* child1, astree* child2){

   child2->symbol = symbol_;
   return adopt(child1, child2);
}

astree* astree::destroy_paren(astree* lparen, astree* rparen){

   destroy(lparen, rparen);
   return this;
}

void astree::dump_node (FILE* outfile) {

   // .tok file contents.
   fprintf (outfile, "  %-2zu %2zu.%03zu %3d %-15s (%s)\n",
            lloc.filenr, lloc.linenr, lloc.offset, symbol,
            parser::get_tname(symbol), lexinfo->c_str());
   for (size_t child = 0; child < children.size(); ++child) {
      fprintf (outfile, " %p", children.at(child));
   }
}

void astree::dump_tree (FILE* outfile, int depth) {
   fprintf (outfile, "%*s", depth * 3, "");
   dump_node (outfile);
   fprintf (outfile, "\n");
   for (astree* child: children) child->dump_tree (outfile, depth + 1);
   fflush (nullptr);
}

void astree::dump (FILE* outfile, astree* tree) {
   if (tree == nullptr) fprintf (outfile, "nullptr");
                   else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) {

   //fprintf (outfile, "%*s", depth * 3, "");
   for(int i = 0; i < depth; i++){
      fprintf (outfile, "|   ");
   }
   const char *tname = parser::get_tname (tree->symbol);
   if(strstr(tname, "TOK_") == tname) tname += 4;
   fprintf (outfile, "%s \"%s\" %zd.%zd.%zd\n",
            tname, tree->lexinfo->c_str(),
            tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
   for (astree* child: tree->children) {
      astree::print (outfile, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              lexer::filename (lloc.filenr), lloc.linenr, lloc.offset,
              buffer);
}