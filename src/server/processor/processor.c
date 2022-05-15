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

static const char *SPLITTER = " ";

// buffer size
static const int BUFSIZE = 500;

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
    belt.sensors = NULL;

    crane.id = 2;
    crane.name = "guindaste";
    crane.sensors = NULL;

    overhead.id = 3;
    overhead.name = "ponte rolante";
    overhead.sensors = NULL;

    forkLift.id = 4;
    forkLift.name = "empilhadeira";
    forkLift.sensors = NULL;

    equipments = (equipment[4]){belt, crane, overhead, forkLift};

    return equipments;
}

steel *newSteelIndustryBuilding()
{
    steel *newBuilding = malloc(sizeof(steel));
    newBuilding->quantityOfSensors = 0;
    newBuilding->sensors = initializeSensors();
    newBuilding->equipments = initializeEquipments();

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

char *processKillCommand(char *message)
{
    char *output = NULL;
    // the kill command should have only one word (kill)
    char command[BUFSIZE];
    memset(&command, 0, sizeof(command));
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

char *processAddCommand(char *message) { return NULL; }

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
        return processAddCommand(message);
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
    char message[BUFSIZE];
    memset(&message, 0, sizeof(message));

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

    return processCommand(commandType, originalMessage, steelBuilding);
}