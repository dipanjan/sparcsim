#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <loader.h>
#include <memory.h>
#include <decode.h>
#include <register.h>
#include <breakpoint.h>
#include <execute.h>
#include <constants.h>



int processSimulatorCommand(char* simulatorCommand);
char* splitSimulatorCommand(char* simulatorCommand);

#endif
