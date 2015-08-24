#pragma once
#ifndef JARBOCUL_LIB_ADDROFFSET_HXX_
# define JARBOCUL_LIB_ADDROFFSET_HXX_

# include "addroffset.hh"

template <typename CPU>
AddrOffset<CPU>::AddrOffset(typename cpu_traits<CPU>::AddrType addr1,
                            typename cpu_traits<CPU>::AddrType addr2)
{
    this->_negative = (addr2 < addr1);
    this->_offset = (!this->_negative) ? (addr2 - addr1) : (addr1 - addr2);
}

template <typename CPU>
bool AddrOffset<CPU>::negative() const {
    return this->_negative;
}

template <typename CPU>
typename cpu_traits<CPU>::AddrType AddrOffset<CPU>::offset() const {
    return this->_offset;
}

#endif /* !JARBOCUL_LIB_ADDROFFSET_HXX_ */
