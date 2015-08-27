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
std::ostream& operator<<(std::ostream& os, const Instruction<CPU>& inst) {
    os << addr2str<CPU>(inst.pc()) << " - [abstract instruction]";
    return os;
}

#endif /* !JARBOCUL_GRAPH_INSTRUCTION_HXX_ */
