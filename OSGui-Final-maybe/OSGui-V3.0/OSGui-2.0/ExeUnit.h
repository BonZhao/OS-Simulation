/**
 * MyOS\ExeUnit.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/31 21:48		Created				by LyanQQ ;
 * 2016/03/31 22:11		1st time coding		by LyanQQ :
 * 		-Data structure completed.
 */

#ifndef EXEUNIT_H
#define EXEUNIT_H

#include "ProcessManagement.h"

/**
 * ÷∏¡Ó¿‡–Õ
 */
#define INS_NUMBER 12

enum InstructionType {CONFIG, NORMAL, MEM_MALLOC, MEM_FREE_, PRINT, IO, END, ADD, SUB, JUMP, C_JUMP, SAVE, NONE};

struct Statement {
    InstructionType type ;
    int var_1 ;
    int var_2 ;
} ;

bool getInstruction(unsigned char * __left, unsigned char * __right);
bool readMemoryUnit(PCB * __pcb, int __label, int __offset, unsigned char * __val);
bool writeMemoryUnit(PCB * __pcb, int __label, int __offset, unsigned char __val);
Statement decode(unsigned char __left, unsigned char __right);
void execute(unsigned char __left, unsigned char __right);
void startExeUnit();
void shutDownExeUnit();

#endif
