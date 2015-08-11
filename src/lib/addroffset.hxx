#ifndef ADDROFFSET_HXX_
# define ADDROFFSET_HXX_

# include "addroffset.hh"

template <typename CPU>
AddrOffset<CPU>::AddrOffset<CPU>(cpu_traits<CPU>::Addr addr1, cpu_traits<CPU>::Addr addr2) {
    this->_negative = (addr2 < addr1);
    this->_offset = (!this->_negative) ? (addr2 - addr1) : (addr1 - addr2);
}

#endif /* !ADDROFFSET_HXX_ */
