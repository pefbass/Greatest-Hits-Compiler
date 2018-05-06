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

MODULES		= lyutils auxlib astree stringset
HDRSRC		= ${MODULES:=.h}
CPPSRC		= ${MODULES:=.cpp} oc.cpp 
FLEXSRC		= scanner.l
BISONSRC	= parser.y
PARSEHDR	= yyparse.h
LEXCPP		= yylex.cpp
PARSECPP	= yyparse.cpp
CGENS		= ${LEXCPP} ${PARSECPP}
ALLGENS		= ${PARSEHDR} ${CGENS}
ALLCSRC		= ${CPPSRC} ${CGENS}
OBJECTS		= ${ALLCSRC:.cpp=.o}
LEXOUT		= yylex.output
PARSEOUT	= yyparse.output
REPORTS		= ${LEXOUT} ${PARSEOUT}
EXECBIN		= oc
MODSRC		= ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.cpp}
MISCSRC		= ${filter-out ${MODSRC}, ${HDRSRC} ${CPPSRC}}
LISTSRC		= ${CPPSRC} ${DEPSFILE} ${PARSEHDR}

all:		${EXECBIN}

${EXECBIN}:	${OBJECTS}
			${CPPWARN} -o${EXECBIN} ${OBJECTS}

yylex.o:	yylex.cpp
			${CPPYY} -c $<

yyparse.o:	yyparse.cpp
			${CPPYY} -c $<

%.o:		%.cpp
			${CPP} -c $<

${LEXCPP}:	${FLEXSRC}
			${FLEX} ${FLEXSRC}

${PARSECPP} ${PARSEHDR}:	${BISONSRC}
							${BISON} ${BISONSRC}

clean:	
			- rm ${OBJECTS} ${DEPSFILE} ${ALLGENS} ${REPORTS}

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
