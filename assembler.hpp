#include <cstdio>
#include <cstddef>

#define CONTINUE_IFN0(code) {      \
    ErrorCodes errorCode = (code); \
    if (errorCode) {               \
        return errorCode;          \
    }                              \
}

struct stringData {
    char *str = nullptr;
    size_t length = 0;
};

enum ErrorCodes {
    OKAY = 0,
    ARGUMENTS_HANDLING_ERROR,
    FILE_OPENING_ERROR,
    SIZE_GETTING_ERROR,
    MEM_ALLOCATION_ERROR,
    UNKNOWN_COMMAND,
};

//переедет в отдельынй файл
//typedef unsigned char processorCmd_t;
// тут есть какой-то баг, а ещё санитайзер ругается..
typedef int32_t processorData_t;

//хочется union + bitfields c int
const processorData_t IMMEDIATE_CONST_CMD = 1;
const processorData_t REGISTER_CMD        = 2;
const processorData_t MEMORY_CMD          = 4;
const processorData_t COMMAND_VALUE_CMD   = 224;
const processorData_t COMMAND_OFFSET_CMD  = 3;

const processorData_t REGISTER_VALUE_ARG  = 1 << 6;
const processorData_t REGISTER_OFFSET_ARG = 6;

const size_t MAX_INPUT_STR_LEN    = 100;
const size_t MAX_OUTPUT_ARRAY_LEN = 1000;

#define DEFINE_CMD_(name, index, nArguments, code) name = (index),
enum Commands {
    #include "../commands.hpp"
};
#undef DEFINE_CMD_

size_t mapTextToCodes(stringData *strings, size_t nStrings, processorData_t outputArray[]);
ErrorCodes parseArgument(char *string, processorData_t *idxCmd, processorData_t *arg, size_t *nChanged);
ErrorCodes saveArrayToFile(processorData_t outputArray[], size_t nArrayElements, char *outFilePath);
ErrorCodes getRawDataFromFile(const char *inpFilePath, char **rawData, stringData **strings, size_t *nStrings);
size_t replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(char *rawData, stringData strings[], size_t szFile);
long getFileSize(FILE *inpFile);
void readDataFromFile(FILE *inpFile, char *rawData, size_t *szFile);