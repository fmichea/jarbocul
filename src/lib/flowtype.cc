#include "flowtype.hh"

namespace ft_np = jarbocul::lib::flowtype;

namespace jarbocul { namespace lib { namespace flowtype {
    FT::FT(ft_np::flowtype_opcode_type ft_op_type)
        : _ft_op_type (ft_op_type), _taken (false)
    {}

    ft_np::flowtype_opcode_type FT::opcode_type() const {
        return this->_ft_op_type;
    }

    bool FT::taken() const {
        return this->_taken;
    }

    void FT::set_opcode_type(ft_np::flowtype_opcode_type ft_op_type) {
        this->_ft_op_type = ft_op_type;
    }

    void FT::set_taken(bool taken) {
        this->_taken = taken;
    }
}}}
