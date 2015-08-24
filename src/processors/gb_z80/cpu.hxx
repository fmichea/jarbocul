#ifndef JARBOCUL_PROCESSORS_GBZ80_CPU_HXX_
# define JARBOCUL_PROCESSORS_GBZ80_CPU_HXX_

#include "cpu.hh"

namespace ft_np = jarbocul::lib::flowtype;

template<>
struct cpu_functions<GB_Z80> {
    static Instruction<GB_Z80>* parse_line(const char* line) {
        if (strlen(line) != 41)
            return nullptr;

        char _number_str[5];
        GB_Z80_Instruction* inst = new GB_Z80_Instruction();

        memcpy(_number_str, line + 12, 4); _number_str[4] = 0;
        inst->set_pc(strtol(_number_str, NULL, 16));

        memcpy(_number_str, line + 27, 2); _number_str[2] = 0;
        inst->set_opcode(strtol(_number_str, NULL, 16));

        uint8_t data[2];
        memcpy(_number_str, line + 37, 2); _number_str[2] = 0;
        data[0] = strtol(_number_str, NULL, 16);
        memcpy(_number_str, line + 39, 2);
        data[1] = strtol(_number_str, NULL, 16);
        inst->set_data(data);

        return inst;
    }

    static ft_np::FT flowtype(Block<GB_Z80>* last_block, AddrOffset<GB_Z80>& offset) {
        ft_np::flowtype_opcode_type opcode_type = ft_np::FLOWTYPE_OPCODE_NONE;
        uint16_t opcode_size = 0;

        Instruction<GB_Z80>* _inst = last_block->op();
        GB_Z80_Instruction* inst = static_cast<GB_Z80_Instruction*>(_inst);

        switch (inst->opcode()) {
        case 0xC0: case 0xC8: case 0xC9:
        case 0xD0: case 0xD8: case 0xD9:
            opcode_type = ft_np::FLOWTYPE_OPCODE_RET;
            opcode_size = 1;
            break;

        case 0xC4: case 0xCC: case 0xCD:
        case 0xD4: case 0xDC:
            opcode_type = ft_np::FLOWTYPE_OPCODE_CALL;
            opcode_size = 3;
            break;

        case 0xC2: case 0xC3: case 0xCA:
        case 0xD2: case 0xDA:
        case 0xE9:
            opcode_type = ft_np::FLOWTYPE_OPCODE_JUMP;
            opcode_size = 3;
            break;

        case 0x18:
        case 0x20: case 0x28:
        case 0x30: case 0x38:
            opcode_type = ft_np::FLOWTYPE_OPCODE_JUMP; // Relative jump.
            opcode_size = 2;
            break;
        };

        ft_np::FT ret(opcode_type);
        ret.set_taken(offset.offset() != opcode_size);
        return ret;
    }

    static bool is_interrupt(Instruction<GB_Z80>* op) {
        for (uint16_t addr = 0x0000; addr <= 0x0060; addr += 0x8) {
            if (op->pc() == addr)
                return true;
        }
        return false;
    }

    static bool is_interrupt_call(Instruction<GB_Z80>* _inst) {
        GB_Z80_Instruction* inst = static_cast<GB_Z80_Instruction*>(_inst);

        for (uint16_t tmp = 0xC7; tmp < 0x100; tmp += 0x8) {
            if (tmp == inst->opcode()) {
                return true;
            }
        }
        return false;
    }

    static bool is_flow_instruction(Instruction<GB_Z80>* _inst) {
        GB_Z80_Instruction* inst = static_cast<GB_Z80_Instruction*>(_inst);

        switch (inst->opcode()) {
        // ret
        case 0xC0: case 0xC8: case 0xC9:
        case 0xD0: case 0xD8: case 0xD9:
        // call
        case 0xC4: case 0xCC: case 0xCD:
        case 0xD4: case 0xDC:
        // jump
        case 0xC2: case 0xC3: case 0xCA:
        case 0xD2: case 0xDA:
        case 0xE9:
        // jr
        case 0x18:
        case 0x20: case 0x28:
        case 0x30: case 0x38:
            return true;

        default:
            return false;
        };
    }
};

#endif /* !JARBOCUL_PROCESSORS_GBZ80_CPU_HXX_ */
