#ifndef TRAITS_HH_
# define TRAITS_HH_

# include "../../lib/flowtype.hh"

using ft_np = jarbocul::lib::flowtype;

class GB_Z80;

template <>
struct cpu_traits<GB_Z80> {
    typedef uint16_t Addr;
    typedef uint64_t BlockId;
};


class GB_Z80_Instruction : public Instruction<GB_Z80> {
public:
    GB_Z80_Instruction();

private:
    uint8_t opcode; // operation code
    uint8_t data[2];  // data after opcode
};

template<>
Instruction<GB_Z80>* parse_line<GB_Z80>(const char* line);

template <>
cpu_traits<GB_Z80>::BlockId block_id(Block<GB_Z80>* block);

template <>
const ft_np::FT flowtype<GB_Z80>(Block<CPU>* last_block, AddrOffset<CPU>& offset);

# include "traits.hxx"

#endif /* !TRAITS_HH_ */
