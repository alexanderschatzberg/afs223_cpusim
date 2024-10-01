#include "sol.h"
#include <stdio.h>
#include <stdint.h>

uint32_t fetch(uint64_t PC, uint32_t* instructions) {
    return instructions[PC / 4];
}

info decode(uint32_t instruction) {
    info result;
    uint32_t opcode = instruction & 0x7F;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x7;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t rs2 = (instruction >> 20) & 0x1F;
    uint32_t funct7 = (instruction >> 25) & 0x7F;

    int32_t imm_I = (int32_t)(instruction) >> 20; // Sign-extended immediate for I-type
    int32_t imm_S = ((instruction >> 7) & 0x1F) | (((int32_t)(instruction) >> 25) << 5); // Sign-extended immediate for S-type
    int32_t imm_B = 0;
    imm_B |= ((instruction >> 31) & 0x1) << 12; // imm[12]
    imm_B |= ((instruction >> 25) & 0x3F) << 5; // imm[10:5]
    imm_B |= ((instruction >> 8) & 0xF) << 1;   // imm[4:1]
    imm_B |= ((instruction >> 7) & 0x1) << 11;  // imm[11]
    if (imm_B & (1 << 12)) {
        imm_B |= 0xFFFFE000; // Sign-extend to 32 bits
    }

    int32_t imm_U = (int32_t)(instruction & 0xFFFFF000); // For LUI

    result.inst = 0;
    result.first = 0;
    result.second = 0;
    result.third = 0;

    switch (opcode) {
    case 0x33: // R-type instructions
        switch (funct3) {
        case 0x0:
            if (funct7 == 0x00) {
                result.inst = ADD;
            }
            else if (funct7 == 0x20) {
                result.inst = SUB;
            }
            break;
        case 0x7:
            if (funct7 == 0x00) {
                result.inst = AND;
            }
            break;
        case 0x2:
            if (funct7 == 0x00) {
                result.inst = SLT;
            }
            break;
        case 0x1:
            if (funct7 == 0x00) {
                result.inst = SLL;
            }
            break;
        case 0x5:
            if (funct7 == 0x20) {
                result.inst = SRA;
            }
            break;
        default:
            break;
        }
        result.first = registers[rs1];
        result.second = registers[rs2];
        result.third = rd;
        break;
    case 0x13: // I-type instructions
        switch (funct3) {
        case 0x0:
            result.inst = ADDI;
            break;
        case 0x7:
            result.inst = ANDI;
            break;
        default:
            break;
        }
        result.first = registers[rs1];
        result.second = imm_I;
        result.third = rd;
        break;
    case 0x03: // Load instructions
        switch (funct3) {
        case 0x3:
            result.inst = LD;
            break;
        case 0x2:
            result.inst = LW;
            break;
        case 0x0:
            result.inst = LB;
            break;
        default:
            break;
        }
        result.first = registers[rs1];
        result.second = imm_I;
        result.third = rd;
        break;
    case 0x23: // S-type instructions (Store)
        switch (funct3) {
        case 0x3:
            result.inst = SD;
            break;
        case 0x2:
            result.inst = SW;
            break;
        case 0x0:
            result.inst = SB;
            break;
        default:
            break;
        }
        result.first = registers[rs1];
        result.second = registers[rs2];
        result.third = imm_S;
        break;
    case 0x37: // LUI
        result.inst = LUI;
        result.first = imm_U;
        result.second = rd;
        break;
    case 0x63: // B-type instructions
        switch (funct3) {
        case 0x0:
            result.inst = BEQ;
            break;
        default:
            break;
        }
        result.first = registers[rs1];
        result.second = registers[rs2];
        result.third = imm_B;
        break;
    default:
        break;
    }

    return result;
}

info execute(info information) {
    info result = information;

    switch (result.inst) {
    case ADD:
        result.first = result.first + result.second;
        break;
    case SUB:
        result.first = result.first - result.second;
        break;
    case AND:
        result.first = result.first & result.second;
        break;
    case SLT:
        result.first = ((int64_t)result.first < (int64_t)result.second) ? 1 : 0;
        break;
    case SLL:
        result.first = result.first << (result.second & 0x3F);
        break;
    case SRA:
        result.first = ((int64_t)result.first) >> (result.second & 0x3F);
        break;
    case ADDI:
        result.first = result.first + (int64_t)(int32_t)result.second;
        break;
    case ANDI:
        result.first = result.first & result.second;
        break;
    case LUI:
        // result.first already has the immediate value
        break;
    case LD:
    case LW:
    case LB:
        // Compute effective address for load
        result.first = result.first + (int64_t)(int32_t)result.second;
        break;
    case SD:
    case SW:
    case SB:
        // Compute effective address for store
        result.first = result.first + (int64_t)(int32_t)result.third;
        break;
    case BEQ:
        result.first = (result.first == result.second) ? 1 : 0;
        break;
    default:
        break;
    }

    return result;
}

info memory(info information) {
    info result = information;

    switch (result.inst) {
    case LD:
    {
        uint64_t data_value = 0;
        for (int i = 0; i < 8; ++i) {
            uint8_t byte = ht_get(data, result.first + i);
            data_value |= ((uint64_t)byte) << (8 * i);
        }
        result.second = data_value;
        break;
    }
    case LW:
    {
        uint32_t data_value = 0;
        for (int i = 0; i < 4; ++i) {
            uint8_t byte = ht_get(data, result.first + i);
            data_value |= ((uint32_t)byte) << (8 * i);
        }
        result.second = (int64_t)(int32_t)data_value; // Sign-extend
        break;
    }
    case LB:
    {
        uint8_t byte = ht_get(data, result.first);
        result.second = (int64_t)(int8_t)byte; // Sign-extend
        break;
    }
    case SD:
    {
        uint64_t data_value = result.second;
        for (int i = 0; i < 8; ++i) {
            uint8_t byte = (data_value >> (8 * i)) & 0xFF;
            ht_insert(data, result.first + i, byte);
        }
        break;
    }
    case SW:
    {
        uint32_t data_value = (uint32_t)result.second;
        for (int i = 0; i < 4; ++i) {
            uint8_t byte = (data_value >> (8 * i)) & 0xFF;
            ht_insert(data, result.first + i, byte);
        }
        break;
    }
    case SB:
    {
        uint8_t byte = result.second & 0xFF;
        ht_insert(data, result.first, byte);
        break;
    }
    default:
        break;
    }

    return result;
}

uint64_t writeback(uint64_t PC, info information) {
    switch (information.inst) {
    case ADD:
    case SUB:
    case AND:
    case SLT:
    case SLL:
    case SRA:
    case ADDI:
    case ANDI:
        if (information.third != 0) {
            registers[information.third] = information.first;
        }
        break;
    case LUI:
        if (information.second != 0) {
            registers[information.second] = information.first;
        }
        break;
    case LD:
    case LW:
    case LB:
        if (information.third != 0) {
            registers[information.third] = information.second;
        }
        break;
    case BEQ:
        if (information.first) {
            PC = PC + (int64_t)(int32_t)information.third;
            return PC;
        }
        break;
    default:
        break;
    }

    return PC + 4;
}
