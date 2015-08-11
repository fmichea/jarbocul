#ifndef INSTRUCTION_HXX_
# define INSTRUCTION_HXX_

template <typename CPU>
cpu_traits<CPU>::Addr Instruction::pc() {
    return this->_pc;
}

template <typename CPU>
void Instruction::set_pc(cpu_traits<CPU>::Addr pc) {
    this->_pc = pc;
}

#endif /* !INSTRUCTION_HXX_ */
