#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=/usr/local/bin/gcc
CCC=/usr/local/bin/g++
CXX=/usr/local/bin/g++
FC=/usr/local/bin/g77
AS=/usr/local/bin/as

# Macros
CND_PLATFORM=GNU-Solaris-Sparc
CND_CONF=gcc
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/loader.o \
	${OBJECTDIR}/misc.o \
	${OBJECTDIR}/breakpoint.o \
	${OBJECTDIR}/register.o \
	${OBJECTDIR}/decode.o \
	${OBJECTDIR}/memory.o \
	${OBJECTDIR}/execute.o \
	${OBJECTDIR}/trap.o \
	${OBJECTDIR}/simulator.o \
	${OBJECTDIR}/bits.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1 \
	${TESTDIR}/TestFiles/f2

# C Compiler Flags
CFLAGS=-I. -I /usr/local/include/ -I /usr/local/include/CUnit

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sparcsim

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sparcsim: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	gcc -lelf -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sparcsim ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/loader.o: loader.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/loader.o loader.c

${OBJECTDIR}/misc.o: misc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/misc.o misc.c

${OBJECTDIR}/breakpoint.o: breakpoint.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/breakpoint.o breakpoint.c

${OBJECTDIR}/register.o: register.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/register.o register.c

${OBJECTDIR}/decode.o: decode.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/decode.o decode.c

${OBJECTDIR}/memory.o: memory.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/memory.o memory.c

${OBJECTDIR}/execute.o: execute.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/execute.o execute.c

${OBJECTDIR}/trap.o: trap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/trap.o trap.c

${OBJECTDIR}/simulator.o: simulator.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/simulator.o simulator.c

${OBJECTDIR}/bits.o: bits.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -MMD -MP -MF $@.d -o ${OBJECTDIR}/bits.o bits.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/testBits.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c}  -lelf -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} -lcunit -lcunit 

${TESTDIR}/TestFiles/f2: ${TESTDIR}/tests/testBreakPoint.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c}  -lelf -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS} -lcunit 


${TESTDIR}/tests/testBits.o: tests/testBits.c 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} $@.d
	$(COMPILE.c) -I. -I. -I. -I. -MMD -MP -MF $@.d -o ${TESTDIR}/tests/testBits.o tests/testBits.c


${TESTDIR}/tests/testBreakPoint.o: tests/testBreakPoint.c 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} $@.d
	$(COMPILE.c) -I. -I. -MMD -MP -MF $@.d -o ${TESTDIR}/tests/testBreakPoint.o tests/testBreakPoint.c


${OBJECTDIR}/loader_nomain.o: ${OBJECTDIR}/loader.o loader.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/loader.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/loader_nomain.o loader.c;\
	else  \
	    ${CP} ${OBJECTDIR}/loader.o ${OBJECTDIR}/loader_nomain.o;\
	fi

${OBJECTDIR}/misc_nomain.o: ${OBJECTDIR}/misc.o misc.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/misc.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/misc_nomain.o misc.c;\
	else  \
	    ${CP} ${OBJECTDIR}/misc.o ${OBJECTDIR}/misc_nomain.o;\
	fi

${OBJECTDIR}/breakpoint_nomain.o: ${OBJECTDIR}/breakpoint.o breakpoint.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/breakpoint.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/breakpoint_nomain.o breakpoint.c;\
	else  \
	    ${CP} ${OBJECTDIR}/breakpoint.o ${OBJECTDIR}/breakpoint_nomain.o;\
	fi

${OBJECTDIR}/register_nomain.o: ${OBJECTDIR}/register.o register.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/register.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/register_nomain.o register.c;\
	else  \
	    ${CP} ${OBJECTDIR}/register.o ${OBJECTDIR}/register_nomain.o;\
	fi

${OBJECTDIR}/decode_nomain.o: ${OBJECTDIR}/decode.o decode.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/decode.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/decode_nomain.o decode.c;\
	else  \
	    ${CP} ${OBJECTDIR}/decode.o ${OBJECTDIR}/decode_nomain.o;\
	fi

${OBJECTDIR}/memory_nomain.o: ${OBJECTDIR}/memory.o memory.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/memory.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/memory_nomain.o memory.c;\
	else  \
	    ${CP} ${OBJECTDIR}/memory.o ${OBJECTDIR}/memory_nomain.o;\
	fi

${OBJECTDIR}/execute_nomain.o: ${OBJECTDIR}/execute.o execute.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/execute.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/execute_nomain.o execute.c;\
	else  \
	    ${CP} ${OBJECTDIR}/execute.o ${OBJECTDIR}/execute_nomain.o;\
	fi

${OBJECTDIR}/trap_nomain.o: ${OBJECTDIR}/trap.o trap.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/trap.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/trap_nomain.o trap.c;\
	else  \
	    ${CP} ${OBJECTDIR}/trap.o ${OBJECTDIR}/trap_nomain.o;\
	fi

${OBJECTDIR}/simulator_nomain.o: ${OBJECTDIR}/simulator.o simulator.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/simulator.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/simulator_nomain.o simulator.c;\
	else  \
	    ${CP} ${OBJECTDIR}/simulator.o ${OBJECTDIR}/simulator_nomain.o;\
	fi

${OBJECTDIR}/bits_nomain.o: ${OBJECTDIR}/bits.o bits.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/bits.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/bits_nomain.o bits.c;\
	else  \
	    ${CP} ${OBJECTDIR}/bits.o ${OBJECTDIR}/bits_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	    ${TESTDIR}/TestFiles/f2 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sparcsim

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
