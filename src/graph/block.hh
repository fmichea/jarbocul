#pragma once
#ifndef JARBOCUL_GRAPH_BLOCK_HH_
# define JARBOCUL_GRAPH_BLOCK_HH_

# include <iomanip>
# include <iostream>
# include <list>
# include <sstream>
# include <string>
# include <vector>

# include <assert.h>

# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_generators.hpp>

# include "lib/ostream_writable.hh"
# include "graph/instruction.hh"

typedef enum {
    BLOCKTYPE_INT   = 0x01,
    BLOCKTYPE_LOC   = 0x02,
    BLOCKTYPE_SUB   = 0x03,
} blocktype;

std::string blocktype2str(blocktype t);

typedef boost::uuids::uuid BlockId;

template <typename CPU>
class Block : public OStreamWritable {
public:
    Block(Instruction<CPU>* inst);
    virtual ~Block();

    std::string uniq_name() const;
    virtual std::string name() const;

    BlockId id() const;
    blocktype block_type() const;

    uint32_t uniq_id() const;
    bool mergeable() const;
    std::list<std::string> parents() const;

    void merge(Block<CPU>* other);

    Instruction<CPU>* op();
    Instruction<CPU>* op(int idx);

    typename cpu_traits<CPU>::AddrType pc() const;

    void set_uniq_id(uint32_t uniq_id);
    void set_block_type(blocktype block_type);
    void set_mergeable(bool mergeable);
    void add_parent(std::string block);
    void set_sep(std::string sep);

    void _ostream_write(std::ostream& os) const;

protected:
    std::string _sep;

    std::vector<Instruction<CPU>*> _insts;

private:
    BlockId _id;

    blocktype _block_type;

    bool _uniq;
    uint32_t _uniq_id;

    std::list<std::string> _parents;

    bool _mergeable;
};

template <typename CPU>
class SpecialInstruction : public Instruction<CPU> {
public:
    SpecialInstruction(Block<CPU>* block, std::string label)
        : Instruction<CPU>()
        , _block (block)
        , _label (label)
    {}

    void _ostream_write(std::ostream& os) const;

private:
    Block<CPU>* _block;
    std::string _label;
};

template <typename CPU>
class SpecialLabelBlock : public Block<CPU> {
public:
    SpecialLabelBlock(std::string label);

    void _ostream_write(std::ostream& os) const;
    std::string name() const;
};

# include "graph/block.hxx"

#endif /* !JARBOCUL_GRAPH_BLOCK_HH_ */
