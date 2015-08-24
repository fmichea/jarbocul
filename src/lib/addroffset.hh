#pragma once
#ifndef JARBOCUL_LIB_ADDROFFSET_HH_
# define JARBOCUL_LIB_ADDROFFSET_HH_

# include "../processors/cpu_traits.hh"

template <typename CPU>
class AddrOffset {
public:
    AddrOffset(typename cpu_traits<CPU>::AddrType addr1,
               typename cpu_traits<CPU>::AddrType addr2);

    bool negative() const;
    typename cpu_traits<CPU>::AddrType offset() const;

private:
    typename cpu_traits<CPU>::AddrType _offset;
    bool _negative;
};

# include "addroffset.hxx"

#endif /* !JARBOCUL_LIB_ADDROFFSET_HH_ */
