#include "monitor.h"

#define BUFSIZE 500

int getRandomNumber()
{
    return (rand() % 9) + 1;
}

sensor *initializeSensors()
{
    sensor *sensors = malloc(sizeof(sensor) * 4);

    sensor temperature, pressure, speed, current;

    temperature.id = 1;
    temperature.name = "temperatura";

    pressure.id = 2;
    pressure.name = "pressão";

    speed.id = 3;
    speed.name = "velocidade";

    current.id = 4;
    current.name = "corrente";

    sensors = (sensor[4]){temperature, pressure, speed, current};

    return sensors;
}

equipment *initializeEquipments()
{
    equipment *equipments = malloc(sizeof(equipment) * 4);

    equipment belt, crane, overhead, forkLift;

    belt.id = 1;
    belt.name = "esteira";
    memset(&belt.sensors, 0, sizeof(int) * 4);

    crane.id = 2;
    crane.name = "guindaste";
    memset(&crane.sensors, 0, sizeof(int) * 4);

    overhead.id = 3;
    overhead.name = "ponte rolante";
    memset(&overhead.sensors, 0, sizeof(int) * 4);

    forkLift.id = 4;
    forkLift.name = "empilhadeira";
    memset(&forkLift.sensors, 0, sizeof(int) * 4);

    equipments = (equipment[4]){belt, crane, overhead, forkLift};

    return equipments;
}

steel *newSteelIndustryBuilding()
{
    steel *newBuilding = malloc(sizeof(steel));
    newBuilding->quantityOfSensors = 0;

    sensor *sensors = initializeSensors();
    memcpy(newBuilding->sensors, sensors, sizeof(sensor) * 4);

    equipment *equipments = initializeEquipments();
    memcpy(newBuilding->equipments, equipments, sizeof(equipment) * 4);

    return newBuilding;
}

int invalidSensor(steel *steelBuilding, int sensorId)
{
    int invalid = 1;
    for (int i = 0; i < 4; i++)
    {
        if (steelBuilding->sensors[i].id == sensorId)
        {
            invalid = 0;
        }
    }
    return invalid;
}

int invalidEquipment(steel *steelBuilding, int equipmentId)
{
    int invalid = 1;
    for (int i = 0; i < 4; i++)
    {
        if (steelBuilding->equipments[i].id != equipmentId)
        {
            invalid = 0;
        }
    }
    return invalid;
}

equipment *getEquipment(steel *steelBuilding, int equipmentId)
{
    equipment *equipment = NULL;
    for (int i = 0; i < 4; i++)
    {
        if (steelBuilding->equipments[i].id == equipmentId)
        {
            equipment = &(steelBuilding->equipments[i]);
            break;
        }
    }

    return equipment;
}

char *addSensorsToEquipment(int sensorsToAdd[4], int equipmentToAddSensorsTo, steel *steelBuilding)
{
    char output[BUFSIZE] = "";

    equipment *equipment = getEquipment(steelBuilding, equipmentToAddSensorsTo);
    if (equipment == NULL)
    {
        return "invalid equipment";
    }

    int existingSensors[4] = {0, 0, 0, 0};
    int *sensors = equipment->sensors;

    // validate if any of the sensors already exist
    int existingSensorCounter = 0;
    for (int i = 0; i < 4; i++)
    {
        // sensors to add != 0 => in that position we have a sensor id
        // sensors[(sensorsToAdd[i]) - 1] == 1 => that sensor already exists
        if (sensorsToAdd[i] != 0 && sensors[(sensorsToAdd[i]) - 1] == 1)
        {
            existingSensors[(sensorsToAdd[i]) - 1] = 1;
            existingSensorCounter++;
        }
    }

    strcat(output, "sensor ");
    int anySensorWasAdded = 0;
    for (int i = 0; i < 4; i++)
    {
        char sensorsExistingMessage[BUFSIZE] = "";
        if (sensorsToAdd[i] != 0 && existingSensors[(sensorsToAdd[i]) - 1] == 0)
        {
            sprintf(sensorsExistingMessage, "0%i ", sensorsToAdd[i]);
            strcat(output, sensorsExistingMessage);
            equipment->sensors[(sensorsToAdd[i]) - 1] = 1;
            steelBuilding->quantityOfSensors++;
            anySensorWasAdded = 1;
        }
    }

    if (anySensorWasAdded == 1)
    {
        strcat(output, "added");
    }

    if (existingSensorCounter > 0)
    {
        if (anySensorWasAdded == 1)
        {
            strcat(output, " ");
        }

        char sensorsExistingMessage[BUFSIZE] = "";
        for (int i = 0; i < 4; i++)
        {
            if (existingSensors[i] == 1)
            {
                sprintf(sensorsExistingMessage, "0%i ", (i + 1));
                strcat(output, sensorsExistingMessage);
            }
        }

        strcat(output, "already exists in ");
        sprintf(sensorsExistingMessage, "0%i ", equipmentToAddSensorsTo);
        strcat(output, sensorsExistingMessage);
    }

    char *strToReturn = output;

    return strToReturn;
}

char *removeSensorsFromEquipment(int sensorsToRemove[4], int equipmentToRemoveSensorsFrom, steel *steelBuilding)
{
    char output[BUFSIZE] = "";

    equipment *equipment = getEquipment(steelBuilding, equipmentToRemoveSensorsFrom);
    if (equipment == NULL)
    {
        return "invalid equipment";
    }

    int existingSensors[4] = {0, 0, 0, 0};
    int triedToAddNonExistingSensors = 0;
    int nonExistingSensors[4] = {0, 0, 0, 0};
    int *sensors = equipment->sensors;

    // validate if any of the sensors already exist
    int existingSensorCounter = 0;
    for (int i = 0; i < 4; i++)
    {
        // sensors to add != 0 => in that position we have a sensor id
        // sensors[(sensorsToAdd[i]) - 1] == 1 => that sensor already exists
        if (sensorsToRemove[i] != 0)
        {
            if (sensors[(sensorsToRemove[i]) - 1] == 1)
            {
                existingSensors[(sensorsToRemove[i]) - 1] = 1;
                existingSensorCounter++;
            }
            else
            {
                triedToAddNonExistingSensors = 1;
                nonExistingSensors[(sensorsToRemove[i]) - 1] = 1;
            }
        }
    }

    // sensor [id] removed
    // sensor [id] does not exist in [equipment]

    strcat(output, "sensor ");

    // remove the sensors that exist and add to the returning message
    int anySensorWasRemoved = 0;
    for (int i = 0; i < 4; i++)
    {
        char sensorsRemovedMessage[BUFSIZE] = "";
        if (sensorsToRemove[i] != 0 && existingSensors[(sensorsToRemove[i]) - 1] == 1)
        {
            sprintf(sensorsRemovedMessage, "0%i ", sensorsToRemove[i]);
            strcat(output, sensorsRemovedMessage);
            equipment->sensors[(sensorsToRemove[i]) - 1] = 0;
            anySensorWasRemoved = 1;
        }
    }

    if (anySensorWasRemoved == 1)
    {
        strcat(output, "removed");
    }

    // client tried to remove sensors that did not exist?
    if (triedToAddNonExistingSensors > 0)
    {
        if (anySensorWasRemoved == 1)
        {
            strcat(output, " ");
        }

        char sensorsNotExistingMessage[BUFSIZE] = "";
        puts("DEBUG: REMOVE command - trying to remove non existing sensors.");
        for (int i = 0; i < 4; i++)
        {
            if (nonExistingSensors[i] == 1)
            {
                sprintf(sensorsNotExistingMessage, "0%i ", (i + 1));
                strcat(output, sensorsNotExistingMessage);
            }
        }

        strcat(output, "does not exist in ");
        sprintf(sensorsNotExistingMessage, "0%i ", equipmentToRemoveSensorsFrom);
        strcat(output, sensorsNotExistingMessage);
    }

    char *strToReturn = output;

    return strToReturn;
}

char *listSensorsInEquipment(int equipmentId, steel *steelBuilding)
{
    char output[BUFSIZE] = "";

    equipment *equipment = getEquipment(steelBuilding, equipmentId);
    if (equipment == NULL)
    {
        return "invalid equipment";
    }

    int *sensors = equipment->sensors;

    int equipmentHasSensors = 0;
    char sensorList[BUFSIZE] = "";
    for (int i = 0; i < 4; i++)
    {
        if (sensors[i] == 1)
        {
            sprintf(sensorList, "0%i ", (i + 1));
            strcat(output, sensorList);

            equipmentHasSensors = 1;
        }
    }

    if (equipmentHasSensors == 0) // no sensor found in the equipment
    {
        strcat(output, "none");
    }

    char *strToReturn = output;

    return strToReturn;
}

char *readSensorsInEquipment(int sensorsToRead[4], int equipmentToReadSensorsFrom, steel *steelBuilding)
{
    char output[BUFSIZE] = "";

    equipment *equipment = getEquipment(steelBuilding, equipmentToReadSensorsFrom);
    if (equipment == NULL)
    {
        return "invalid equipment";
    }

    int existingSensors[4] = {0, 0, 0, 0};
    int triedToReadNonExistingSensors = 0;
    int nonExistingSensors[4] = {0, 0, 0, 0};
    int *sensors = equipment->sensors;

    // validate if any of the sensors already exist
    int existingSensorCounter = 0;
    for (int i = 0; i < 4; i++)
    {
        // sensors to read != 0 => in that position we have a sensor id
        // sensors[(sensorsToRead[i]) - 1] == 1 => that sensor already exists
        if (sensorsToRead[i] != 0)
        {
            if (sensors[(sensorsToRead[i]) - 1] == 1)
            {
                existingSensors[(sensorsToRead[i]) - 1] = 1;
                existingSensorCounter++;
            }
            else
            {
                triedToReadNonExistingSensors = 1;
                nonExistingSensors[(sensorsToRead[i]) - 1] = 1;
            }
        }
    }

    // sensor(s) [id] not installed
    // [value.2f]
    // [value.2f] and [id] not installed

    // read the values that exist
    int anySensorValueWasRead = 0;
    for (int i = 0; i < 4; i++)
    {
        char sensorsRemovedMessage[BUFSIZE] = "";
        if (sensorsToRead[i] != 0 && existingSensors[(sensorsToRead[i]) - 1] == 1)
        {
            if (anySensorValueWasRead == 1 && i < 3)
            {
                strcat(output, " ");
            }
            sprintf(sensorsRemovedMessage, "%i.%i%i", getRandomNumber(), getRandomNumber(), getRandomNumber());
            strcat(output, sensorsRemovedMessage);
            anySensorValueWasRead = 1;
        }
    }

    if (anySensorValueWasRead == 0)
    {
        strcat(output, "sensor(s) ");
    }

    // client tried to remove sensors that did not exist?
    if (triedToReadNonExistingSensors > 0)
    {
        if (anySensorValueWasRead == 1)
        {
            strcat(output, " and ");
        }

        char sensorsNotExistingMessage[BUFSIZE] = "";
        for (int i = 0; i < 4; i++)
        {
            if (nonExistingSensors[i] == 1)
            {
                sprintf(sensorsNotExistingMessage, "0%i ", (i + 1));
                strcat(output, sensorsNotExistingMessage);
            }
        }

        strcat(output, "not installed");
    }

    char *strToReturn = output;

    return strToReturn;
}
