// name, index, argType, code
DEFINE_CMD_(hlt, 0, WO_ARGUMENTS, {
    printf("hlt\n");
})
DEFINE_CMD_(push, 1, WITH_NUMERIC_ARGUMENT, ({
    if ((codeArr[instuctionPtr] & FLAGS_RANGE_CMD) == IMMEDIATE_CONST_CMD) {
        stackPush(&stack, codeArr[instuctionPtr + 1]);
    }
    else if (processorData_t *pushArg = getPtrWrtFlags(codeArr + instuctionPtr, registers, ram)) {
        stackPush(&stack, *pushArg);
    }
    else {
        ABORT_WITH_PRINTF(("Unknown flag set. Command = %d\n", codeArr[instuctionPtr]));
    }

    instuctionPtr += 2;
}))
DEFINE_CMD_(pop, 2, WITH_NUMERIC_ARGUMENT, ({
    if ((codeArr[instuctionPtr] & FLAGS_RANGE_CMD) == 0) {
        processorData_t value;
        stackPop(&stack, &value);
    }
    else if (processorData_t *popArg = getPtrWrtFlags(codeArr + instuctionPtr, registers, ram)) {
        stackPop(&stack, popArg);
    }
    else {
        ABORT_WITH_PRINTF(("Unknown flag set. Command = %d\n", codeArr[instuctionPtr]));
    }

    instuctionPtr += 2;
}))
DEFINE_CMD_(mul, 3, WO_ARGUMENTS, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    stackPush(&stack, a * b);
    ++instuctionPtr;
}))
DEFINE_CMD_(add, 4, WO_ARGUMENTS, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    stackPush(&stack, a + b);
    ++instuctionPtr;
}))
DEFINE_CMD_(sub, 5, WO_ARGUMENTS, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    stackPush(&stack, b - a);
    ++instuctionPtr;
}))
DEFINE_CMD_(in, 6, WO_ARGUMENTS, ({
    processorData_t a = 0;
    scanf("%d", &a);
    stackPush(&stack, a);
    ++instuctionPtr;
}))
DEFINE_CMD_(out, 7, WO_ARGUMENTS, ({
    processorData_t a = 0;
    stackPop(&stack, &a);
    printf("%d\n", a);
    ++instuctionPtr;
}))
DEFINE_CMD_(jmp, 8, JMP_TYPE, ({
    instuctionPtr = codeArr[instuctionPtr + 1];
}))
DEFINE_CMD_(ja, 9, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b > a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(jae, 10, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b >= a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(jb, 11, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b < a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(jbe, 12, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b <= a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(je, 13, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b == a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(jne, 14, JMP_TYPE, ({
    processorData_t a = 0, b = 0;
    stackPop(&stack, &a);
    stackPop(&stack, &b);
    if (b != a) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(js, 15, JMP_TYPE, ({
    if (rand() % 2) {
        instuctionPtr = codeArr[instuctionPtr + 1];
    }
    else {
        instuctionPtr += 2;
    }
}))
DEFINE_CMD_(call, 16, JMP_TYPE, ({
    stackPush(&stack, instuctionPtr + 2);
    instuctionPtr = codeArr[instuctionPtr + 1];
}))
DEFINE_CMD_(ret, 17, WO_ARGUMENTS, ({
    processorData_t a = 0;
    stackPop(&stack, &a);
    instuctionPtr = a;
}))