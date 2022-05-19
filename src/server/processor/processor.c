#include "processor.h"

// constants related to the commands processed by the server
static const char *KILL_COMMAND = "kill";
#define KILL 1
static const char *ADD_COMMAND = "add";
#define ADD 2
static const char *REMOVE_COMMAND = "remove";
#define REMOVE 3
static const char *LIST_COMMAND = "list";
#define LIST 4
static const char *READ_COMMAND = "read";
#define READ 5

#define SENSOR_LIMIT 15

static const char *SPLITTER = " ";

// buffer size
#define BUFSIZE 500

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
    puts("DEBUG: Initializing steel industry building!");
    steel *newBuilding = malloc(sizeof(steel));
    newBuilding->quantityOfSensors = 0;

    sensor *sensors = initializeSensors();
    memcpy(newBuilding->sensors, sensors, sizeof(sensor) * 4);

    equipment *equipments = initializeEquipments();
    memcpy(newBuilding->equipments, equipments, sizeof(equipment) * 4);

    return newBuilding;
}

int getCommandType(char *command)
{
    if (strcmp(command, KILL_COMMAND) == 0)
    {
        return KILL;
    }
    else if (strcmp(command, ADD_COMMAND) == 0)
    {
        return ADD;
    }
    else if (strcmp(command, LIST_COMMAND) == 0)
    {
        return LIST;
    }
    else if (strcmp(command, REMOVE_COMMAND) == 0)
    {
        return REMOVE;
    }
    else if (strcmp(command, READ_COMMAND) == 0)
    {
        return READ;
    }
    else
    {
        return -1;
    }
}

int invalidSensor(steel *steelBuilding, int sensorId)
{
    int invalid = 1;
    for (int i = 0; i < 4; i++)
    {
        printf("DEBUG: invalidSensor - comparing ids. from steel %i, id searched %i\n", steelBuilding->sensors[i].id, sensorId);
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
        printf("DEBUG: invalidEquipment - comparing ids. from steel %i, id searched %i\n", steelBuilding->equipments[i].id, equipmentId);
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
        printf("DEBUG: getEquipment - comparing ids. from steel %i, id searched %i\n", steelBuilding->equipments[i].id, equipmentId);
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
    puts("DEBUG: ADD command - going to add sensors to the equipment!");

    char output[BUFSIZE] = "";

    equipment *equipment = getEquipment(steelBuilding, equipmentToAddSensorsTo);
    if (equipment == NULL)
    {
        return "invalid equipment";
    }
    printf("DEBUG: ADD command - found equipment -> %i\n", equipment->id);

    int existingSensors[4] = {0, 0, 0, 0};
    int *sensors = equipment->sensors;

    puts("DEBUG: ADD command - checking if there is any sensor already added.");
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

    puts("DEBUG: ADD command - all good to add the sensors to the equipment");
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
        puts("DEBUG: ADD command - trying to add existing sensors.");
        for (int i = 0; i < 4; i++)
        {
            if (existingSensors[i] == 1)
            {
                sprintf(sensorsExistingMessage, "0%i ", (i+1));
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

char *processKillCommand(char *message)
{
    char *output = NULL;
    // the kill command should have only one word (kill)
    char command[BUFSIZE] = "";
    strncpy(command, message, strlen(message) - 1);

    // split message by spaces
    char *word;
    word = strtok(message, SPLITTER);
    word = strtok(NULL, " ");

    if (word == NULL)
    {
        output = "kill";
    }

    // reset command
    memset(&command, 0, sizeof(command));
    // reset word
    memset(&word, 0, sizeof(word));

    return output;
}

char *processAddCommand(char *message, steel *steelBuilding)
{
    puts("DEBUG: add command!");
    char *output = NULL;

    //
    // add sensor [sensorId] in [equipmentid]
    //

    char command[BUFSIZE] = "";

    strcpy(command, message);

    int finishedListingSensors = 0;

    // split message by spaces
    puts("DEBUG: finding out what are the words int the command");
    char *word = strtok(command, SPLITTER);
    word = strtok(NULL, " ");

    int sensorCounter = 0;
    int equipmentCounter = 0;

    if (strcmp(word, "sensor") == 0) // second word should be sensor
    {
        int sensorsToAdd[4] = {0, 0, 0, 0};
        int equipmentToAddSensorsTo = 0;

        while (word != NULL)
        {
            word = strtok(NULL, " ");
            if (word == NULL)
                break;

            printf("DEBUG: ADD command - current word: %s\n", word);
            if (strcmp(word, "in") == 0)
            {
                finishedListingSensors = 1;
                continue;
            }

            // should be either a sensor or an equipment
            int id = atoi(word);
            if (id == 0)
            { // invalid number!
                puts("DEBUG: invalid number in the ADD command");
                break;
            }

            if (finishedListingSensors == 0) // it should be a sensor
            {
                puts("DEBUG: ADD command - validating sensor");
                if (sensorCounter > 3)
                {
                    // the command is already invalid. we can not have more than 4 sensors per equipment.
                    // if it is already bigger than 3, this is after the fourth
                    puts("DEBUG: ADD command - more sensors than it should have!");
                    break;
                }
                if (invalidSensor(steelBuilding, id) == 1)
                {
                    output = "invalid sensor";
                    break;
                }
                sensorsToAdd[sensorCounter] = id; // sensors to add => {03, 01, 02, 04}
                sensorCounter++;
            }
            else
            { // it should be an equipment
                puts("DEBUG: ADD command - validating equipment");
                equipmentCounter++;

                if (equipmentCounter > 1)
                {
                    // the command is already invalid. we can not have more than 1 equipment per command.
                    puts("DEBUG: ADD command - more equipments than it should have!");
                    break;
                }
                if (invalidEquipment(steelBuilding, id) == 1)
                {
                    output = "invalid equipment";
                    break;
                }

                equipmentToAddSensorsTo = id;
            }
        }

        puts("DEBUG: ADD command - finished reading the command. Let's process it.");

        if (finishedListingSensors != 0 && sensorCounter > 0 && equipmentCounter == 1)
        {
            // verify if limit of equipments (at total) will be exceeded
            if (steelBuilding->quantityOfSensors + sensorCounter <= SENSOR_LIMIT)
            {
                output = addSensorsToEquipment(sensorsToAdd, equipmentToAddSensorsTo, steelBuilding);
            }
            else
            {
                output = "limit exceeded";
            }
        }
    }

    // reset command
    memset(&command, 0, sizeof(command));
    // reset word
    memset(&word, 0, sizeof(word));

    return output;
}

char *processRemoveCommand(char *message) { return NULL; }

char *processListCommand(char *message) { return NULL; }

char *processReadCommand(char *message) { return NULL; }

char *processCommand(int commandType, char *message, steel *steelBuilding)
{
    switch (commandType)
    {
    case KILL:
        return processKillCommand(message);
    case ADD:
        return processAddCommand(message, steelBuilding);
    case REMOVE:
        return processRemoveCommand(message);
    case LIST:
        return processListCommand(message);
    case READ:
        return processReadCommand(message);
    default:
        return NULL;
    }
}

char *processMessage(char *originalMessage, steel *steelBuilding)
{
    char message[BUFSIZE] = "";

    strcpy(message, originalMessage);

    // split message by spaces
    char *command = strtok(message, SPLITTER);
    int commandType = getCommandType(command);

    if (commandType < 0)
    {
        return NULL;
    }

    // reset message
    memset(&message, 0, sizeof(message));
    // reset command
    memset(&command, 0, sizeof(command));

    printf("DEBUG: will process command: %s\n", originalMessage);
    return processCommand(commandType, originalMessage, steelBuilding);
}