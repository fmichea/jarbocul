#ifndef ADDROFFSET_HH_
# define ADDROFFSET_HH_

template <typename CPU>
class AddrOffset {
public:
    AddrOffset(cpu_traits<CPU>::Addr addr1, cpu_traits<CPU>::Addr addr2);

    bool negative();
    cpu_traits<CPU>::Addr offset();

private:
    bool _negative;
    cpu_traits<CPU>::Addr _offset;
};

# include "addroffset.hxx"

#endif /* !ADDROFFSET_HH_ */
