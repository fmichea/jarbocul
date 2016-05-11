#pragma once
#ifndef JARBOCUL_GRAPH_INSTRUCTION_HH_
# define JARBOCUL_GRAPH_INSTRUCTION_HH_

# include "../lib/ostream_writable.hh"
# include "../processors/cpu_traits.hh"

template <typename CPU>
class Instruction : public OStreamWritable {
public:
    Instruction();
    virtual ~Instruction();

    virtual typename cpu_traits<CPU>::AddrType pc() const;
    virtual void set_pc(typename cpu_traits<CPU>::AddrType value);

    bool operator == (const Instruction<CPU>& other);

protected:
    void _ostream_write(std::ostream& os) const;

protected:
    typename cpu_traits<CPU>::AddrType _pc;
};

# include "instruction.hxx"

#endif /* !JARBOCUL_GRAPH_INSTRUCTION_HH_ */
