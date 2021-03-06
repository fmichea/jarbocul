#pragma once
#ifndef JARBOCUL_PROCESSORS_CPU_TRAITS_HXX_
# define JARBOCUL_PROCESSORS_CPU_TRAITS_HXX_

# include "processors/cpu_traits.hh"

template <typename CPU>
std::string addr2str(typename cpu_traits<CPU>::AddrType pc) {
    std::ostringstream addr;

    size_t addr_width = sizeof (typename cpu_traits<CPU>::AddrType) * 2;

    addr << std::setfill('0') << std::setw(addr_width) << std::hex
         << std::uppercase << pc;
    return addr.str();
}

#endif /* !JARBOCUL_PROCESSORS_CPU_TRAITS_HXX_ */
