#pragma once
#ifndef CPU_TRAITS_HXX_
# define CPU_TRAITS_HXX_

# include "cpu_traits.hh"

template <typename CPU>
std::string addr2str(typename cpu_traits<CPU>::AddrType pc) {
    std::ostringstream addr;

    uint16_t addr_width = sizeof (typename cpu_traits<CPU>::AddrType) * 2;

    addr << std::setfill('0') << std::setw(addr_width) << std::hex
         << std::uppercase << pc;
    return addr.str();
}

#endif /* !CPU_TRAITS_HXX_ */
