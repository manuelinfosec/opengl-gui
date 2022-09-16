# $Id: Makefile,v 1.10 2018-02-21 17:49:00-08 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
GPP         = g++ -std=gnu++17 -g -O0 -Wall -Wextra -Wold-style-cast

MODULES     = graphics interp rgbcolor shape debug util main
CPPHEADER   = $(wildcard ${MODULES:=.h})
CPPSOURCE   = $(wildcard ${MODULES:=.cpp})
TCCFILES    = $(wildcard ${MODULES:=.tcc})
GENFILES    = colors.cppgen
SOURCES     = $(wildcard ${foreach MOD, ${MODULES}, \
                 ${MOD}.h ${MOD}.tcc ${MOD}.cpp})
OTHERS      = ${MKFILE} ${DEPFILE} mk-colors.perl
ALLSOURCES  = ${SOURCES} ${OTHERS}
EXECBIN     = gdraw
OBJECTS     = ${CPPSOURCE:.cpp=.o}
LINKLIBS    = -lGL -lGLU -lglut -lm

LISTING     = Listing.ps
CLASS       = cmps109-wm.w15
PROJECT     = asg3

all : ${EXECBIN}
	- checksource ${ALLSOURCES}

echo :
	- echo ${ALLSOURCES}

${EXECBIN} : ${OBJECTS}
	${GPP} -o $@ ${OBJECTS} ${LINKLIBS}

%.o : %.cpp
	${GPP} -c $<
	- checksource $<
	- cpplint.py.perl $<

colors.cppgen: mk-colors.perl
	mk-colors.perl >colors.cppgen

ci : ${ALLSOURCES}
	cid + ${ALLSOURCES}
	- checksource ${ALLSOURCES}

lis : ${ALLSOURCES}
	mkpspdf ${LISTING} ${ALLSOURCES} ${DEPFILE}

clean :
	- rm ${OBJECTS} ${DEPFILE} core ${GENFILES}

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}


submit : ${ALLSOURCES}
	- checksource ${ALLSOURCES}
	submit ${CLASS} ${PROJECT} ${ALLSOURCES}

dep : ${CPPSOURCE} ${CPPHEADER} ${TCCFILES} ${GENFILES}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${GPP} -MM ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

