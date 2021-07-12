/**
 * @file CPU_6502.c
 * @author Evan Stoddard
 * @brief 
 */

#include "CPU_6502.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/
#define true 1U
#define false 0U

#define INST(cpu) ((CPU_6502 *)cpu)
#define MEM(cpu) ((uint8_t *)INST(cpu)->bus)
typedef uint8_t bool;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

static uint8_t fetchByte(CPU_6502 * cpu);
static uint8_t fetchFromMemory(CPU_6502 * cpu, CPU_6502_Operation op);

// Default Instructions
static void LDA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void LDX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void LDY(CPU_6502 * cpu, CPU_6502_Operation * op);

static void STA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void STX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void STY(CPU_6502 * cpu, CPU_6502_Operation * op);

static void AND(CPU_6502 * cpu, CPU_6502_Operation * op);
static void ORA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void EOR(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BIT(CPU_6502 * cpu, CPU_6502_Operation * op);

static void ROL(CPU_6502 * cpu, CPU_6502_Operation * op);
static void ROR(CPU_6502 * cpu, CPU_6502_Operation * op);
static void ASL(CPU_6502 * cpu, CPU_6502_Operation * op);
static void LSR(CPU_6502 * cpu, CPU_6502_Operation * op);

static void CMP(CPU_6502 * cpu, CPU_6502_Operation * op);
static void CPX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void CPY(CPU_6502 * cpu, CPU_6502_Operation * op);

static void INC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void INX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void INY(CPU_6502 * cpu, CPU_6502_Operation * op);
static void DEC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void DEX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void DEY(CPU_6502 * cpu, CPU_6502_Operation * op);

static void ADC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void SBC(CPU_6502 * cpu, CPU_6502_Operation * op);

static void TAX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void TAY(CPU_6502 * cpu, CPU_6502_Operation * op);
static void TXA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void TYA(CPU_6502 * cpu, CPU_6502_Operation * op);

static void JMP(CPU_6502 * cpu, CPU_6502_Operation * op);
static void JSR(CPU_6502 * cpu, CPU_6502_Operation * op);
static void RTS(CPU_6502 * cpu, CPU_6502_Operation * op);

static void BCC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BCS(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BEQ(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BMI(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BNE(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BPL(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BVC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void BVS(CPU_6502 * cpu, CPU_6502_Operation * op);

static void CLC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void CLD(CPU_6502 * cpu, CPU_6502_Operation * op);
static void CLI(CPU_6502 * cpu, CPU_6502_Operation * op);
static void CLV(CPU_6502 * cpu, CPU_6502_Operation * op);

static void SEC(CPU_6502 * cpu, CPU_6502_Operation * op);
static void SED(CPU_6502 * cpu, CPU_6502_Operation * op);
static void SEI(CPU_6502 * cpu, CPU_6502_Operation * op);

static void TSX(CPU_6502 * cpu, CPU_6502_Operation * op);
static void TXS(CPU_6502 * cpu, CPU_6502_Operation * op);
static void PHA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void PHP(CPU_6502 * cpu, CPU_6502_Operation * op);
static void PLA(CPU_6502 * cpu, CPU_6502_Operation * op);
static void PLP(CPU_6502 * cpu, CPU_6502_Operation * op);

static void BRK(CPU_6502 * cpu, CPU_6502_Operation * op);
static void RTI(CPU_6502 * cpu, CPU_6502_Operation * op);

static void NOP(CPU_6502 * cpu, CPU_6502_Operation * op);

// Default Address modes
static uint16_t immediate(void * cpu);
static uint16_t zeroPageAddr(void * cpu);
static uint16_t zeroPageXAddr(void * cpu);
static uint16_t zeroPageYAddr(void * cpu);
static uint16_t absoluteAddr(void * cpu);
static uint16_t absoluteXAddr(void * cpu);
static uint16_t absoluteYAddr(void * cpu);
static uint16_t indirectXAddr(void * cpu);
static uint16_t indirectYAddr(void * cpu);
static uint16_t indirectAddr(void * cpu);
static uint16_t relativeAddr(void * cpu);
static uint16_t accumlatorAddr(void * cpu);

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

    // Setup default addressing modes
    cpu->addressModes.immediate = immediate;
    cpu->addressModes.zeroPageAddr = zeroPageAddr;
    cpu->addressModes.zeroPageXAddr = zeroPageXAddr;
    cpu->addressModes.zeroPageYAddr = zeroPageYAddr;
    cpu->addressModes.absoluteAddr = absoluteAddr;
    cpu->addressModes.absoluteXAddr = absoluteXAddr;
    cpu->addressModes.absoluteYAddr = absoluteYAddr;
    cpu->addressModes.indirectXAddr = indirectXAddr;
    cpu->addressModes.indirectYAddr = indirectYAddr;
    cpu->addressModes.indirectAddr = indirectAddr;
    cpu->addressModes.relativeAddr = relativeAddr;
    cpu->addressModes.accumulatorAddr = accumlatorAddr;

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

void CPU_6502_IRQ(CPU_6502 * cpu)
{

}

void CPU_6502_NMI(CPU_6502 * cpu)
{

}

/**
 * @brief returns byte at address of PC and increments PC
 * 
 * @param cpu cpu object
 * @return uint8_t byte at PC when function called
 */
static uint8_t fetchByte(CPU_6502 * cpu)
{
    // Grab value from memory at PC and increment PC
    uint8_t value = MEM(cpu)[INST(cpu)->PC];
    INST(cpu)->PC++;

    return value;
}

/**
 * @brief performs addressing operation of function and returns value at address
 * 
 * @param cpu cpu object
 * @param op instruction fetching value from memory
 * @return uint8_t value at memory
 */
static uint8_t fetchFromMemory(CPU_6502 * cpu, CPU_6502_Operation op)
{
    // Perform addressing operation and return value in memory
    if (op.addrMode != INST(cpu)->addressModes.immediate)
        return MEM(cpu)[op.addrMode(cpu)];
    else
        return op.addrMode(cpu);
}


// Addressing Modes

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t immediate(void * cpu)
{
    return fetchByte(cpu);
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t zeroPageAddr(void * cpu)
{
    return fetchByte(cpu);
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t zeroPageXAddr(void * cpu)
{
    return (uint8_t)(fetchByte(cpu) + INST(cpu)->registers.X);
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t zeroPageYAddr(void * cpu)
{
    return (uint8_t)(fetchByte(cpu) + INST(cpu)->registers.Y);
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t absoluteAddr(void * cpu)
{
    // Fetch 16-bit address
    return fetchByte(cpu) + (fetchByte(cpu) << 8);
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t absoluteXAddr(void * cpu)
{
    // Fetch 16-bit address and add offset from X register
    uint16_t addr  = fetchByte(cpu);
             addr += (fetchByte(cpu) << 8);
             addr += INST(cpu)->registers.X;

    return addr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t absoluteYAddr(void * cpu)
{
    // Fetch 16-bit address and add offset from Y register
    uint16_t addr  = fetchByte(cpu);
             addr += (fetchByte(cpu) << 8);
             addr += INST(cpu)->registers.Y;
            
    return addr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t indirectXAddr(void * cpu)
{
    // Fetch offset and add value in register X
    uint8_t zeroAddr =  fetchByte(cpu);
            zeroAddr += INST(cpu)->registers.X;

    // Read 16-bit address from zero page address
    uint16_t effectiveAddr =  MEM(cpu)[zeroAddr];
             effectiveAddr += (MEM(cpu)[zeroAddr + 1] << 8);

    return effectiveAddr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t indirectYAddr(void * cpu)
{
    // Feth 16-bit address from zero page memory
    uint8_t offset = fetchByte(cpu);
    
    uint16_t effectiveAddr =  MEM(cpu)[offset];
             effectiveAddr += (MEM(cpu)[offset + 1] << 8);

    // Add Y to effective address
    effectiveAddr += INST(cpu)->registers.Y;

    return effectiveAddr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t indirectAddr(void * cpu)
{
    /* 
     * Fetch 16-bit address from address specified by passed in address
     * Note.  This implementation contains the page boundary bug in the
     * Original 6502.  See http://obelisk.me.uk/6502/reference.html#JMP
     * for details.
     **/

    uint8_t LB = fetchByte(cpu);
    uint8_t UB = fetchByte(cpu);

    // Get final address
    uint16_t memAddr = LB + (UB << 8);
    uint16_t effectiveAddr = MEM(cpu)[memAddr];

    // Implement page boundary bug
    if (LB == 0xFF) effectiveAddr += (MEM(cpu)[(memAddr & 0xFF00)] << 8);
    else effectiveAddr += (MEM(cpu)[memAddr + 1] << 8);

    return effectiveAddr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t relativeAddr(void * cpu)
{
    // Fetch signed byte and add to PC
    int8_t offset = fetchByte(cpu);

    uint16_t addr = INST(cpu)->PC + offset;

    return addr;
}

/**
 * @brief 
 * 
 * @param cpu 
 * @return uint16_t 
 */
static uint16_t accumlatorAddr(void * cpu)
{
    return INST(cpu)->registers.A;
}

