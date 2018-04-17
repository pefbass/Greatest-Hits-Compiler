#------------------------------------------------------------------------------
# Preston Fraser, pfraser
# Makefile
#
# Contains the following targets: all, clean, spotless, ci, and deps. 
#------------------------------------------------------------------------------

DEPSFILE	= Makefile.deps
NOINCLUDE	= ci clean spotless
NEEDINCL	= ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
CPP			= g++ -std=gnu++17 -g -O0
CPPWARN		= ${CPP} -Wall -Wextra -Wold-style-cast
CPPYY		= ${CPP} -Wno-sign-compare -Wno-register
MKDEPS		= g++ -std=gnu++17 -MM
GRIND		= valgrind --leak-check=full --show-reachable=yes
FLEX		= flex --outfile=${LEXCPP}
BISON		= bison --defines=${PARSEHDR} --output=${PARSECPP}

MODULES		= stringset
HDRSRC		= ${MODULES}
CPPSRC		= ${MODULES:=.cpp} oc.cpp
FLEXSRC		= lexer.l
# LEXCPP		= yylex.cpp
# BISONSRC	= parser.y
# PARSECPP	= yyparse.cpp
# CGENS		= ${LEXCP} ${PARSECPP}
# ALLGENS		= ${PARSEHDR} ${CGENS}
# PARSEHDR	= yyparse.h
# ALLCSRC		= ${CPPSRC} ${CGENS}
OBJECTS		= ${CPPSRC:.cpp=.o}
LEXOUT		= yylex.output
PARSEOUT	= yyparse.output
EXECBIN		= oc
MODSRC		= ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.cpp}
MISCSRC		= ${filter-out ${MODSRC}, ${HDRSRC} ${CPPSRC}}
LISTSRC		= ${CPPSRC} ${DEPSFILE} ${PARSEHDR}

all:		${EXECBIN}

${EXECBIN}:	${OBJECTS}
			${CPPWARN} -o${EXECBIN} ${OBJECTS}

# yylex.o:	yylex.cpp
# 			@ # Suppress warning message from flex compilation.
# 			${CPP} -Wno-sign-compare -c $<

# ${LEXCPP}:	${FLEXSRC}
# 			${FLEX} ${FLEXSRC}

# ${PARSECPP} ${PARSEHDR}:	${BISONSRC}
# 							${BISON} ${BISONSRC}

clean:	
			- rm ${OBJECTS} ${DEPSFILE}

spotless:	clean
			- rm ${EXECBIN}

ci:			
			- git add .

deps:		${CPPSRC}
			@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
			${MKDEPS} ${CPPSRC} >>${DEPSFILE}

${DEPSFILE} :
			@ touch ${DEPSFILE}
			${MAKE} --no-print-directory deps

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif
