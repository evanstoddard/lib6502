/**
 * @file CPU_6502.h
 * @author Evan Stoddard
 * @brief 
 */

#ifndef CPU_6502_H_
#define CPU_6502_H_

#include <stdint.h>
#include <stdlib.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define CPU_6502_RESET_VECTOR 0xFFFC

#define CPU_6502_STATUS_NEGATIVE 7
#define CPU_6502_STATUS_OVERFLOW 6
#define CPU_6502_STATUS_B_HIGH   5
#define CPU_6502_STATUS_B_LOW    4
#define CPU_6502_STATUS_DECIMAL  3
#define CPU_6502_STATUS_INT      2
#define CPU_6502_STATUS_ZERO     1
#define CPU_6502_STATUS_CARRY    0

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct 
{
    uint8_t A;
    uint8_t X;
    uint8_t Y;
} CPU_6502_Registers;

typedef struct 
{
    uint16_t (*immediate)(void * cpu);
    uint16_t (*zeroPageAddr)(void * cpu);
    uint16_t (*zeroPageXAddr)(void * cpu);
    uint16_t (*zeroPageYAddr)(void * cpu);
    uint16_t (*absoluteAddr)(void * cpu);
    uint16_t (*absoluteXAddr)(void * cpu);
    uint16_t (*absoluteYAddr)(void * cpu);
    uint16_t (*indirectXAddr)(void * cpu);
    uint16_t (*indirectYAddr)(void * cpu);
    uint16_t (*indirectAddr)(void * cpu);
    uint16_t (*relativeAddr)(void * cpu);
    uint16_t (*accumulatorAddr)(void * cpu);
} CPU_6502_Address_Modes;

typedef struct 
{
    uint8_t opCode;
    uint8_t cycles;
    void (*opFunction)(void * cpu, void * op);
    uint16_t (*addrMode)(void * cpu);
} CPU_6502_Operation;

typedef struct 
{
    CPU_6502_Address_Modes addressModes;
    CPU_6502_Registers registers;

    uint8_t status;
    uint16_t PC;
    uint8_t SP;

    void * bus;

    CPU_6502_Operation operations[0x100];
    uint8_t cyclesRemaining;
} CPU_6502;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize CPU object
 * 
 * @param cpu cpu object
 */
void CPU_6502_Init(CPU_6502 * cpu);

/**
 * @brief Connect CPU to bus
 * 
 * @param cpu cpu object
 * @param bus pointer to cpu bus
 */
void CPU_6502_Connect_Bus(CPU_6502 * cpu, void * bus);

/**
 * @brief Reset CPU
 * 
 * @param cpu cpu object
 */
void CPU_6502_Reset(CPU_6502 * cpu);

/**
 * @brief Clock CPU
 * 
 * @param cpu cpu object
 */
void CPU_6502_Clock(CPU_6502 * cpu);

/**
 * @brief performs interrupt if enabled
 * 
 * @param cpu cpu object
 */
void CPU_6502_IRQ(CPU_6502 * cpu);

/**
 * @brief performs interrupt regardless of interrupts enabled
 * 
 * @param cpu cpu object
 */
void CPU_6502_NMI(CPU_6502 * cpu);

#endif /* CPU_6502_H_ */