#include "cpu.hh"

GB_Z80_Instruction::GB_Z80_Instruction()
    : Instruction<GB_Z80>()
{}

uint8_t GB_Z80_Instruction::opcode() const {
    return this->_opcode;
}

void GB_Z80_Instruction::data(uint8_t out[2]) const {
    memcpy(out, this->_data, 2);
}

void GB_Z80_Instruction::set_opcode(uint8_t opcode) {
    this->_opcode  = opcode;
}

void GB_Z80_Instruction::set_data(uint8_t data[2]) {
    memcpy(this->_data, data, 2);
}
