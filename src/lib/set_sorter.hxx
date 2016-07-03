#ifndef JARBOCUL_LIB_SET_SORTER_HXX_
# define JARBOCUL_LIB_SET_SORTER_HXX_

# include "lib/set_sorter.hh"

template <typename T>
std::vector<T> set_sorter(std::set<T> in, bool (*cmp_func)(T a, T b)) {
    std::vector<T> res(in.size());

    std::copy(in.begin(), in.end(), res.begin());
    std::sort(res.begin(), res.end(), cmp_func);
    return res;
}

#endif /* !JARBOCUL_LIB_SET_SORTER_HXX_ */
