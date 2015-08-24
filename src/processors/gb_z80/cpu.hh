#pragma once
#ifndef JARBOCUL_PROCESSORS_GBZ80_CPU_HH_
# define JARBOCUL_PROCESSORS_GBZ80_CPU_HH_

# include <string.h>

# include "../../graph/instruction.hh"
# include "../../lib/flowtype.hh"
# include "../cpu_traits.hh"
# include "../cpu_functions.hh"

class GB_Z80;

template <>
struct cpu_traits<GB_Z80> {
    typedef uint16_t AddrType;
};

class GB_Z80_Instruction : public Instruction<GB_Z80> {
public:
    GB_Z80_Instruction();

    uint8_t opcode() const;
    void data(uint8_t out[2]) const;

    void set_opcode(uint8_t opcode);
    void set_data(uint8_t data[2]);

private:
    uint8_t _opcode;
    uint8_t _data[2];
};

# include "cpu.hxx"

#endif /* !JARBOCUL_PROCESSORS_GBZ80_CPU_HH_ */
