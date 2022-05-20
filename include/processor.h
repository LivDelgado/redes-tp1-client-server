#ifndef PROCESSOR
#define PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monitor.h"

int getCommandType(char *command);

char *processMessage(char *originalMessage, steel *steelBuilding);

#endif