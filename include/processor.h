#ifndef PROCESSOR
#define PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int id;
    char *name;
} sensor;

typedef struct
{
    int id;
    char *name;
    int *sensors;
} equipment;

typedef struct
{
    int quantityOfSensors;
    equipment *equipments;
    sensor *sensors;
} steel;

int getCommandType(char *command);
char *processMessage(char *originalMessage, steel *steelBuilding);

steel *newSteelIndustryBuilding();

#endif