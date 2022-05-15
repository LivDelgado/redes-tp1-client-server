#include "processor.h"

// constants related to the commands processed by the server
static const char *KILL_COMMAND = "kill";
static const int KILL = 1;
static const char *ADD_COMMAND = "add";
static const int ADD = 2;
static const char *REMOVE_COMMAND = "remove";
static const int REMOVE = 3;
static const char *LIST_COMMAND = "list";
static const int LIST = 4;
static const char *READ_COMMAND = "read";
static const int READ = 5;

// buffer size
static const int BUFSIZE = 500;

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

char *processMessage(char *originalMessage)
{
    char message[BUFSIZE];
    strncpy(message, originalMessage, strlen(originalMessage) - 1);

    // split message by spaces
    char *word;
    int wordCounter = 0;
    char *splitter = " ";

    word = strtok(message, splitter);
    int commandType = getCommandType(word);

    if (commandType < 0)
    {
        return NULL;
    }

    while (word != NULL)
    {
        wordCounter += 1;
        word = strtok(NULL, " ");
    }

    // reset message
    memset(&message, 0, sizeof(message));

    return "this is a server response";
}