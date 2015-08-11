#pragma once
#ifndef INSTRUCTION_HH_
# define INSTRUCTION_HH_

# include <string>

template <typename CPU>
class Instruction {
public:
    Instruction();
    virtual ~Instruction();

    virtual cpu_traits<CPU>::Addr pc();
    virtual void set_pc(cpu_traits<CPU>::Addr value);

    virtual bool operator == (const Instruction<CPU>& other) = 0;

public:
    cpu_traits<CPU>::Addr _pc;
};

# include "intruction.hxx"

#endif /* !INSTRUCTION_HH_ */
