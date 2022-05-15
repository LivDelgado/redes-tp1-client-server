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

int getCommandType(char *command)
{
    printf("command %s, length: %lu\n", command, strlen(command));
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

char *processCommand(int commandType, char *message)
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

char *processMessage(char *originalMessage)
{
    puts("reached the processor");
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

    return processCommand(commandType, originalMessage);
}