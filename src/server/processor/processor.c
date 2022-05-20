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

static const char *SPLITTER = " "; // character that represents the separation between words in the commands

#define BUFSIZE 500 // buffer size

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
    char command[BUFSIZE] = "";
    strncpy(command, message, strlen(message) - 1);

    // split message by spaces
    char *word;
    word = strtok(message, SPLITTER);
    word = strtok(NULL, " ");

    // if there is more than one word, the output will remain null, what represents an unknown command in the server
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
    // add sensor [sensorId] in [equipmentid]

    char *output = NULL;

    char command[BUFSIZE] = "";
    strcpy(command, message);

    int finishedListingSensors = 0; // flag to represent when the list of sensors finished

    // split message by spaces
    char *word = strtok(command, SPLITTER);
    word = strtok(NULL, " ");

    int sensorCounter = 0;    // number of sensors in the command (max of 4)
    int equipmentCounter = 0; // number of equipments in the command (max of 1)

    if (strcmp(word, "sensor") == 0) // second word should be sensor
    {
        int sensorsToAdd[4] = {0, 0, 0, 0};
        int equipmentToAddSensorsTo = 0;

        while (word != NULL) // while there are still words to be read
        {
            word = strtok(NULL, " "); // takes next word
            if (word == NULL)
                break;

            if (strcmp(word, "in") == 0) // if the word is in, the list of sensors finished
            {
                finishedListingSensors = 1;
                continue;
            }

            // this word should be either a sensor or an equipment
            int id = atoi(word);
            if (id == 0) // invalid number! therefore, invalid command
            {
                break;
            }

            if (finishedListingSensors == 0) // it should be a sensor
            {
                if (sensorCounter > 3)
                {
                    // the command is already invalid. we can not have more than 4 sensors per equipment.
                    // if it is already bigger than 3, this is after the fourth
                    break;
                }
                if (invalidSensor(steelBuilding, id) == 1)
                {
                    output = "invalid sensor";
                    break;
                }
                sensorsToAdd[sensorCounter] = id; // sensors to add format => {03, 01, 02, 04}
                sensorCounter++;
            }
            else
            { // it should be an equipment
                equipmentCounter++;

                if (equipmentCounter > 1)
                {
                    // the command is already invalid. we can not have more than 1 equipment per command.
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

        // finished reading the command

        // finishedListingSensors = 0 would mean that the client never completed the command
        // the sensor counter <= 0 means that the client didn't inform a sensor, which is also invalid
        // the equipment counter != 1 means that the client informed a wrong number of equipments, which is invalid
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

char *processRemoveCommand(char *message, steel *steelBuilding)
{
    // remove sensor [sensorId] in [equipmentid]

    char *output = NULL;

    char command[BUFSIZE] = "";
    strcpy(command, message);

    int finishedListingSensors = 0; // flag to represent when the list of sensors finished

    // split message by spaces
    char *word = strtok(command, SPLITTER);
    word = strtok(NULL, " ");

    int sensorCounter = 0;    // number of sensors in the command (max of 4)
    int equipmentCounter = 0; // number of equipments in the command (max of 1)

    if (strcmp(word, "sensor") == 0) // second word should be sensor
    {
        int sensorsToRemove[4] = {0, 0, 0, 0};
        int equipmentToRemoveSensorsFrom = 0;

        while (word != NULL) // while there are still words to be read
        {
            word = strtok(NULL, " "); // takes next word
            if (word == NULL)
                break;

            if (strcmp(word, "in") == 0) // if the word is in, the list of sensors finished
            {
                finishedListingSensors = 1;
                continue;
            }

            // should be either a sensor or an equipment
            int id = atoi(word);
            if (id == 0)
            { // invalid number!
                break;
            }

            if (finishedListingSensors == 0) // it should be a sensor
            {
                if (invalidSensor(steelBuilding, id) == 1)
                {
                    output = "invalid sensor";
                    break;
                }
                sensorsToRemove[sensorCounter] = id; // sensors to remove format => {03, 01, 02, 04}
                sensorCounter++;
            }
            else
            { // it should be an equipment
                equipmentCounter++;

                if (equipmentCounter > 1)
                {
                    // the command is already invalid. we can not have more than 1 equipment per command.
                    break;
                }
                if (invalidEquipment(steelBuilding, id) == 1)
                {
                    output = "invalid equipment";
                    break;
                }

                equipmentToRemoveSensorsFrom = id;
            }
        }

        // finished reading the command

        // finishedListingSensors = 0 would mean that the client never completed the command
        // the sensor counter <= 0 means that the client didn't inform a sensor, which is also invalid
        // the equipment counter != 1 means that the client informed a wrong number of equipments, which is invalid
        if (finishedListingSensors != 0 && sensorCounter > 0 && equipmentCounter == 1)
        {
            output = removeSensorsFromEquipment(sensorsToRemove, equipmentToRemoveSensorsFrom, steelBuilding);
        }
    }

    // reset command
    memset(&command, 0, sizeof(command));
    // reset word
    memset(&word, 0, sizeof(word));

    return output;
}

char *processListCommand(char *message, steel *steelBuilding)
{
    // list sensor in [equipmentid]

    char *output = NULL;

    char command[BUFSIZE] = "";
    strcpy(command, message);

    // split message by spaces
    char *word = strtok(command, SPLITTER);
    word = strtok(NULL, " ");

    if (strcmp(word, "sensor") == 0) // second word should be sensor
    {
        int equipmentToListSensorsFrom = 0;

        word = strtok(NULL, " ");

        if (strcmp(word, "in") == 0)
        { // third word should be in
            word = strtok(NULL, " ");
            // fourth and last word is the quipment id
            int id = atoi(word);
            if (id != 0) // valid number
            {
                if (invalidEquipment(steelBuilding, id) == 1)
                {
                    output = "invalid equipment";
                }
                else
                {
                    equipmentToListSensorsFrom = id;

                    word = strtok(NULL, " "); // takes next word
                    if (word == NULL)         // valid command now that we only listed 1 equipment
                    {
                        output = listSensorsInEquipment(equipmentToListSensorsFrom, steelBuilding);
                    }
                }
            }
        }
    }

    // reset command
    memset(&command, 0, sizeof(command));
    // reset word
    memset(&word, 0, sizeof(word));

    return output;
}

char *processReadCommand(char *message, steel *steelBuilding)
{
    // read [sensorId] in [equipmentid]

    char *output = NULL;

    char command[BUFSIZE] = "";
    strcpy(command, message);

    int finishedListingSensors = 0;

    // split message by spaces
    char *word = strtok(command, SPLITTER);

    int sensorCounter = 0;
    int equipmentCounter = 0;

    int sensorsToRead[4] = {0, 0, 0, 0}; // number of sensors in the command (max of 4)
    int equipmentToReadSensorsFrom = 0;  // number of equipments in the command (max of 1)

    while (word != NULL) // while there are still words to be read
    {
        word = strtok(NULL, " "); // takes next word
        if (word == NULL)
            break;

        if (strcmp(word, "in") == 0) // if the word is in, the list of sensors finished
        {
            finishedListingSensors = 1;
            continue;
        }

        // should be either a sensor or an equipment
        int id = atoi(word);
        if (id == 0)
        { // invalid number!
            break;
        }

        if (finishedListingSensors == 0) // it should be a sensor
        {
            if (sensorCounter > 3)
            {
                // the command is already invalid. we can not have more than 4 sensors per equipment.
                // if it is already bigger than 3, this is after the fourth
                break;
            }
            if (invalidSensor(steelBuilding, id) == 1)
            {
                output = "invalid sensor";
                break;
            }
            sensorsToRead[sensorCounter] = id; // sensors to read format => {03, 01, 02, 04}
            sensorCounter++;
        }
        else
        { // it should be an equipment
            equipmentCounter++;

            if (equipmentCounter > 1)
            {
                // the command is already invalid. we can not have more than 1 equipment per command.
                break;
            }
            if (invalidEquipment(steelBuilding, id) == 1)
            {
                output = "invalid equipment";
                break;
            }

            equipmentToReadSensorsFrom = id;
        }
    }

    // finished reading the command

    // finishedListingSensors = 0 would mean that the client never completed the command
    // the sensor counter <= 0 means that the client didn't inform a sensor, which is also invalid
    // the equipment counter != 1 means that the client informed a wrong number of equipments, which is invalid
    if (finishedListingSensors != 0 && sensorCounter > 0 && equipmentCounter == 1)
    {
        output = readSensorsInEquipment(sensorsToRead, equipmentToReadSensorsFrom, steelBuilding);
    }

    // reset command
    memset(&command, 0, sizeof(command));
    // reset word
    memset(&word, 0, sizeof(word));

    return output;
}

char *processCommand(int commandType, char *message, steel *steelBuilding)
{
    // select the processor based on the command type (kind of a strategy pattern)
    switch (commandType)
    {
    case KILL:
        return processKillCommand(message);
    case ADD:
        return processAddCommand(message, steelBuilding);
    case REMOVE:
        return processRemoveCommand(message, steelBuilding);
    case LIST:
        return processListCommand(message, steelBuilding);
    case READ:
        return processReadCommand(message, steelBuilding);
    default:
        return NULL;
    }
}

char *processMessage(char *originalMessage, steel *steelBuilding)
{
    char message[BUFSIZE] = "";
    strcpy(message, originalMessage); // copy to avoid changing the message string

    // split message by spaces
    char *command = strtok(message, SPLITTER);
    int commandType = getCommandType(command);

    if (commandType < 0) // invalid command type
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