#pragma once
#ifndef JARBOCUL_PROCESSORS_CPU_FUNCTIONS_HH_
# define JARBOCUL_PROCESSORS_CPU_FUNCTIONS_HH_

# include <assert.h>

# include "graph/block.hh"
# include "graph/instruction.hh"
# include "lib/addroffset.hh"
# include "processors/cpu_traits.hh"

template <typename CPU>
struct cpu_functions {
    static Instruction<CPU>* parse_line(const char* line);

    static bool is_flow_instruction(Instruction<CPU>* inst);

    static bool is_interrupt(Instruction<CPU>* inst);

    static size_t interrupt_call_opcode_size(Instruction<CPU>* inst);

    static jarbocul::lib::flowtype::FT flowtype(Block<CPU>* last_block, AddrOffset<CPU>& offset);
};

#endif /* !JARBOCUL_PROCESSORS_CPU_FUNCTIONS_HH_ */
