// Ayeen Poostforoushan 401105742

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <windows.h>

#define MAX_CMD_LINE_LENGTH 200
#define MAX_WORD_LENGTH 100
#define MAX_WORD_IN_LINE 10
#define MAX_PATH_LENGTH 200
#define MAX_FILE_LINE_LENGTH 1000
#define MAX_STREAM_LENGTH 10000
#define MAX_INT_LENGTH 10
#define MAX_ARGUMENTNAME_LENGTH 10
#define TEMP_ADDRESS "root/.TMP"
#define CMD_REM 1
#define CMD_COPY 2
#define CMD_CUT 3

void run();
void cmdCreatefile(char *input);
void cmdInsert(char *input);
void cmdRemCopyCut(char *input, int mode);
void cmdFind(char *input);
void cmdPaste(char *input);
void cmdCat(char *input);
bool cat(char *fileName);
bool insertText(char *fileName, char *text, int linePos, int charPos);
bool removeText(char *fileName, int linePos, int charPos, int size, bool isForward);
void find(char *fileName, char *toBeFound, int at, bool isCount, bool isByWord, bool isAll);
int findAt(const char *fileString, char *toBeFound, int at, bool isByWord, int *size);
bool copyFileContentToClipboard(char *fileName, int linePos, int charPos, int size, bool isForward);
bool cutFileContentToClipboard(char *fileName, int linePos, int charPos, int size, bool isForward);
bool pasteFromClipboard(char *fileName, int linePos, int charPos);
void copyStrToClipboard(const char *str);
void retrieveStrFromClipboard(char *str);
void handleWildCards(char *str, bool *leadingWC, bool *endingWC);
void handleNewlines(char *str);
void readAndWriteNlines(int n, FILE *tempptr, FILE *sourceptr);
bool readAndWriteNchars(int n, FILE *tempptr, FILE *sourceptr);
bool seekNlines(int n, FILE *sourceptr);
bool seekNchars(int n, bool isForward, FILE *sourceptr);
void writeStrToFile(char *text, bool isEOL, FILE *tempptr, FILE *sourceptr);
bool createFileAndDirs(char *fileName);
bool createFile(const char *fileName);
void createAllDirs(const char *dirName);
bool directoryExists(const char *path);
void inputLine(char *str);
void fileToString(FILE *fp, char *str);
bool handleDoubleQuotation(char *str);
bool removeDoubleQuotations(char *str);
int findMatchingWord(const char *str, const char *match);
bool findMatchFromIndex(const char *str, const char *match, int startingIndex, bool isForward);
void copyStringRange(char *dest, const char *source, int start, int end);
bool copyNthWord(char *dest, const char *str, int n);
bool findNthWord(const char *str, int n, int *startIndex, int *endIndex);
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
            cmdCreatefile(input);
        else if (strcmp(command, "insertstr") == 0)
            cmdInsert(input);
        else if (strcmp(command, "removestr") == 0)
            cmdRemCopyCut(input, CMD_REM);
        else if (strcmp(command, "copystr") == 0)
            cmdRemCopyCut(input, CMD_COPY);
        else if (strcmp(command, "cutstr") == 0)
            cmdRemCopyCut(input, CMD_CUT);
        else if (strcmp(command, "pastestr") == 0)
            cmdPaste(input);
        else if (strcmp(command, "find") == 0)
            cmdFind(input);
        else if (strcmp(command, "cat") == 0)
            cmdCat(input);
    }
    free(input);
}

void cmdCreatefile(char *input)
{
    int arg1index = findMatchingWord(input, " --file");
    if (arg1index == -1)
    {
        printf("Required: --file\n");
        return;
    }
    char fileName[MAX_PATH_LENGTH];
    copyStringRange(fileName, input, arg1index + 1, -1);
    if (!handleDoubleQuotation(fileName))
    {
        printf("Invalid path input\n");
        return;
    }
    fixPathString(fileName);
    createFileAndDirs(fileName);
}

void cmdInsert(char *input)
{
    char path[MAX_PATH_LENGTH];
    char textToInsert[MAX_STREAM_LENGTH];
    char pos[MAX_INT_LENGTH];
    int linePos, charPos;

    int arg1index = findMatchingWord(input, " --file");
    int arg2index = findMatchingWord(input, " --str");
    int arg3index = findMatchingWord(input, " -pos");
    if (arg1index == -1 || arg2index == -1 || arg3index == -1)
    {
        printf("Required: --file, --str, -pos\n");
        return;
    }
    copyStringRange(path, input, arg1index + 1, arg2index - 6);
    copyStringRange(textToInsert, input, arg2index + 1, arg3index - 5);
    copyStringRange(pos, input, arg3index + 1, -1);
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
    fixPathString(path);
    insertText(path, textToInsert, linePos, charPos);
}

void cmdRemCopyCut(char *input, int mode)
{
    char path[MAX_PATH_LENGTH];
    char pos[MAX_INT_LENGTH];
    char sizeStr[MAX_INT_LENGTH];
    int linePos, charPos, size;
    bool isForward = true;

    int arg1index = findMatchingWord(input, " --file");
    int arg2index = findMatchingWord(input, " -pos");
    int arg3index = findMatchingWord(input, " -size");
    int arg4index = findMatchingWord(input, " -f");
    if (arg4index == -1)
    {
        isForward = false;
        arg4index = findMatchingWord(input, " -b");
    }
    if (arg1index == -1 || arg2index == -1 || arg3index == -1 || arg4index == -1)
    {
        printf("Required: --file, -pos, -size, -b or -f\n");
        return;
    }
    copyStringRange(path, input, arg1index + 1, arg2index - 5);
    copyStringRange(pos, input, arg2index + 1, arg3index - 5);
    copyStringRange(sizeStr, input, arg3index + 1, arg4index - 3);

    if (!handleDoubleQuotation(path))
    {
        printf("Invalid path input\n");
        return;
    }
    fixPathString(path);
    if (sscanf(pos, "%d:%d", &linePos, &charPos) != 2)
    {
        printf("Type the position properly after -pos\n");
        return;
    }
    if (sscanf(sizeStr, "%d", &size) != 1)
    {
        printf("Type the size properly after -size\n");
        return;
    }
    switch (mode)
    {
    case CMD_REM:
        removeText(path, linePos, charPos, size, isForward);
        break;
    case CMD_COPY:
        copyFileContentToClipboard(path, linePos, charPos, size, isForward);
        break;
    case CMD_CUT:
        cutFileContentToClipboard(path, linePos, charPos, size, isForward);
        break;
    }
}

void cmdFind(char *input)
{
    char path[MAX_PATH_LENGTH];
    char textToBeFound[MAX_STREAM_LENGTH];
    bool isCount = true, isByword = true, isAll = true;
    int at = 1;
    int leastArgIndex = MAX_CMD_LINE_LENGTH;
    int arg1index = findMatchingWord(input, "--str");
    int arg2index = findMatchingWord(input, " --file");
    int arg3index = findMatchingWord(input, " -count");
    if (arg3index == -1)
        isCount = false;
    else
        leastArgIndex = arg3index - 7;
    arg3index = findMatchingWord(input, " -at");
    if (arg3index != -1)
    {
        char atstr[MAX_INT_LENGTH];
        copyStringRange(atstr, input, arg3index + 1, arg3index + 3);
        if (sscanf(atstr, "%d ", &at) != 1)
        {
            printf("Type the number properly after -at\n");
            return;
        }
        if (arg3index <= leastArgIndex)
            leastArgIndex = arg3index - 4;
    }
    arg3index = findMatchingWord(input, " -byword");
    if (arg3index == -1)
        isByword = false;
    else if (arg3index <= leastArgIndex)
        leastArgIndex = arg3index - 8;
    arg3index = findMatchingWord(input, " -all");
    if (arg3index == -1)
        isAll = false;
    else if (arg3index <= leastArgIndex)
        leastArgIndex = arg3index - 5;
    if (arg1index == -1 || arg2index == -1)
    {
        printf("Required: --str, --file\n");
        return;
    }
    if (leastArgIndex == MAX_CMD_LINE_LENGTH)
        leastArgIndex = -1;
    copyStringRange(textToBeFound, input, arg1index + 1, arg2index - 7);
    copyStringRange(path, input, arg2index + 1, leastArgIndex);
    if (!handleDoubleQuotation(path))
    {
        printf("Invalid path input\n");
        return;
    }
    if (!handleDoubleQuotation(textToBeFound))
    {
        printf("Invalid text input\n");
        return;
    }
    fixPathString(path);
    find(path, textToBeFound, at, isCount, isByword, isAll);
}

void cmdPaste(char *input)
{
    char path[MAX_PATH_LENGTH];
    char pos[MAX_INT_LENGTH];
    int linePos, charPos;

    int arg1index = findMatchingWord(input, " --file");
    int arg2index = findMatchingWord(input, " -pos");
    if (arg1index == -1 || arg2index == -1)
    {
        printf("Required: --file, -pos\n");
        return;
    }
    copyStringRange(path, input, arg1index + 1, arg2index - 5);
    copyStringRange(pos, input, arg2index + 1, -1);
    if (!handleDoubleQuotation(path))
    {
        printf("Invalid path input\n");
        return;
    }
    if (sscanf(pos, "%d:%d", &linePos, &charPos) != 2)
    {
        printf("Type the position properly after -pos\n");
        return;
    }
    fixPathString(path);
    pasteFromClipboard(path, linePos, charPos);
}

void cmdCat(char *input)
{
    int arg1index = findMatchingWord(input, " --file");
    if (arg1index == -1)
    {
        printf("Required: --file\n");
        return;
    }
    char fileName[MAX_PATH_LENGTH];
    copyStringRange(fileName, input, arg1index + 1, -1);
    if (!handleDoubleQuotation(fileName))
    {
        printf("Invalid path input\n");
        return;
    }
    fixPathString(fileName);
    cat(fileName);
}

bool cat(char *fileName)
{
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
    // writing first lines
    readAndWriteNlines(linePos, tempptr, sourceptr);
    // writing first chars in the line
    bool isEOL = readAndWriteNchars(charPos, tempptr, sourceptr);
    // writing the string being inserted
    writeStrToFile(text, isEOL, tempptr, sourceptr);
    // writing the rest of the file
    char c;
    while ((c = fgetc(sourceptr)) != EOF)
        fprintf(tempptr, "%c", c);
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

bool removeText(char *fileName, int linePos, int charPos, int size, bool isForward)
{
    FILE *sourceptr = fopen(fileName, "r");
    if (sourceptr == NULL)
    {
        printf("File doesn't exist\n");
        return false;
    }
    FILE *tempptr = fopen(TEMP_ADDRESS, "w+");
    // writing first lines
    readAndWriteNlines(linePos, tempptr, sourceptr);
    // writing first chars in the line
    readAndWriteNchars(charPos, tempptr, sourceptr);
    if (!isForward)
        seekNchars(size, false, tempptr);
    else
        seekNchars(size, true, sourceptr);
    // writing the rest of the file
    char c;
    while ((c = fgetc(sourceptr)) != EOF)
        fprintf(tempptr, "%c", c);
    fclose(tempptr);
    fclose(sourceptr);
    remove(fileName);
    rename(TEMP_ADDRESS, fileName);
    return true;
}

void find(char *fileName, char *toBeFound, int at, bool isCount, bool isByWord, bool isAll)
{
    if ((isAll && isCount) || (isAll && at != 1) || (isCount && isByWord) || (isCount && at != 1))
    {
        printf("Wrong combination of arguments for find\n");
        return;
    }
    if (access(fileName, R_OK) == -1)
    {
        printf("File doesn't exist\n");
        return;
    }
    FILE *fp;
    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("Error occured while reading the file\n");
        return;
    }
    char fileString[MAX_STREAM_LENGTH];
    fileToString(fp, fileString);
    fclose(fp);
    
    int size;
    if (isAll)
    {
        while (1)
        {
            int index = findAt(fileString, toBeFound, at, isByWord, &size);
            if (index == -1)
                break;
            printf("%d ", index);
            at++;
        }
        printf("\n");
        return;
    }
    if(isCount)
    {
        while (1)
        {
            int index = findAt(fileString, toBeFound, at, isByWord, &size);
            if (index == -1)
                break;
            at++;
        }
        printf("%d\n", at - 1);
        return;
    }
    printf("%d\n", findAt(fileString, toBeFound, at, isByWord, &size));
    return;
}

int findAt(const char *fileString, char *toBeFound, int at, bool isByWord, int *size)
{
    bool leadingWC = false, endingWC = false;
    handleWildCards(toBeFound, &leadingWC, &endingWC);
    handleNewlines(toBeFound);

    int startIndex, endIndex, count = 0;
    for (int wordCount = 1; 1; wordCount++)
    {
        if (!findNthWord(fileString, wordCount, &startIndex, &endIndex))
            break;
        for (int i = startIndex; i <= endIndex; i++)
        {
            if (!findMatchFromIndex(fileString, toBeFound, i, 1))
                continue;
            count++;
            if (at == count)
            {
                if (isByWord)
                    return wordCount;
                else
                    return i;
            }
        }
    }
    return -1;
}

bool copyFileContentToClipboard(char *fileName, int linePos, int charPos, int size, bool isForward)
{
    FILE *sourceptr = fopen(fileName, "r");
    char copyingStr[MAX_STREAM_LENGTH];
    if (!seekNlines(linePos - 1, sourceptr))
    {
        printf("Line position too big\n");
        return false;
    }
    if (!seekNchars(charPos, true, sourceptr))
    {
        printf("Char position invalid\n");
        return false;
    }
    if (!isForward)
        seekNchars(size, false, sourceptr);
    char c;
    int i;
    for (i = 0; i < size && (c = fgetc(sourceptr)) != EOF; i++)
        copyingStr[i] = c;
    copyingStr[i] = '\0';
    copyStrToClipboard(copyingStr);
    fclose(sourceptr);
    return true;
}

bool cutFileContentToClipboard(char *fileName, int linePos, int charPos, int size, bool isForward)
{
    if (!copyFileContentToClipboard(fileName, linePos, charPos, size, isForward))
        return false;
    if (!removeText(fileName, linePos, charPos, size, isForward))
        return false;
    return true;
}

bool pasteFromClipboard(char *fileName, int linePos, int charPos)
{
    char clipboardText[MAX_STREAM_LENGTH];
    retrieveStrFromClipboard(clipboardText);
    if (!insertText(fileName, clipboardText, linePos, charPos))
        return false;
    return true;
}

void copyStrToClipboard(const char *str)
{
    const int len = strlen(str) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(char));
    memcpy(GlobalLock(hMem), str, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

void retrieveStrFromClipboard(char *str)
{
    OpenClipboard(0);
    HANDLE hClipboardData = GetClipboardData(CF_TEXT);
    char *pchData = (char *)GlobalLock(hClipboardData);
    strcpy(str, pchData);
    GlobalUnlock(hClipboardData);
    CloseClipboard();
}

void handleWildCards(char *str, bool *leadingWC, bool *endingWC)
{
    if (str[0] == '*')
    {
        *leadingWC = true;
        for (int i = 0; 1; i++)
        {
            str[i] = str[i + 1];
            if (str[i] == '\0')
                break;
        }
    }
    int length = strlen(str);
    if (str[length - 1] == '*' && str[length - 2] != '\\')
    {
        *endingWC = true;
        str[length - 1] = '\0';
    }
    // clearing the "\*"s
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\\' && str[i + 1] == '*')
        {
            for (int j = i; 1; j++)
            {
                str[j] = str[j + 1];
                if (str[j] == '\0')
                    break;
            }
        }
    }
}

void handleNewlines(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\\' && str[i + 1] == 'n')
        {
            str[i] = '\n';
            i++;
            for (int j = i; 1; j++)
            {
                str[j] = str[j + 1];
                if (str[j] == '\0')
                    break;
            }
        }
    }
}

void readAndWriteNlines(int n, FILE *tempptr, FILE *sourceptr)
{
    char c;
    int lineCounter = 1;
    while (lineCounter != n)
    {
        if (feof(sourceptr))
        {
            fprintf(tempptr, "\n");
            lineCounter++;
            continue;
        }
        while (1)
        {
            c = fgetc(sourceptr);
            if (c == EOF)
                break;
            if (c == '\n')
            {
                lineCounter++;
                break;
            }
            fprintf(tempptr, "%c", c);
        }
        if (!feof(sourceptr))
            fprintf(tempptr, "\n");
    }
}

bool readAndWriteNchars(int n, FILE *tempptr, FILE *sourceptr)
{
    char c;
    bool isEOL = false;
    int charCounter = 0;
    while (charCounter != n)
    {
        if (feof(sourceptr) || isEOL)
        {
            fprintf(tempptr, " ");
            charCounter++;
            continue;
        }
        c = fgetc(sourceptr);
        if (c == EOF)
            continue;
        if (c == '\n')
        {
            isEOL = true;
            continue;
        }
        fprintf(tempptr, "%c", c);
        charCounter++;
    }
    return isEOL;
}

bool seekNlines(int n, FILE *sourceptr)
{
    char c;
    int lineCounter = 0;
    while (lineCounter != n)
    {
        while (1)
        {
            c = fgetc(sourceptr);
            if (c == EOF)
                return false;
            if (c == '\n')
            {
                lineCounter++;
                break;
            }
        }
    }
    return true;
}

bool seekNchars(int n, bool isForward, FILE *sourceptr)
{
    int charCounter = 0;
    char c;
    if (isForward)
    {
        while (charCounter != n)
        {
            c = fgetc(sourceptr);
            charCounter++;
            if (c == EOF)
                return false;
        }
    }
    else
    {
        while (charCounter != n)
        {
            if (fseek(sourceptr, -1, SEEK_CUR) != 0)
                return false;
            c = fgetc(sourceptr);
            if (c == '\n')
                fseek(sourceptr, -2, SEEK_CUR);
            else
                fseek(sourceptr, -1, SEEK_CUR);
            charCounter++;
        }
    }
    return true;
}

void writeStrToFile(char *text, bool isEOL, FILE *tempptr, FILE *sourceptr)
{
    bool backSlashMode = false;
    for (int i = 0; text[i] != '\0'; i++)
    {
        if (backSlashMode)
        {
            if (text[i] == '\\')
                fprintf(tempptr, "\\");
            else if (text[i] == 'n')
                fprintf(tempptr, "\n");
            backSlashMode = false;
            continue;
        }
        if (text[i] == '\\')
        {
            backSlashMode = true;
            continue;
        }
        if (text[i] == '\r')
            continue;
        fprintf(tempptr, "%c", text[i]);
    }
    if (isEOL)
        fprintf(tempptr, "\n");
}

bool createFileAndDirs(char *fileName)
{
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

void fileToString(FILE *fp, char *str)
{
    char c;
    int i = 0;
    while ((c = fgetc(fp)) != EOF)
        str[i++] = c;
    str[i] = '\0';
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

bool findMatchFromIndex(const char *str, const char *match, int startingIndex, bool isForward)
{
    int matchIndex, iterator = 1;
    int matchLen = strlen(match);
    if (isForward)
        matchIndex = 0;
    else
    {
        iterator = -1;
        matchIndex = matchLen - 1;
    }
    for (int i = startingIndex; matchLen != 0; matchLen--)
    {
        if (i == -1 || matchIndex == -1 || match[matchIndex] == '\0' || str[i] == '\0')
            return false;
        if (str[i] != match[matchIndex])
            return false;
        i += iterator;
        matchIndex += iterator;
    }
    return true;
}

void copyStringRange(char *dest, const char *source, int start, int end)
{
    if (end == -1)
        end = MAX_STREAM_LENGTH;
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
    int start, end;
    if (!findNthWord(str, n, &start, &end))
        return false;
    copyStringRange(dest, str, start, end + 1);
    return true;
}

bool findNthWord(const char *str, int n, int *startIndex, int *endIndex)
{
    int wordNum = 1;
    int i = 0;
    while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
        i++;
    while (wordNum < n)
    {
        if (str[i] == '\0')
            return false;
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
        {
            while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
                i++;
            wordNum++;
            if (str[i] == '\0')
                return false;
            continue;
        }
        i++;
    }
    *startIndex = i;
    while (str[i] != ' ' && str[i] != '\0' && str[i] != '\n' && str[i] != '\t')
        i++;
    *endIndex = i - 1;
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
