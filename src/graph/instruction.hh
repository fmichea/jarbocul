#pragma once
#ifndef INSTRUCTION_HH_
# define INSTRUCTION_HH_

# include <string>

class Instruction {
public:
    Instruction();
    virtual ~Instruction();

    bool operator == (const Instruction& other);

public:
    uint16_t pc;
    uint8_t opcode; // operation code
    uint8_t data[2];  // data after opcode
};

#endif /* !INSTRUCTION_HH_ */
