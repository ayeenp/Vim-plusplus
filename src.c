// Ayeen Poostforoushan 401105742

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 200
#define MAX_WORD_LENGTH 100
#define MAX_WORD_IN_LINE 10
#define MAX_PATH_LENGTH 200
#define MAX_ARGUMENTNAME_LENGTH 10

void run();
void cmdcreatefile(char *input);
void cmdcat(char *input);
bool cat(char *fileName);
bool createFileAndDirs(char *fileName);
bool createFile(const char *fileName);
void createAllDirs(const char *dirName);
bool directoryExists(const char *path);
void inputLine(char *str);
void copyStringRange(char *dest, const char *source, int start, int end);
bool copyNthWord(char *dest, const char *str, int n);
void spaceSplit(char (*words)[MAX_WORD_LENGTH], const char *str);
void makeArrayInitialsZero(char (*words)[MAX_WORD_LENGTH]);
void fixPathString(char *path);
void printSplittedWords(char (*words)[MAX_WORD_LENGTH], int strCount);

int main()
{
    run();
}

void run()
{

    char(*splittedWords)[MAX_WORD_LENGTH] = (char(*)[MAX_WORD_LENGTH])malloc(MAX_WORD_IN_LINE * sizeof(char[MAX_WORD_LENGTH]));
    char *input = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    char command[20];
    while (1)
    {
        inputLine(input);
        copyNthWord(command, input, 1);
        if (strcmp(command, "quit") == 0)
            break;
        else if (strcmp(command, "createfile") == 0)
            cmdcreatefile(input);
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
        {
            int fisrtIndex, lastIndex;
            char *comma = strchr(argumentContent, '"');
            fisrtIndex = (int)(comma - argumentContent);
            comma = strrchr(argumentContent, '"');
            lastIndex = (int)(comma - argumentContent);
            if (fisrtIndex == lastIndex || (argumentContent[lastIndex + 1] != ' ' && argumentContent[lastIndex + 1] != '\0'))
            {
                printf("Invalid path input\n");
                return;
            }
            copyStringRange(path, argumentContent, fisrtIndex + 1, lastIndex);
        }
        createFileAndDirs(path);
    }
    else
        printf("Invalid argument\n");
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
        {
            int fisrtIndex, lastIndex;
            char *comma = strchr(argumentContent, '"');
            fisrtIndex = (int)(comma - argumentContent);
            comma = strrchr(argumentContent, '"');
            lastIndex = (int)(comma - argumentContent);
            if (fisrtIndex == lastIndex || fisrtIndex == lastIndex - 1)
            {
                printf("Invalid path input\n");
                return;
            }
            if (argumentContent[lastIndex + 1] != ' ' && argumentContent[lastIndex + 1] != '\0')
            {
                printf("Invalid path input\n");
                return;
            }
            copyStringRange(path, argumentContent, fisrtIndex + 1, lastIndex);
        }
        cat(path);
    }
    else
        printf("Invalid argument\n");
}

bool cat(char *fileName)
{
    fixPathString(fileName);
    if (access(fileName, R_OK) == -1)
    {
        printf("Cant read the file\n");
        return false;
    }
    else
    {
        FILE *fp;
        fp = fopen("sal.txt", "r");
        if (fp == NULL)
            return false;
        char str[400];
        while (1)
        {
            if (fscanf(fp, "%s", str) == -1)
                break;
            printf("%s\n", str);
        }
    }
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
    else
        return createFile(fileName);
}

bool createFile(const char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "w");
    if (fp == NULL)
        return false;
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
    while (c != '\n' && c != EOF && inputIndex != MAX_LINE_LENGTH)
    {
        str[inputIndex++] = c;
        c = getchar();
    }
    str[inputIndex] = '\0';
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
