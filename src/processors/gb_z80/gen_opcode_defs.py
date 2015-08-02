from __future__ import print_function

import os

_ROOT = os.path.abspath(os.path.dirname(__file__))

lines = []

lines.extend('''
X(0x08, ld_ma16_sp)
X(0x18, jr_r8)
X(0x33, inc_reg, "%sp")
X(0x34, inc_reg, "(%hl)")
X(0x35, dec_reg, "(%hl)")
X(0x3C, inc_reg, "%a")
X(0x3D, dec_reg, "%a")
X(0xC3, jmp_a16)
X(0xCB, cb)
X(0xCD, call_a16)
X(0xE0, ldh_ma8_a)
X(0xE2, ld_mc_a)
X(0xE8, add_sp_r8)
X(0xE9, jmp_mhl)
X(0xEA, ld_ma16_a)
X(0xF0, ldh_a_ma8)
X(0xF2, ld_a_mc)
X(0xF8, ld_hl_sppr8)
X(0xF9, ld_sp_hl)
X(0xFA, ld_a_ma16)
'''.strip().splitlines())

def r(reg):
    return '"%{reg}"'.format(reg=reg)

def f(opcode, *a):
    m = 'X(0x{opcode:02X}, {a})'
    return m.format(opcode=opcode, a=', '.join(str(x) for x in a))

for i, reg in enumerate(['bc', 'de', 'hl']):
    # INC
    lines.append(f(0x10 * i + 0x3, 'inc_reg', r(reg)))
    lines.append(f(0x10 * i + 0x4, 'inc_reg', r(reg[0])))
    lines.append(f(0x10 * i + 0xC, 'inc_reg', r(reg[1])))
    # DEC
    lines.append(f(0x10 * i + 0x5, 'dec_reg', r(reg[0])))
    lines.append(f(0x10 * i + 0xB, 'dec_reg', r(reg)))
    lines.append(f(0x10 * i + 0xD, 'dec_reg', r(reg[1])))

for i, reg in enumerate(['bc', 'de', 'hl', 'af']):
    lines.append(f(0xC0 + 0x10 * i + 0x1, 'pop_reg', r(reg)))
    lines.append(f(0xC0 + 0x10 * i + 0x5, 'push_reg', r(reg)))

for i1, op in enumerate(['add', 'adc', 'sub', 'sbc', 'and', 'xor', 'or', 'cp']):
    op = '"{}"'.format(op)
    for i2, reg in enumerate([r(a) for a in 'bcdehl'] + ['"(%hl)"', r('a')]):
        lines.append(f(0x80 + 0x8 * i1 + i2, 'op_a_reg', op, reg))
    lines.append(f(0xC6 + 0x8 * i1, 'op_a_d8', op))

for i, reg in enumerate(['bc', 'de', 'hl', 'sp']):
    lines.append(f(0x10 * i + 0x1, 'ld_reg_d16', r(reg)))

for i, reg in enumerate(['bc', 'de', 'hl', 'sp']):
    lines.append(f(0x09 + 0x10 * i, 'add_hl_reg', r(reg)))

for i1, reg1 in enumerate([r(a) for a in 'bcdehl'] + ['"(%hl)"', r('a')]):
    for i2, reg2 in enumerate([r(a) for a in 'bcdehl'] + ['"(%hl)"', r('a')]):
        if (reg1, reg2) == ('"(%hl)"', '"(%hl)"'):
            continue
        lines.append(f(0x40 + 0x8 * i1 + i2, 'ld_reg_reg', reg1, reg2))
    lines.append(f(0x06 + 0x08 * i1, 'ld_reg_d8', reg1))

for i, reg in enumerate(['bc', 'de', 'hl+', 'hl-']):
    lines.append(f(0x10 * i + 0x2, 'ld_mreg_a', r(reg)))
    lines.append(f(0x10 * i + 0xA, 'ld_a_mreg', r(reg)))

for i in range(0x00, 0x40, 0x8):
    lines.append(f(0xC7 + i, 'rst_nn', i))

for i, flag in enumerate(['"{}"'.format(x) for x in ['nzf', 'zf', 'ncy', 'cy']]):
    lines.append(f(0xC0 + 0x8 * i, 'ret_flag', flag))
    lines.append(f(0x20 + 0x8 * i, 'jr_flag_r8', flag))
    lines.append(f(0xC2 + 0x8 * i, 'jmp_flag_a16', flag))
    lines.append(f(0xC2 + 0x8 * i + 0x2, 'call_flag_a16', flag))

for opcode, op in [(0x00, 'nop'), (0x10, 'stop'), (0xFB, 'ei'),
                 (0xF3, 'di'), (0x76, 'halt'), (0xC9, 'ret'),
                 (0xD9, 'reti')]:
    lines.append(f(opcode, 'single_op', '"{}"'.format(op)))

for i, op in enumerate(['rlca', 'rrca', 'rla', 'rra', 'daa', 'cpl', 'scf', 'ccf']):
    lines.append(f(0x07 + 0x08 * i, 'single_op', '"{}"'.format(op)))

with open(os.path.join(_ROOT, 'opcodes.gdef'), 'w') as fd:
    print('\n'.join(sorted(lines)), file=fd)
