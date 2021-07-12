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
#define OP(op) ((CPU_6502_Operation *) op)
typedef uint8_t bool;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

static void initializeOperations(CPU_6502 * cpu);

static uint8_t fetchByte(CPU_6502 * cpu);
static uint8_t fetchFromMemory(CPU_6502 * cpu, CPU_6502_Operation * op);

static void setStatusFlag(CPU_6502 * cpu, uint8_t flag, uint8_t val);
static bool getStatusFlag(CPU_6502 * cpu, uint8_t flag);

// Default Operations
static void LDA(void * cpu, void * op);
static void LDX(void * cpu, void * op);
static void LDY(void * cpu, void * op);

static void STA(void * cpu, void * op);
static void STX(void * cpu, void * op);
static void STY(void * cpu, void * op);

static void AND(void * cpu, void * op);
static void ORA(void * cpu, void * op);
static void EOR(void * cpu, void * op);
static void BIT(void * cpu, void * op);

static void ROL(void * cpu, void * op);
static void ROR(void * cpu, void * op);
static void ASL(void * cpu, void * op);
static void LSR(void * cpu, void * op);

static void CMP(void * cpu, void * op);
static void CPX(void * cpu, void * op);
static void CPY(void * cpu, void * op);

static void INC(void * cpu, void * op);
static void INX(void * cpu, void * op);
static void INY(void * cpu, void * op);
static void DEC(void * cpu, void * op);
static void DEX(void * cpu, void * op);
static void DEY(void * cpu, void * op);

static void ADC(void * cpu, void * op);
static void SBC(void * cpu, void * op);

static void TAX(void * cpu, void * op);
static void TAY(void * cpu, void * op);
static void TXA(void * cpu, void * op);
static void TYA(void * cpu, void * op);

static void JMP(void * cpu, void * op);
static void JSR(void * cpu, void * op);
static void RTS(void * cpu, void * op);

static void BCC(void * cpu, void * op);
static void BCS(void * cpu, void * op);
static void BEQ(void * cpu, void * op);
static void BMI(void * cpu, void * op);
static void BNE(void * cpu, void * op);
static void BPL(void * cpu, void * op);
static void BVC(void * cpu, void * op);
static void BVS(void * cpu, void * op);

static void CLC(void * cpu, void * op);
static void CLD(void * cpu, void * op);
static void CLI(void * cpu, void * op);
static void CLV(void * cpu, void * op);

static void SEC(void * cpu, void * op);
static void SED(void * cpu, void * op);
static void SEI(void * cpu, void * op);

static void TSX(void * cpu, void * op);
static void TXS(void * cpu, void * op);
static void PHA(void * cpu, void * op);
static void PHP(void * cpu, void * op);
static void PLA(void * cpu, void * op);
static void PLP(void * cpu, void * op);

static void BRK(void * cpu, void * op);
static void RTI(void * cpu, void * op);

static void NOP(void * cpu, void * op);

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
static uint16_t accumulatorAddr(void * cpu);

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
    cpu->addressModes.accumulatorAddr = accumulatorAddr;

    // Initialize operations
    initializeOperations(cpu);

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

void initializeOperations(CPU_6502 * cpu)
{
    // Initialize operations to NOPs
    for (uint16_t i = 0; i < 0xFF; i++)
    {
        INST(cpu)->operations[i] = (CPU_6502_Operation){i, 2,  NOP, NULL};
    }

    // Set default operations

    // LDA Instructions
    INST(cpu)->operations[0xA9] = (CPU_6502_Operation){0xA9, 2, LDA, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xA5] = (CPU_6502_Operation){0xA5, 3, LDA, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xB5] = (CPU_6502_Operation){0xB5, 4, LDA, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xAD] = (CPU_6502_Operation){0xAD, 4, LDA, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xBD] = (CPU_6502_Operation){0xBD, 4, LDA, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0xB9] = (CPU_6502_Operation){0xB9, 4, LDA, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0xA1] = (CPU_6502_Operation){0xA1, 6, LDA, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0xB1] = (CPU_6502_Operation){0xB1, 5, LDA, INST(cpu)->addressModes.indirectYAddr};

    // LDX Instructions
    INST(cpu)->operations[0xA2] = (CPU_6502_Operation){0xA2, 2, LDX, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xA6] = (CPU_6502_Operation){0xA6, 3, LDX, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xB6] = (CPU_6502_Operation){0xB6, 4, LDX, INST(cpu)->addressModes.zeroPageYAddr};
    INST(cpu)->operations[0xAE] = (CPU_6502_Operation){0xAE, 4, LDX, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xBE] = (CPU_6502_Operation){0xBE, 4, LDX, INST(cpu)->addressModes.absoluteYAddr};

    // LDY Instructions
    INST(cpu)->operations[0xA0] = (CPU_6502_Operation){0xA0, 2, LDY , INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xA4] = (CPU_6502_Operation){0xA4, 3, LDY , INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xB4] = (CPU_6502_Operation){0xB4, 4, LDY , INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xAC] = (CPU_6502_Operation){0xAC, 4, LDY , INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xBC] = (CPU_6502_Operation){0xBC, 4, LDY , INST(cpu)->addressModes.absoluteXAddr};

    // STA Instructions
    INST(cpu)->operations[0x85] = (CPU_6502_Operation){0x85, 3, STA, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x95] = (CPU_6502_Operation){0x95, 4, STA, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x8D] = (CPU_6502_Operation){0x8D, 4, STA, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x9D] = (CPU_6502_Operation){0x9D, 5, STA, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0x99] = (CPU_6502_Operation){0x99, 5, STA, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0x81] = (CPU_6502_Operation){0x81, 6, STA, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0x91] = (CPU_6502_Operation){0x91, 6, STA, INST(cpu)->addressModes.indirectYAddr};

    // STX Instructions
    INST(cpu)->operations[0x86] = (CPU_6502_Operation){0x86, 3, STX, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x96] = (CPU_6502_Operation){0x96, 4, STX, INST(cpu)->addressModes.zeroPageYAddr};
    INST(cpu)->operations[0x8E] = (CPU_6502_Operation){0x8E, 4, STX, INST(cpu)->addressModes.absoluteAddr};
     
    // STY Instructions
    INST(cpu)->operations[0x84] = (CPU_6502_Operation){0x84, 3, STY, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x94] = (CPU_6502_Operation){0x94, 4, STY, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x8C] = (CPU_6502_Operation){0x8C, 4, STY, INST(cpu)->addressModes.absoluteAddr};

    // AND Instructions
    INST(cpu)->operations[0x29] = (CPU_6502_Operation){0x29, 2, AND, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0x25] = (CPU_6502_Operation){0x25, 3, AND, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x35] = (CPU_6502_Operation){0x35, 4, AND, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x2D] = (CPU_6502_Operation){0x2D, 4, AND, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x3D] = (CPU_6502_Operation){0x3D, 4, AND, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0x39] = (CPU_6502_Operation){0x39, 4, AND, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0x21] = (CPU_6502_Operation){0x21, 6, AND, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0x31] = (CPU_6502_Operation){0x31, 5, AND, INST(cpu)->addressModes.indirectYAddr};

    // OR Instructions
    INST(cpu)->operations[0x09] = (CPU_6502_Operation){0x09, 2, ORA, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0x05] = (CPU_6502_Operation){0x05, 3, ORA, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x15] = (CPU_6502_Operation){0x15, 4, ORA, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x0D] = (CPU_6502_Operation){0x0D, 4, ORA, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x1D] = (CPU_6502_Operation){0x1D, 4, ORA, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0x19] = (CPU_6502_Operation){0x19, 4, ORA, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0x01] = (CPU_6502_Operation){0x01, 6, ORA, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0x11] = (CPU_6502_Operation){0x11, 5, ORA, INST(cpu)->addressModes.indirectYAddr};

    // EOR Instructions
    INST(cpu)->operations[0x49] = (CPU_6502_Operation){0x49, 2, EOR, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0x45] = (CPU_6502_Operation){0x45, 3, EOR, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x55] = (CPU_6502_Operation){0x55, 4, EOR, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x4D] = (CPU_6502_Operation){0x4D, 4, EOR, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x5D] = (CPU_6502_Operation){0x5D, 4, EOR, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0x59] = (CPU_6502_Operation){0x59, 4, EOR, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0x41] = (CPU_6502_Operation){0x41, 6, EOR, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0x51] = (CPU_6502_Operation){0x51, 5, EOR, INST(cpu)->addressModes.indirectYAddr};

    // ROL Instructions
    INST(cpu)->operations[0x2A] = (CPU_6502_Operation){0x2A, 2, ROL, INST(cpu)->addressModes.accumulatorAddr};
    INST(cpu)->operations[0x26] = (CPU_6502_Operation){0x26, 5, ROL, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x36] = (CPU_6502_Operation){0x36, 6, ROL, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x2E] = (CPU_6502_Operation){0x2E, 6, ROL, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x3E] = (CPU_6502_Operation){0x3E, 7, ROL, INST(cpu)->addressModes.absoluteXAddr};

    // ROR Instructions
    INST(cpu)->operations[0x6A] = (CPU_6502_Operation){0x6A, 2, ROR, INST(cpu)->addressModes.accumulatorAddr};
    INST(cpu)->operations[0x66] = (CPU_6502_Operation){0x66, 5, ROR, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x76] = (CPU_6502_Operation){0x76, 6, ROR, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x6E] = (CPU_6502_Operation){0x6E, 6, ROR, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x7E] = (CPU_6502_Operation){0x7E, 7, ROR, INST(cpu)->addressModes.absoluteXAddr};

    // ASL Instructions
    INST(cpu)->operations[0x0A] = (CPU_6502_Operation){0x0A, 2, ASL, INST(cpu)->addressModes.accumulatorAddr};
    INST(cpu)->operations[0x06] = (CPU_6502_Operation){0x06, 5, ASL, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x16] = (CPU_6502_Operation){0x16, 6, ASL, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x0E] = (CPU_6502_Operation){0x0E, 6, ASL, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x1E] = (CPU_6502_Operation){0x1E, 7, ASL, INST(cpu)->addressModes.absoluteXAddr};

    // LSR Instructions
    INST(cpu)->operations[0x4A] = (CPU_6502_Operation){0x4A, 2, LSR, INST(cpu)->addressModes.accumulatorAddr};
    INST(cpu)->operations[0x46] = (CPU_6502_Operation){0x46, 5, LSR, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x56] = (CPU_6502_Operation){0x56, 6, LSR, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x4E] = (CPU_6502_Operation){0x4E, 6, LSR, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x5E] = (CPU_6502_Operation){0x5E, 7, LSR, INST(cpu)->addressModes.absoluteXAddr};

    // BIT Instructions
    INST(cpu)->operations[0x24] = (CPU_6502_Operation){0x24, 3, BIT, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x2C] = (CPU_6502_Operation){0x2C, 4, BIT, INST(cpu)->addressModes.absoluteAddr};

    // CMP Instructions
    INST(cpu)->operations[0xC9] = (CPU_6502_Operation){0xC9, 2, CMP, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xC5] = (CPU_6502_Operation){0xC5, 3, CMP, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xD5] = (CPU_6502_Operation){0xD5, 4, CMP, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xCD] = (CPU_6502_Operation){0xCD, 4, CMP, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xDD] = (CPU_6502_Operation){0xDD, 4, CMP, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0xD9] = (CPU_6502_Operation){0xD9, 4, CMP, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0xC1] = (CPU_6502_Operation){0xC1, 6, CMP, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0xD1] = (CPU_6502_Operation){0xD1, 5, CMP, INST(cpu)->addressModes.indirectYAddr};

    // CPX Instructions
    INST(cpu)->operations[0xE0] = (CPU_6502_Operation){0xE0, 2, CPX, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xE4] = (CPU_6502_Operation){0xE4, 3, CPX, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xEC] = (CPU_6502_Operation){0xEC, 4, CPX, INST(cpu)->addressModes.absoluteAddr};

    // CPY Instructions
    INST(cpu)->operations[0xC0] = (CPU_6502_Operation){0xC0, 2, CPY, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xC4] = (CPU_6502_Operation){0xC4, 3, CPY, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xCC] = (CPU_6502_Operation){0xCC, 4, CPY, INST(cpu)->addressModes.absoluteAddr};

    // INC Instructions
    INST(cpu)->operations[0xE6] = (CPU_6502_Operation){0xE6, 5, INC, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xF6] = (CPU_6502_Operation){0xF6, 6, INC, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xEE] = (CPU_6502_Operation){0xEE, 6, INC, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xFE] = (CPU_6502_Operation){0xFE, 7, INC, INST(cpu)->addressModes.absoluteXAddr};

    // INX Instructions
    INST(cpu)->operations[0xE8] = (CPU_6502_Operation){0xE8, 2, INX};

    // INY Instructions
    INST(cpu)->operations[0xC8] = (CPU_6502_Operation){0xC8, 2, INY};

    // DEC Instructions
    INST(cpu)->operations[0xC6] = (CPU_6502_Operation){0xC6, 5, DEC, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xD6] = (CPU_6502_Operation){0xD6, 6, DEC, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xCE] = (CPU_6502_Operation){0xCE, 6, DEC, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xDE] = (CPU_6502_Operation){0xDE, 7, DEC, INST(cpu)->addressModes.absoluteXAddr};

    // DEX Instructions
    INST(cpu)->operations[0xCA] = (CPU_6502_Operation){0xCA, 2, DEX};
    
    // DEY Instructions
    INST(cpu)->operations[0x88] = (CPU_6502_Operation){0x88, 2, DEY};

    // ADC Instructions
    INST(cpu)->operations[0x69] = (CPU_6502_Operation){0x69, 2, ADC, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0x65] = (CPU_6502_Operation){0x65, 3, ADC, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0x75] = (CPU_6502_Operation){0x75, 4, ADC, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0x6D] = (CPU_6502_Operation){0x6D, 4, ADC, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x7D] = (CPU_6502_Operation){0x7D, 4, ADC, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0x79] = (CPU_6502_Operation){0x79, 4, ADC, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0x61] = (CPU_6502_Operation){0x61, 6, ADC, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0x71] = (CPU_6502_Operation){0x71, 5, ADC, INST(cpu)->addressModes.indirectYAddr};

    // SBC Instructions
    INST(cpu)->operations[0xE9] = (CPU_6502_Operation){0xE9, 2, SBC, INST(cpu)->addressModes.immediate};
    INST(cpu)->operations[0xE5] = (CPU_6502_Operation){0xE5, 3, SBC, INST(cpu)->addressModes.zeroPageAddr};
    INST(cpu)->operations[0xF5] = (CPU_6502_Operation){0xF5, 4, SBC, INST(cpu)->addressModes.zeroPageXAddr};
    INST(cpu)->operations[0xED] = (CPU_6502_Operation){0xED, 4, SBC, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0xFD] = (CPU_6502_Operation){0xFD, 4, SBC, INST(cpu)->addressModes.absoluteXAddr};
    INST(cpu)->operations[0xF9] = (CPU_6502_Operation){0xF9, 4, SBC, INST(cpu)->addressModes.absoluteYAddr};
    INST(cpu)->operations[0xE1] = (CPU_6502_Operation){0xE1, 6, SBC, INST(cpu)->addressModes.indirectXAddr};
    INST(cpu)->operations[0xF1] = (CPU_6502_Operation){0xF1, 5, SBC, INST(cpu)->addressModes.indirectYAddr};

    // Transfer Instructions
    INST(cpu)->operations[0xAA] = (CPU_6502_Operation){0xAA, 2, TAX};
    INST(cpu)->operations[0xA8] = (CPU_6502_Operation){0xA8, 2, TAY};
    INST(cpu)->operations[0x8A] = (CPU_6502_Operation){0x8A, 2, TXA};
    INST(cpu)->operations[0x98] = (CPU_6502_Operation){0x98, 2, TYA};

    // JMP Instructions
    INST(cpu)->operations[0x4C] = (CPU_6502_Operation){0x4C, 3, JMP, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x6C] = (CPU_6502_Operation){0x6C, 5, JMP, INST(cpu)->addressModes.indirectAddr}; 
    INST(cpu)->operations[0x20] = (CPU_6502_Operation){0x20, 6, JSR, INST(cpu)->addressModes.absoluteAddr};
    INST(cpu)->operations[0x60] = (CPU_6502_Operation){0x60, 6, RTS};

    // Branch Instructions
    INST(cpu)->operations[0x90] = (CPU_6502_Operation){0x90, 2, BCC, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0xB0] = (CPU_6502_Operation){0xB0, 2, BCS, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0xF0] = (CPU_6502_Operation){0xF0, 2, BEQ, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0x30] = (CPU_6502_Operation){0x30, 2, BMI, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0xD0] = (CPU_6502_Operation){0xD0, 2, BNE, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0x10] = (CPU_6502_Operation){0x10, 2, BPL, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0x50] = (CPU_6502_Operation){0x50, 2, BVC, INST(cpu)->addressModes.relativeAddr};
    INST(cpu)->operations[0x70] = (CPU_6502_Operation){0x70, 2, BVS, INST(cpu)->addressModes.relativeAddr};

    // Status Flag Instructions
    INST(cpu)->operations[0x18] = (CPU_6502_Operation){0x18, 2, CLC};
    INST(cpu)->operations[0xD8] = (CPU_6502_Operation){0xD8, 2, CLD};
    INST(cpu)->operations[0x58] = (CPU_6502_Operation){0x58, 2, CLI};
    INST(cpu)->operations[0xB8] = (CPU_6502_Operation){0xB8, 2, CLV};
    INST(cpu)->operations[0x38] = (CPU_6502_Operation){0x38, 2, SEC};
    INST(cpu)->operations[0xF8] = (CPU_6502_Operation){0xF8, 2, SED};
    INST(cpu)->operations[0x78] = (CPU_6502_Operation){0x78, 2, SEI};

    // Stack Instructions
    INST(cpu)->operations[0xBA] = (CPU_6502_Operation){0xBA, 2, TSX};
    INST(cpu)->operations[0x9A] = (CPU_6502_Operation){0x9A, 2, TXS};
    INST(cpu)->operations[0x48] = (CPU_6502_Operation){0x48, 3, PHA};
    INST(cpu)->operations[0x08] = (CPU_6502_Operation){0x08, 3, PHP};
    INST(cpu)->operations[0x68] = (CPU_6502_Operation){0x68, 4, PLA};
    INST(cpu)->operations[0x28] = (CPU_6502_Operation){0x28, 4, PLP};

    // Break/RTI
    INST(cpu)->operations[0x00] = (CPU_6502_Operation){0x00, 7, BRK};
    INST(cpu)->operations[0x40] = (CPU_6502_Operation){0x40, 6, RTI};

    // Misc.
    INST(cpu)->operations[0xEA] = (CPU_6502_Operation){0xEA, 2, NOP};
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
static uint8_t fetchFromMemory(CPU_6502 * cpu, CPU_6502_Operation * op)
{
    // Perform addressing operation and return value in memory
    if (op->addrMode != INST(cpu)->addressModes.immediate)
        return MEM(cpu)[op->addrMode(cpu)];
    else
        return op->addrMode(cpu);
}

static void setStatusFlag(CPU_6502 * cpu, uint8_t flag, uint8_t val)
{
    if (val)
        cpu->status |= (1 << flag);
    else
        cpu-> status &= ~(1 << flag);
}

static bool getStatusFlag(CPU_6502 * cpu, uint8_t flag)
{
    if (cpu->status & (1 << flag))
        return true;
    
    return false;
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
static uint16_t accumulatorAddr(void * cpu)
{
    return INST(cpu)->registers.A;
}

// Default Operations
static void LDA(void * cpu, void * op)
{
    // Fetch Value and store into accumulator
    INST(cpu)->registers.A = fetchFromMemory(INST(cpu), OP(op));

    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.A & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.A == 0));
}
static void LDX(void * cpu, void * op)
{
    // Fetch Value and store into accumulator
    INST(cpu)->registers.X = fetchFromMemory(INST(cpu), OP(op));

    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.X & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.X == 0));
}
static void LDY(void * cpu, void * op)
{
    // Fetch Value and store into Y
    INST(cpu)->registers.Y = fetchFromMemory(INST(cpu), OP(op));

    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.Y & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.Y == 0));
}

static void STA(void * cpu, void * op)
{
    // Fetch Zero Page address
    uint16_t addr = OP(op)->addrMode(INST(cpu));

    // Store Accumulator to index
    MEM(INST(cpu))[addr] = INST(cpu)->registers.A;
    
    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.A & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.A == 0));
}
static void STX(void * cpu, void * op)
{
    // Fetch Zero Page address
    uint16_t addr = OP(op)->addrMode(INST(cpu));

    // Store X to index
    MEM(INST(cpu))[addr] = INST(cpu)->registers.X;
    
    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.X & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.X == 0));
}
static void STY(void * cpu, void * op)
{
    // Fetch Zero Page address
    uint16_t addr = OP(op)->addrMode(INST(cpu));

    // Store Y to index
    MEM(INST(cpu))[addr] = INST(cpu)->registers.Y;
    
    // Set Zero & Negative Flags appropriately
    setStatusFlag(INST(cpu), CPU_6502_STATUS_NEGATIVE, (INST(cpu)->registers.Y & 0b10000000));
    setStatusFlag(INST(cpu), CPU_6502_STATUS_ZERO, (INST(cpu)->registers.Y == 0));
}

static void AND(void * cpu, void * op)
{

}
static void ORA(void * cpu, void * op)
{

}
static void EOR(void * cpu, void * op)
{

}
static void BIT(void * cpu, void * op)
{

}

static void ROL(void * cpu, void * op)
{

}
static void ROR(void * cpu, void * op)
{

}
static void ASL(void * cpu, void * op)
{

}
static void LSR(void * cpu, void * op)
{

}

static void CMP(void * cpu, void * op)
{

}
static void CPX(void * cpu, void * op)
{

}
static void CPY(void * cpu, void * op)
{

}

static void INC(void * cpu, void * op)
{

}
static void INX(void * cpu, void * op)
{

}
static void INY(void * cpu, void * op)
{

}
static void DEC(void * cpu, void * op)
{

}
static void DEX(void * cpu, void * op)
{

}
static void DEY(void * cpu, void * op)
{

}

static void ADC(void * cpu, void * op)
{

}
static void SBC(void * cpu, void * op)
{

}

static void TAX(void * cpu, void * op)
{

}
static void TAY(void * cpu, void * op)
{

}
static void TXA(void * cpu, void * op)
{

}
static void TYA(void * cpu, void * op)
{

}

static void JMP(void * cpu, void * op)
{

}
static void JSR(void * cpu, void * op)
{

}
static void RTS(void * cpu, void * op)
{

}

static void BCC(void * cpu, void * op)
{

}
static void BCS(void * cpu, void * op)
{

}
static void BEQ(void * cpu, void * op)
{

}
static void BMI(void * cpu, void * op)
{

}
static void BNE(void * cpu, void * op)
{

}
static void BPL(void * cpu, void * op)
{

}
static void BVC(void * cpu, void * op)
{

}
static void BVS(void * cpu, void * op)
{

}

static void CLC(void * cpu, void * op)
{

}
static void CLD(void * cpu, void * op)
{

}
static void CLI(void * cpu, void * op)
{

}
static void CLV(void * cpu, void * op)
{

}

static void SEC(void * cpu, void * op)
{

}
static void SED(void * cpu, void * op)
{

}
static void SEI(void * cpu, void * op)
{

}

static void TSX(void * cpu, void * op)
{

}
static void TXS(void * cpu, void * op)
{

}
static void PHA(void * cpu, void * op)
{

}
static void PHP(void * cpu, void * op)
{

}
static void PLA(void * cpu, void * op)
{

}
static void PLP(void * cpu, void * op)
{

}

static void BRK(void * cpu, void * op)
{

}
static void RTI(void * cpu, void * op)
{

}

static void NOP(void * cpu, void * op)
{

}