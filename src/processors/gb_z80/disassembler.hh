#pragma once
#ifndef JARBOCUL_PROCESSORS_GBZ80_DISASSEMBLER_HH_
# define JARBOCUL_PROCESSORS_GBZ80_DISASSEMBLER_HH_

# include <unistd.h>
# include <stdio.h>
# include <stdint.h>

# include "cpu.hh"

const char* disassemble(GB_Z80_Instruction* inst);

#endif /* !JARBOCUL_PROCESSORS_GBZ80_DISASSEMBLER_HH_ */
