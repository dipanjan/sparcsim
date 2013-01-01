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


# C Compiler Flags
CFLAGS=-I. -I /usr/local/include/

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/Dip/Workspace/sparcsim/dist/gcc/GNU-Solaris-Sparc/sparcsim

/home/Dip/Workspace/sparcsim/dist/gcc/GNU-Solaris-Sparc/sparcsim: ${OBJECTFILES}
	${MKDIR} -p /home/Dip/Workspace/sparcsim/dist/gcc/GNU-Solaris-Sparc
	gcc -lelf -o /home/Dip/Workspace/sparcsim/dist/gcc/GNU-Solaris-Sparc/sparcsim ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/loader.o: loader.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/loader.o loader.c

${OBJECTDIR}/misc.o: misc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/misc.o misc.c

${OBJECTDIR}/breakpoint.o: breakpoint.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/breakpoint.o breakpoint.c

${OBJECTDIR}/register.o: register.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/register.o register.c

${OBJECTDIR}/decode.o: decode.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/decode.o decode.c

${OBJECTDIR}/memory.o: memory.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/memory.o memory.c

${OBJECTDIR}/execute.o: execute.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/execute.o execute.c

${OBJECTDIR}/trap.o: trap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/trap.o trap.c

${OBJECTDIR}/simulator.o: simulator.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/simulator.o simulator.c

${OBJECTDIR}/bits.o: bits.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/bits.o bits.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} /home/Dip/Workspace/sparcsim/dist/gcc/GNU-Solaris-Sparc/sparcsim

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
