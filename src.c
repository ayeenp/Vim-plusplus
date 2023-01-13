// Ayeen Poostforoushan 401105742

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_CMD_LINE_LENGTH 200
#define MAX_WORD_LENGTH 100
#define MAX_WORD_IN_LINE 10
#define MAX_PATH_LENGTH 200
#define MAX_FILE_LINE_LENGTH 1000
#define MAX_INSERT_LENGHT 5000
#define MAX_ARGUMENTNAME_LENGTH 10

void run();
void cmdcreatefile(char *input);
void cmdInsert(char *input);
void cmdcat(char *input);
bool cat(char *fileName);
bool insertText(char *fileName, char *text, int linePos, int charPos);
bool createFileAndDirs(char *fileName);
bool createFile(const char *fileName);
void createAllDirs(const char *dirName);
bool directoryExists(const char *path);
void inputLine(char *str);
bool handleDoubleQuotationInput(const char *argumentContent, char *argument);
void copyStringRange(char *dest, const char *source, int start, int end);
bool copyNthWord(char *dest, const char *str, int n);
void spaceSplit(char (*words)[MAX_WORD_LENGTH], const char *str);
void makeBoolArrayZero(bool *arr, int n);
bool checkBoolArray(bool *arr, int n);
void makeArrayInitialsZero(char (*words)[MAX_WORD_LENGTH]);
void fixPathString(char *path);
void printSplittedWords(char (*words)[MAX_WORD_LENGTH], int strCount);

int main()
{
    run();
}

void run()
{
    char *input = (char *)malloc(MAX_CMD_LINE_LENGTH * sizeof(char));
    char command[20];
    while (1)
    {
        inputLine(input);
        copyNthWord(command, input, 1);
        if (strcmp(command, "quit") == 0)
            break;
        else if (strcmp(command, "createfile") == 0)
            cmdcreatefile(input);
        else if (strcmp(command, "insertstr") == 0)
            cmdInsert(input);
        else if (strcmp(command, "cat") == 0)
            cmdcat(input);
    }
    free(input);
}

void cmdcreatefile(char *input)
{
    // strtok's first output is the command which we dont need
    strtok(input, "-");
    char *argumentContent = strtok(NULL, "-");
    if (argumentContent == NULL)
    {
        printf("Type -file and continue with the path\n");
        return;
    }
    char argumentName[MAX_ARGUMENTNAME_LENGTH];
    copyNthWord(argumentName, argumentContent, 1);
    if (strcmp(argumentName, "file") == 0)
    {
        char path[MAX_PATH_LENGTH];
        if (copyNthWord(path, argumentContent, 2) == false)
        {
            printf("Type the path after -file\n");
            return;
        }
        if (path[0] == '"')
            if(handleDoubleQuotationInput(argumentContent, path) == false)
                    return;
        if (createFileAndDirs(path) == false)
            return;
    }
    else
        printf("Invalid argument\n");
}

void cmdInsert(char *input)
{
    // strtok's first output is the command which we dont need
    strtok(input, "-");
    bool argumentChecklist[3];
    makeBoolArrayZero(argumentChecklist, 3);
    char *argumentContent;
    char path[MAX_PATH_LENGTH];
    char textToInsert[MAX_INSERT_LENGHT];
    int linePos, charPos;

    for (int i = 0; i < 3; i++)
    {
        argumentContent = strtok(NULL, "-");
        if (argumentContent == NULL)
        {
            printf("Type the arguments after -\n");
            return;
        }
        char argumentName[MAX_ARGUMENTNAME_LENGTH];
        copyNthWord(argumentName, argumentContent, 1);
        if (strcmp(argumentName, "file") == 0 && argumentChecklist[0] == false)
        {
            if (copyNthWord(path, argumentContent, 2) == false)
            {
                printf("Type the path after -file\n");
                return;
            }
            if (path[0] == '"')
                if(handleDoubleQuotationInput(argumentContent, path) == false)
                    return;
            argumentChecklist[0] = true;
        }
        else if (strcmp(argumentName, "str") == 0 && argumentChecklist[1] == false)
        {
            if (copyNthWord(textToInsert, argumentContent, 2) == false)
            {
                printf("Type the text after -str\n");
                return;
            }
            if (textToInsert[0] == '"')
                if(handleDoubleQuotationInput(argumentContent, textToInsert) == false)
                    return;
            argumentChecklist[1] = true;
        }
        else if (strcmp(argumentName, "pos") == 0 && argumentChecklist[2] == false)
        {
            if(sscanf(argumentContent, "pos %d:%d", &linePos, &charPos) != 2)
            {
                printf("Type the position properly after -pos\n");
                return;
            }
            argumentChecklist[2] = true;
        }
        else
        {
            printf("Invalid input\n", i);
            return;
        }
    }
    if(checkBoolArray(argumentChecklist, 3))
        insertText(path, textToInsert, linePos, charPos);
}

void cmdcat(char *input)
{
    // strtok's first output is the command which we dont need
    strtok(input, "-");
    char *argumentContent = strtok(NULL, "-");
    if (argumentContent == NULL)
    {
        printf("Type -file and continue with the path\n");
        return;
    }
    char argumentName[MAX_ARGUMENTNAME_LENGTH];
    copyNthWord(argumentName, argumentContent, 1);
    if (strcmp(argumentName, "file") == 0)
    {
        char path[MAX_PATH_LENGTH];
        if (copyNthWord(path, argumentContent, 2) == false)
        {
            printf("Type the path after -file\n");
            return;
        }
        if (path[0] == '"')
            if(handleDoubleQuotationInput(argumentContent, path) == false)
                return;
        if (cat(path) == false)
            return;
    }
    else
        printf("Invalid argument\n");
}

bool cat(char *fileName)
{
    fixPathString(fileName);
    if (access(fileName, R_OK) == -1)
    {
        printf("File doesn't exist\n");
        return false;
    }
    FILE *fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Error occured while reading the file\n");
        return false;
    }
    char c;
    int lineNum = 1;
    printf("%d  >", lineNum++);
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n')
        {
            printf("\n%d  >", lineNum++);
            continue;
        }
        printf("%c", c);
    }
    printf("\n");
    fclose(fp);
    return true;
}

bool insertText(char *fileName, char *text, int linePos, int charPos)
{
    return true;
}

bool createFileAndDirs(char *fileName)
{
    fixPathString(fileName);
    createAllDirs(fileName);
    if (access(fileName, F_OK) == 0)
    {
        printf("File already exists\n");
        return false;
    }
    return createFile(fileName);
}

bool createFile(const char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        printf("Error occured while creating the file\n");
        return false;
    }
    fclose(fp);
    return true;
}

void createAllDirs(const char *dirName)
{
    char pathToMake[MAX_PATH_LENGTH];
    for (int i = 0; dirName[i] != '\0'; i++)
    {
        if (dirName[i] == '/')
        {
            pathToMake[i] = '\0';
            if (!directoryExists(pathToMake))
                mkdir(pathToMake);
        }
        pathToMake[i] = dirName[i];
    }
}

bool directoryExists(const char *path)
{
    struct stat stats;
    if (stat(path, &stats) == 0 && S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}

void inputLine(char *str)
{
    char c;
    int inputIndex = 0;
    c = getchar();
    while (c != '\n' && c != EOF && inputIndex != MAX_CMD_LINE_LENGTH)
    {
        str[inputIndex++] = c;
        c = getchar();
    }
    str[inputIndex] = '\0';
}

bool handleDoubleQuotationInput(const char *argumentContent, char *argument)
{
    int fisrtIndex, lastIndex;
    char *comma = strchr(argumentContent, '"');
    fisrtIndex = (int)(comma - argumentContent);
    comma = strrchr(argumentContent, '"');
    lastIndex = (int)(comma - argumentContent);
    if (fisrtIndex == lastIndex || fisrtIndex == lastIndex - 1)
    {
        printf("Invalid path input\n");
        return false;
    }
    if (argumentContent[lastIndex + 1] != ' ' && argumentContent[lastIndex + 1] != '\0')
    {
        printf("Invalid path input\n");
        return false;
    }
    copyStringRange(argument, argumentContent, fisrtIndex + 1, lastIndex);
    return true;
}

void copyStringRange(char *dest, const char *source, int start, int end)
{
    int destIndex = 0;
    for (int i = start; i < end && source[i] != '\0'; i++)
        dest[destIndex++] = source[i];
    dest[destIndex] = '\0';
}

bool copyNthWord(char *dest, const char *str, int n)
{
    int wordNum = 1;
    int i = 0;
    while (wordNum < n)
    {
        if (str[i] == '\0')
            return false;
        if (str[i] == ' ')
        {
            while (str[i] == ' ')
            {
                i++;
            }
            wordNum++;
            if (str[i] == '\0' || str[i] == '\n')
                return false;
            continue;
        }
        i++;
    }
    int destIndex = 0;
    while (str[i] != ' ' && str[i] != '\0' && str[i] != '\n')
    {
        dest[destIndex++] = str[i++];
    }
    dest[destIndex] = '\0';
    return true;
}

void spaceSplit(char (*words)[MAX_WORD_LENGTH], const char *str)
{
    int wordIndex = 0, charIndex = 0;
    makeArrayInitialsZero(words);
    for (int i = 0; wordIndex < MAX_WORD_IN_LINE; i++)
    {
        if (str[i] == ' ')
        {
            words[wordIndex][charIndex++] = '\0';
            wordIndex++;
            charIndex = 0;
            continue;
        }
        if (str[i] == '\0')
        {
            words[wordIndex][charIndex++] = '\0';
            return;
        }
        words[wordIndex][charIndex++] = str[i];
    }
}

void makeBoolArrayZero(bool *arr, int n)
{
    for (int i = 0; i < n; i++)
        arr[i] = false;
}

bool checkBoolArray(bool *arr, int n)
{
    for (int i = 0; i < n; i++)
        if(arr[i] == false)
            return false;
    return true;
}

void makeArrayInitialsZero(char (*words)[MAX_WORD_LENGTH])
{
    for (int i = 0; i < MAX_WORD_IN_LINE; i++)
        words[i][0] = '\0';
}

void fixPathString(char *path)
{
    if (path[0] != '/')
        return;
    for (int i = 0; 1; i++)
    {
        path[i] = path[i + 1];
        if (path[i] == '\0')
        {
            if (path[i - 1] == '/')
                path[i - 1] = '\0';
            break;
        }
    }
}

void printSplittedWords(char (*words)[MAX_WORD_LENGTH], int strCount)
{
    for (int i = 0; i < strCount; i++)
        printf("%d :%s\n", i, words[i]);
}
