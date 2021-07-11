/**
 * @file CPU_6502.c
 * @author Evan Stoddard
 * @brief 
 */

#include "CPU_6502.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void CPU_6502_Init(CPU_6502 * cpu)
{
    // Initialize  Registers
    cpu->registers.A = 0;
    cpu->registers.X = 0;
    cpu->registers.Y = 0;

    // Initialize PC and SP
    cpu->PC = 0;
    cpu->SP = 0;

    // Initialize status flags
    cpu->status = 0;

    // Initialize bus pointer
    cpu->bus = NULL;
}

void CPU_6502_Connect_Bus(CPU_6502 * cpu, void * bus)
{
    // Set CPUs bus pointer
    cpu->bus = bus;
}

void CPU_6502_Reset(CPU_6502 * cpu)
{
    // Set PC and SP
    cpu->PC = CPU_6502_RESET_VECTOR;
    cpu->SP = 0xFF;

    // Set Registers
    cpu->registers.A = 0;
    cpu->registers.X = 0;
    cpu->registers.Y = 0;

    // Set Status flags
}

void CPU_6502_Clock(CPU_6502 * cpu)
{
    
}