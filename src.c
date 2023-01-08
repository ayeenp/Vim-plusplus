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


void run();
bool createFile(char *fileName);
void createDir(const char *dirName);
bool directoryExists(const char *path);
void inputLine(char *str);
void spaceSplit(char (*words)[MAX_WORD_LENGTH], const char *str);
void makeArrayInitialsZero(char (*words)[MAX_WORD_LENGTH]);
void fixPathString(char *path);
void printSplittedWords(char (*words)[MAX_WORD_LENGTH], int strCount);
void printStr(char *str);

int main()
{
    run();
}

void run()
{
    char *input = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    char(*splittedWords)[MAX_WORD_LENGTH] = (char(*)[MAX_WORD_LENGTH])malloc(MAX_WORD_IN_LINE * sizeof(char[MAX_WORD_LENGTH]));
    while (1)
    {
        inputLine(input);
        spaceSplit(splittedWords, input);
        if (strcmp(splittedWords[0], "quit") == 0)
            break;
        else if (strcmp(splittedWords[0], "createfile") == 0)
        {
            createFile(splittedWords[2]);
        }
    }
    free(input);
    free(splittedWords);
}

bool createFile(char *fileName)
{
    fixPathString(fileName);
    createDir(fileName);
    if (access(fileName, F_OK) == 0)
        printf("File already exists\n");
    else
    {
        FILE *fp;
        fp = fopen(fileName, "w");
        fclose(fp);
    }
}

void createDir(const char *dirName)
{
    char pathToMake[MAX_PATH_LENGTH];
    for(int i = 0; dirName[i - 1] != '\0'; i++)
    {
        if(dirName[i] == '/' && !directoryExists(pathToMake))
            mkdir(pathToMake);
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
    {
        words[i][0] = '\0';
    }
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
            if(path[i - 1] == '/')
            {
                path[i - 1] = '\0';
            }
            break;
        }
    }
}

void printSplittedWords(char (*words)[MAX_WORD_LENGTH], int strCount)
{
    for (int i = 0; i < strCount; i++)
    {
        printf("%d :", i);
        printStr(words[i]);
        printf("\n");
    }
}

void printStr(char *str)
{
    // printf("*");
    for (int i = 0; str[i] != '\0'; i++)
        printf("%c", str[i]);
    // printf("*");
}
