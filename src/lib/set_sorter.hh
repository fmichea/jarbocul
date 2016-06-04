#ifndef JARBOCUL_LIB_SET_SORTER_HH_
# define JARBOCUL_LIB_SET_SORTER_HH_

# include <algorithm>
# include <vector>
# include <set>

template <typename T>
std::vector<T> set_sorter(std::set<T> in, bool (*cmp_func)(T a, T b));

# include "set_sorter.hxx"

#endif /* !JARBOCUL_LIB_SET_SORTER_HH_ */
