#define POP_IN_A_B                  \
    processorData_t a = 0, b = 0;   \
    stackPop(&stack, &a);           \
    stackPop(&stack, &b);          

#define POP_IN_A_B_AND_INC_PTR      \
    POP_IN_A_B;                     \
    ++instructionPtr;            


// name, index, argType, code
DEFINE_CMD_(hlt, 0, WO_ARGUMENTS, {})
DEFINE_CMD_(push, 1, WITH_NUMERIC_ARGUMENT, ({
    if ((codeArr[instructionPtr] & FLAGS_RANGE_CMD) == IMMEDIATE_CONST_CMD) {
        stackPush(&stack, codeArr[instructionPtr + 1]);
    }
    else if (processorData_t *pushArg = getPtrWrtFlags(codeArr + instructionPtr, registers, ram)) {
        stackPush(&stack, *pushArg);
    }
    else {
        ABORT_WITH_PRINTF(("Unknown flag set. Command = %d\n", codeArr[instructionPtr]));
    }

    instructionPtr += 2;
}))
DEFINE_CMD_(pop, 2, WITH_NUMERIC_ARGUMENT, ({
    if ((codeArr[instructionPtr] & FLAGS_RANGE_CMD) == 0) {
        processorData_t value;
        stackPop(&stack, &value);
    }
    else if (processorData_t *popArg = getPtrWrtFlags(codeArr + instructionPtr, registers, ram)) {
        stackPop(&stack, popArg);
    }
    else {
        ABORT_WITH_PRINTF(("Unknown flag set. Command = %d\n", codeArr[instructionPtr]));
    }

    instructionPtr += 2;
}))
DEFINE_CMD_(mul, 3, WO_ARGUMENTS, ({
    POP_IN_A_B_AND_INC_PTR;
    stackPush(&stack, a * b);
}))
DEFINE_CMD_(add, 4, WO_ARGUMENTS, ({
    POP_IN_A_B_AND_INC_PTR;
    stackPush(&stack, a + b);
}))
DEFINE_CMD_(sub, 5, WO_ARGUMENTS, ({
    POP_IN_A_B_AND_INC_PTR;
    stackPush(&stack, b - a);
}))
DEFINE_CMD_(in, 6, WO_ARGUMENTS, ({
    processorData_t a = 0;
    scanf("%d", &a);
    stackPush(&stack, a);
    ++instructionPtr;
}))
DEFINE_CMD_(out, 7, WO_ARGUMENTS, ({
    processorData_t a = 0;
    stackPop(&stack, &a);
    printf("%d\n", a);
    ++instructionPtr;
}))
DEFINE_CMD_(jmp, 8, JMP_TYPE, ({
    instructionPtr = codeArr[instructionPtr + 1];
}))
DEFINE_CMD_(ja, 9, JMP_TYPE, ({
    POP_IN_A_B;
    if (b > a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(jae, 10, JMP_TYPE, ({
    POP_IN_A_B;
    if (b >= a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(jb, 11, JMP_TYPE, ({
    POP_IN_A_B;
    if (b < a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(jbe, 12, JMP_TYPE, ({
    POP_IN_A_B;
    if (b <= a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(je, 13, JMP_TYPE, ({
    POP_IN_A_B;
    if (b == a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(jne, 14, JMP_TYPE, ({
    POP_IN_A_B;
    if (b != a) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(js, 15, JMP_TYPE, ({
    if (rand() % 2) {
        instructionPtr = codeArr[instructionPtr + 1];
    }
    else {
        instructionPtr += 2;
    }
}))
DEFINE_CMD_(call, 16, JMP_TYPE, ({
    stackPush(&stack, instructionPtr + 2);
    instructionPtr = codeArr[instructionPtr + 1];
}))
DEFINE_CMD_(ret, 17, WO_ARGUMENTS, ({
    processorData_t a = 0;
    stackPop(&stack, &a);
    instructionPtr = a;
}))