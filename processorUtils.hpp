#include <cstdint>

// #define DEFINE_CMD_(name, index, nArguments, code) name = (index),
// enum Commands {
//     #include "commands.hpp"
// };
// #undef DEFINE_CMD_

enum CmdTypes {
    WO_ARGUMENTS,
    WITH_NUMERIC_ARGUMENT,
    JMP_TYPE,
};

//typedef unsigned char processorCmd_t;
// тут есть какой-то баг, а ещё санитайзер ругается..
typedef int32_t processorData_t;

//хочется union + bitfields c int
const processorData_t IMMEDIATE_CONST_CMD = 1;
const processorData_t REGISTER_CMD        = 2;
const processorData_t MEMORY_CMD          = 4;
const processorData_t COMMAND_VALUE_CMD   = 224;
const processorData_t FLAGS_RANGE_CMD     = IMMEDIATE_CONST_CMD | REGISTER_CMD | MEMORY_CMD;
const processorData_t COMMAND_OFFSET_CMD  = 3;

const processorData_t REGISTER_VALUE_ARG       = 1 << 6;
const processorData_t ARGUMENT_VALUE_RANGE_ARG = REGISTER_VALUE_ARG - 1;
const processorData_t REGISTER_OFFSET_ARG      = 6;
const size_t N_REGISTERS = 4;