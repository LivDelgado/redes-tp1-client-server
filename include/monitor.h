#ifndef MONITOR
#define MONITOR

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SENSOR_LIMIT 15

typedef struct
{
    int id;     // id of the sensor
    char *name; // name of the sensor
} sensor;

typedef struct
{
    int id;         // id of the equipment
    char *name;     // equipment name
    int sensors[4]; // array of sensors. if the sensor is there, it will be 1. example -> equipment has only sensor 1 -> [1, 0, 0, 0]
} equipment;

typedef struct
{
    int quantityOfSensors;   // quantity of sensors at total (should have maximum of 15)
    equipment equipments[4]; // equipments
    sensor sensors[4];       // sensors
} steel;

// public methods
steel *newSteelIndustryBuilding();

int invalidSensor(steel *steelBuilding, int sensorId);
int invalidEquipment(steel *steelBuilding, int equipmentId);

char *addSensorsToEquipment(int sensorsToAdd[4], int equipmentToAddSensorsTo, steel *steelBuilding);
char *removeSensorsFromEquipment(int sensorsToRemove[4], int equipmentToRemoveSensorsFrom, steel *steelBuilding);
char *listSensorsInEquipment(int equipmentId, steel *steelBuilding);
char *readSensorsInEquipment(int sensorsToRead[4], int equipmentToReadSensorsFrom, steel *steelBuilding);

#endif