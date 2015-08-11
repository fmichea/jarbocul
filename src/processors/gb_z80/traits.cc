#include "traits.hh"

namespace ft_np = jarbocul::lib::flowtype;

Instruction* GB_Z80::parse_line(const char* line) {
    if (strlen(line) != 41)
        return nullptr;

    GB_Z80_Instruction* inst = new GB_Z80_Instruction();

    char pc[5];
    memcpy(pc, line + 12, 4);
    pc[4] = 0;
    inst->set_pc(strtol(pc, NULL, 16));

    char opcode[3];
    memcpy(opcode, line + 27, 2);
    opcode[2] = 0;
    inst->opcode = strtol(opcode, NULL, 16);

    char data[3];
    memcpy(data, line + 37, 2);
    data[2] = 0;
    inst->data[0] = strtol(data, NULL, 16);
    memcpy(data, line + 39, 2);
    inst->data[1] = strtol(data, NULL, 16);

    return inst;
}

ft_np::FT flowtype(Block* last_block, uint16_t offset) {
    ft_np::flowtype_opcode_type opcode_type = ft_np::FLOWTYPE_OPCODE_NONE;
    uint16_t opcode_size = 0;

    switch (last_block->op()->opcode) {
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
    ret.set_taken(offset != opcode_size);
    return ret;
}

bool is_interrupt(Instruction* op) {
    for (uint16_t addr = 0x0000; addr <= 0x0060; addr += 0x8) {
        if (op->pc() == addr)
            return true;
    }
    return false;
}
