#include "instruction.hh"

Instruction::Instruction()
{}

Instruction::~Instruction()
{}

bool Instruction::operator == (const Instruction& other) {
    return (this->pc == other.pc && this->opcode == other.opcode);
}
