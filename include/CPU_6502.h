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

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef struct {
    uint8_t A;
    uint8_t X;
    uint8_t Y;
} CPU_6502_Registers;

typedef struct {
    CPU_6502_Registers registers;
    uint8_t status;
    uint16_t PC;
    uint8_t SP;
    void * bus;
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



#endif /* CPU_6502_H_ */