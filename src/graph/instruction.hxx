#pragma once
#ifndef JARBOCUL_GRAPH_INSTRUCTION_HXX_
# define JARBOCUL_GRAPH_INSTRUCTION_HXX_

# include "instruction.hh"

template <typename CPU>
Instruction<CPU>::Instruction()
{}

template <typename CPU>
Instruction<CPU>::~Instruction()
{}

template <typename CPU>
typename cpu_traits<CPU>::AddrType Instruction<CPU>::pc() const {
    return this->_pc;
}

template <typename CPU>
void Instruction<CPU>::set_pc(typename cpu_traits<CPU>::AddrType pc) {
    this->_pc = pc;
}

template <typename CPU>
bool Instruction<CPU>::operator == (const Instruction<CPU>& other) {
    return this->pc() == other.pc();
}

template <typename CPU>
void Instruction<CPU>::_ostream_write(std::ostream& os) const {
    os << "    " << addr2str<CPU>(this->pc()) << " - [abstract instruction]";
}

#endif /* !JARBOCUL_GRAPH_INSTRUCTION_HXX_ */
