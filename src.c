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
#define MAX_POS_LENGHT 10
#define MAX_ARGUMENTNAME_LENGTH 10
#define TEMP_ADDRESS "TMP"

void run();
void cmdcreatefile(char *input);
void cmdInsert(char *input);
void cmdcat(char *input);
bool cat(char *fileName);
bool insertText(char *fileName, char *text, int linePos, int charPos);
bool writeNlines(int n, FILE *tempptr, FILE *sourceptr);
bool writeNchars(int n, bool isEOF, FILE *tempptr, FILE *sourceptr);
void writeStrToFile(char *text, bool isEOL, FILE *tempptr, FILE *sourceptr);
bool createFileAndDirs(char *fileName);
bool createFile(const char *fileName);
void createAllDirs(const char *dirName);
bool directoryExists(const char *path);
void inputLine(char *str);
bool handleDoubleQuotation(char *str);
bool removeDoubleQuotations(char *str);
int findMatchingWord(const char *str, const char *match);
void copyStringRange(char *dest, const char *source, int start, int end);
bool copyNthWord(char *dest, const char *str, int n);
void fixPathString(char *path);

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
    int arg1 = findMatchingWord(input, "--file");
    if (arg1 == -1)
    {
        printf("Required: --file\n");
        return;
    }
    char fileName[MAX_PATH_LENGTH];
    copyStringRange(fileName, input, arg1 + 1, -1);
    if (!handleDoubleQuotation(fileName))
    {
        printf("Invalid path input\n");
        return;
    }
    createFileAndDirs(fileName);
}

void cmdInsert(char *input)
{
    char path[MAX_PATH_LENGTH];
    char textToInsert[MAX_INSERT_LENGHT];
    char pos[MAX_POS_LENGHT];
    int linePos, charPos;

    int arg1 = findMatchingWord(input, "--file");
    int arg2 = findMatchingWord(input, "--str");
    int arg3 = findMatchingWord(input, "-pos");
    if (arg1 == -1 || arg2 == -1 || arg3 == -1)
    {
        printf("Required: --file, --str, -pos\n");
        return;
    }
    copyStringRange(path, input, arg1 + 1, arg2 - 6);
    copyStringRange(textToInsert, input, arg2 + 1, arg3 - 5);
    copyStringRange(pos, input, arg3 + 1, -1);
    if (!handleDoubleQuotation(path))
    {
        printf("Invalid path input\n");
        return;
    }
    if (!handleDoubleQuotation(textToInsert))
    {
        printf("Invalid text input\n");
        return;
    }
    if (sscanf(pos, "%d:%d", &linePos, &charPos) != 2)
    {
        printf("Type the position properly after -pos\n");
        return;
    }
    insertText(path, textToInsert, linePos, charPos);
}

void cmdcat(char *input)
{
    int arg1 = findMatchingWord(input, "--file");
    if (arg1 == -1)
    {
        printf("Required: --file\n");
        return;
    }
    char fileName[MAX_PATH_LENGTH];
    copyStringRange(fileName, input, arg1 + 1, -1);
    if (!handleDoubleQuotation(fileName))
    {
        printf("Invalid path input\n");
        return;
    }
    cat(fileName);
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
    FILE *sourceptr = fopen(fileName, "r");
    if (sourceptr == NULL)
    {
        printf("File doesn't exist\n");
        return false;
    }
    FILE *tempptr = fopen(TEMP_ADDRESS, "w");
    char c;
    // writing first lines
    bool isEOF = writeNlines(linePos, tempptr, sourceptr);
    // writing first chars in the line
    bool isEOL = writeNchars(charPos, isEOF, tempptr, sourceptr);
    // writing the string being inserted
    writeStrToFile(text, isEOL, tempptr, sourceptr);
    // writing the rest of the file
    while ((c = fgetc(sourceptr)) != EOF)
    {
        fprintf(tempptr, "%c", c);
    }
    fclose(tempptr);
    fclose(sourceptr);
    if (remove(fileName) != 0)
    {
        printf("Couldn't compelete the insertion (can't remove the source file)\n");
        return false;
    }
    if (rename(TEMP_ADDRESS, fileName) != 0)
    {
        printf("Couldn't compelete the insertion (can't rename the temp file)\n");
        return false;
    }
    return true;
}

bool writeNlines(int n, FILE *tempptr, FILE *sourceptr)
{
    char c;
    int lineCounter = 1;
    bool isEOF = false;
    while (lineCounter != n)
    {
        if (isEOF)
        {
            fprintf(tempptr, "\n");
            lineCounter++;
            continue;
        }
        while (1)
        {
            c = fgetc(sourceptr);
            if (c == EOF)
            {
                isEOF = true;
                break;
            }
            if (c == '\n')
            {
                lineCounter++;
                break;
            }
            fprintf(tempptr, "%c", c);
        }
        if (!isEOF)
            fprintf(tempptr, "\n");
    }
    return isEOF;
}

bool writeNchars(int n, bool isEOF, FILE *tempptr, FILE *sourceptr)
{
    char c;
    bool isEOL = false;
    int charCounter = 0;
    while (charCounter != n)
    {
        if (isEOF || isEOL)
        {
            fprintf(tempptr, " ");
            charCounter++;
            continue;
        }
        c = fgetc(sourceptr);
        if (c == EOF)
        {
            isEOF = true;
            continue;
        }
        if (c == '\n')
        {
            isEOL = true;
            continue;
        }
        fprintf(tempptr, "%c", c);
        charCounter++;
    }
}

void writeStrToFile(char *text, bool isEOL, FILE *tempptr, FILE *sourceptr)
{
    bool backSlashMode = false;
    for (int i = 0; text[i] != '\0'; i++)
    {
        if (backSlashMode)
        {
            if (text[i] == '\\')
            {
                fprintf(tempptr, "\\");
            }
            else if (text[i] == 'n')
            {
                fprintf(tempptr, "\n");
            }
            backSlashMode = false;
            continue;
        }
        if (text[i] == '\\')
        {
            backSlashMode = true;
            continue;
        }
        fprintf(tempptr, "%c", text[i]);
    }
    if (isEOL)
        fprintf(tempptr, "\n");
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

bool handleDoubleQuotation(char *str)
{
    if (!removeDoubleQuotations(str) && strchr(str, ' ') != NULL)
        return false;
    return true;
}

bool removeDoubleQuotations(char *str)
{
    if (str[0] != '"' || str[strlen(str) - 1] != '"')
        return false;
    for (int i = 0; 1; i++)
    {
        str[i] = str[i + 1];
        if (str[i] == '\0')
        {
            str[i - 1] = '\0';
            break;
        }
    }
    return true;
}

int findMatchingWord(const char *str, const char *match)
{
    int matchIndex = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == match[matchIndex])
        {
            matchIndex++;
            if (match[matchIndex] == '\0')
                return i + 1;
        }
        else
        {
            matchIndex = 0;
            if (str[i] == match[matchIndex])
                matchIndex++;
        }
    }
    return -1;
}

void copyStringRange(char *dest, const char *source, int start, int end)
{
    if (end == -1)
        end = MAX_INSERT_LENGHT;
    if (start >= end)
    {
        printf("copyStringRange start greater than end\n");
        return;
    }
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
