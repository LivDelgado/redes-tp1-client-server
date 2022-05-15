#ifndef PROCESSOR
#define PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int id; // id of the sensor
    char *name; // name of the sensor
} sensor;

typedef struct
{
    int id; // id of the equipment
    char *name; // equipment name
    int sensors[4]; // array of sensors. if the sensor is there, it will be 1. example -> equipment has only sensor 1 -> [1, 0, 0, 0]
} equipment;

typedef struct
{
    int quantityOfSensors;
    equipment equipments[4];
    sensor sensors[4];
} steel;

int getCommandType(char *command);
char *processMessage(char *originalMessage, steel *steelBuilding);

steel *newSteelIndustryBuilding();

#endif