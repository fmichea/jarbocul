#ifndef JARBOCUL_LIB_SET_SORTER_HH_
# define JARBOCUL_LIB_SET_SORTER_HH_

# include <algorithm>
# include <set>
# include <vector>

template <typename T>
std::vector<T> set_sorter(std::set<T> in, bool (*cmp_func)(T a, T b));

# include "lib/set_sorter.hxx"

#endif /* !JARBOCUL_LIB_SET_SORTER_HH_ */
