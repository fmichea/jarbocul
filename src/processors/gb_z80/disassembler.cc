# include "cpu.hh"

static const size_t BUF_SZ = 256;
static char buf[BUF_SZ];

# define DISASS_FUNC_RET(...)                                               \
    (void) inst;                                                            \
    snprintf(buf, BUF_SZ, ## __VA_ARGS__);                                  \
    return buf;

# define DISASS_FUNC(FuncName, ...)                                         \
    inline const char* FuncName(const GB_Z80_Instruction* inst, ## __VA_ARGS__)

inline uint16_t unpack_addr(uint8_t* data) {
    uint16_t ret = data[0];
    ret |= data[1] << 8;
    return ret;
}

DISASS_FUNC(inc_reg, const char* reg) {
    DISASS_FUNC_RET("inc %s", reg);
}

DISASS_FUNC(dec_reg, const char* reg) {
    DISASS_FUNC_RET("dec %s", reg);
}

DISASS_FUNC(push_reg, const char* reg) {
    DISASS_FUNC_RET("push %s", reg);
}

DISASS_FUNC(pop_reg, const char* reg) {
    DISASS_FUNC_RET("pop %s", reg);
}

DISASS_FUNC(ld_a_mreg, const char* reg) {
    DISASS_FUNC_RET("ld %%a, (%s)", reg);
}

DISASS_FUNC(ld_mreg_a, const char* reg) {
    DISASS_FUNC_RET("ld (%s), %%a", reg);
}

DISASS_FUNC(call_flag_a16, const char* flag) {
    uint8_t data[2];

    inst->data(data);
    DISASS_FUNC_RET("call %s, $0x%04X", flag, unpack_addr(data));
}

DISASS_FUNC(jmp_flag_a16, const char* flag) {
    uint8_t data[2];

    inst->data(data);
    DISASS_FUNC_RET("jmp %s, $0x%04X", flag, unpack_addr(data));
}

DISASS_FUNC(jr_flag_r8, const char* flag) {
    uint8_t data[2];
    uint8_t& d = data[0];

    inst->data(data);
    DISASS_FUNC_RET("jr %s, $0x%02X ; ($%i)", flag, d, (int8_t) d);
}

DISASS_FUNC(ret_flag, const char* flag) {
    DISASS_FUNC_RET("ret %s", flag);
}

DISASS_FUNC(ld_reg_reg, const char* reg1, const char* reg2) {
    DISASS_FUNC_RET("ld %s, %s", reg1, reg2);
}

DISASS_FUNC(op_a_reg, const char* op, const char* reg) {
    DISASS_FUNC_RET("%s %%a, %s", op, reg);
}

DISASS_FUNC(rst_nn, uint8_t nn) {
    DISASS_FUNC_RET("rst %02Xh", nn);
}

DISASS_FUNC(jmp_a16) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("jmp $0x%04X", unpack_addr(data));
}

DISASS_FUNC(ld_mc_a) {
    DISASS_FUNC_RET("ld ($0xFFF0 + %%c), %%a");
}

DISASS_FUNC(ld_a_mc) {
    DISASS_FUNC_RET("ld %%a, ($0xFFF0 + %%c)");
}

DISASS_FUNC(cb) {
    static char cb_ops_tmp[256];
    static const char* cb_regs[] = {"%b", "%c", "%d", "%e", "%h", "%l", "(%hl)", "%a"};
    static const char* cb_ops[] = {"rlc", "rrc", "rl", "rr", "sla", "sra", "swap", "srl"};
    static const char* cb_bin_ops[] = {"bit", "res", "set"};

    const char* cb_op = nullptr;

    uint8_t data[2];
    inst->data(data);

    uint8_t op = data[0];
    uint8_t op_idx = op / 8;

    if (op_idx < 8) {
        cb_op = cb_ops[op_idx];
    } else {
        uint8_t op_bin_bit = (op_idx - 8) % 8;
        uint8_t op_bin_op_idx = (op_idx - 8) / 8;
        snprintf(cb_ops_tmp, 256, "%s $%i,", cb_bin_ops[op_bin_op_idx], op_bin_bit);
        cb_op = cb_ops_tmp;
    } 
    DISASS_FUNC_RET("cb $0x%02X ; %s %s", op, cb_op, cb_regs[op % 8]);
}

DISASS_FUNC(call_a16) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("call $0x%04X", unpack_addr(data));
}

DISASS_FUNC(jr_r8) {
    uint8_t data[2];
    inst->data(data);
    uint8_t d = data[0];

    DISASS_FUNC_RET("jr $0x%02X ; ($%i)", d, (int8_t) d);
}

DISASS_FUNC(ld_ma16_sp) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ld ($0x%04X), %%sp", unpack_addr(data));
}

DISASS_FUNC(ld_reg_d8, const char* reg) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ld %s, $0x%02X", reg, data[0]);
}

DISASS_FUNC(ld_reg_d16, const char* reg) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ld %s, $0x%04X", reg, unpack_addr(data));
}

DISASS_FUNC(add_hl_reg, const char* reg) {
    DISASS_FUNC_RET("add %%hl, %s", reg);
}

DISASS_FUNC(ld_ma16_a) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ld ($0x%04X), %%a", unpack_addr(data));
}

DISASS_FUNC(ld_a_ma16) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ld %%a, ($0x%04X)", unpack_addr(data));
}

DISASS_FUNC(ldh_a_ma8) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ldh %%a, ($0x%04X)", 0xFF00 + data[0]);
}

DISASS_FUNC(ldh_ma8_a) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("ldh ($0x%04X), %%a", 0xFF00 + data[0]);
}

DISASS_FUNC(op_a_d8, const char* op) {
    uint8_t data[2];
    inst->data(data);
    DISASS_FUNC_RET("%s %%a, $0x%02X", op, data[0]);
}

DISASS_FUNC(add_sp_r8) {
    uint8_t data[2];
    inst->data(data);
    uint8_t d = data[0];

    DISASS_FUNC_RET("add %%sp, $0x%02X ; ($%i)", d, (int8_t) d);
}

DISASS_FUNC(ld_hl_sppr8) {
    uint8_t data[2];
    inst->data(data);
    uint8_t d = data[0];

    DISASS_FUNC_RET("ld %%hl, %%sp + $0x%02X ; ($%i)", d, (int8_t) d);
}

DISASS_FUNC(ld_sp_hl) {
    DISASS_FUNC_RET("ld %%sp, %%hl");
}

DISASS_FUNC(jmp_mhl) {
    DISASS_FUNC_RET("jmp (%%hl)");
}

DISASS_FUNC(single_op, const char* op) {
    DISASS_FUNC_RET("%s", op);
}

const char* disassemble(const GB_Z80_Instruction* inst) {
    switch (inst->opcode()) {
#define X(Opcode, FuncName, ...)                                            \
    case Opcode:                                                            \
        return FuncName(inst, ## __VA_ARGS__);
# include "opcodes.gdef"
#undef X

    default:
        return "failed to disassemble opcode";
    };
}
