#pragma once
#ifndef JARBOCUL_PROCESSORS_CPU_TRAITS_HH_
# define JARBOCUL_PROCESSORS_CPU_TRAITS_HH_

# include <iomanip>
# include <sstream>
# include <string>

template <typename CPU>
struct cpu_traits {};

template <typename CPU>
std::string addr2str(typename cpu_traits<CPU>::AddrType pc);

# include "processors/cpu_traits.hxx"

#endif /* !JARBOCUL_PROCESSORS_CPU_TRAITS_HH_ */
