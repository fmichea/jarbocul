#pragma once
#ifndef JARBOCUL_LIB_FLOWTYPE_HH_
# define JARBOCUL_LIB_FLOWTYPE_HH_

namespace jarbocul { namespace lib { namespace flowtype {
    typedef enum flowtype_opcode_type_e {
        FLOWTYPE_OPCODE_NONE,
        FLOWTYPE_OPCODE_RET,
        FLOWTYPE_OPCODE_CALL,
        FLOWTYPE_OPCODE_JUMP,
    } flowtype_opcode_type;

    class FT {
    public:
        FT(flowtype_opcode_type ft_op_type=FLOWTYPE_OPCODE_NONE);

        flowtype_opcode_type opcode_type() const;
        bool taken() const;

        void set_opcode_type(flowtype_opcode_type ft_op_type);
        void set_taken(bool taken);

    private:
        flowtype_opcode_type _ft_op_type;
        bool _taken;
    };
}}}

#endif /* !JARBOCUL_LIB_FLOWTYPE_HH_ */
