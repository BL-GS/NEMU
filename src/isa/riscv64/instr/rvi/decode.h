static int table_c_addi_dispatch(Decode *s);
static int table_c_addiw_dispatch(Decode *s);

static inline def_DopHelper(i) {
  op->imm = val;
  print_Dop(op->str, OP_STR_SIZE, (flag ? "0x%lx" : "%ld"), op->imm);
}

static inline def_DopHelper(r) {
  bool load_val = flag;
  static word_t zero_null = 0;
  op->preg = (!load_val && val == 0) ? &zero_null : &reg_l(val);
  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(val, 4));
#ifdef CONFIG_RVV_010
  op->reg = val;
#endif // CONFIG_RVV_010
}

static inline def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);
  decode_op_i(s, id_src2, (sword_t)s->isa.instr.i.simm11_0, false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}

static inline def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, true);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, false);
}

static inline def_DHelper(U) {
  decode_op_i(s, id_src1, (sword_t)s->isa.instr.u.simm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, false);
}

static inline def_DHelper(auipc) {
  decode_U(s, width);
  id_src1->imm += s->pc;
}

static inline def_DHelper(J) {
  sword_t offset = (s->isa.instr.j.simm20 << 20) | (s->isa.instr.j.imm19_12 << 12) |
    (s->isa.instr.j.imm11 << 11) | (s->isa.instr.j.imm10_1 << 1);
  decode_op_i(s, id_src1, s->pc + offset, true);
  decode_op_r(s, id_dest, s->isa.instr.j.rd, false);
  id_src2->imm = s->snpc;
}

static inline def_DHelper(B) {
  sword_t offset = (s->isa.instr.b.simm12 << 12) | (s->isa.instr.b.imm11 << 11) |
    (s->isa.instr.b.imm10_5 << 5) | (s->isa.instr.b.imm4_1 << 1);
  decode_op_i(s, id_dest, s->pc + offset, true);
  decode_op_r(s, id_src1, s->isa.instr.b.rs1, true);
  decode_op_r(s, id_src2, s->isa.instr.b.rs2, true);
}

static inline def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
}

def_THelper(load) {
  print_Dop(id_src1->str, OP_STR_SIZE, "%ld(%s)", id_src2->imm, reg_name(s->isa.instr.i.rs1, 4));
  int mmu_mode = isa_mmu_state();
  if (mmu_mode == MMU_DIRECT) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", ld);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu);
    def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", lwu);
  } else if (mmu_mode == MMU_TRANSLATE) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb_mmu);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh_mmu);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw_mmu);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", ld_mmu);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu_mmu);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu_mmu);
    def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", lwu_mmu);
  } else { assert(0); }
  return EXEC_ID_inv;
}

def_THelper(store) {
  print_Dop(id_src1->str, OP_STR_SIZE, "%ld(%s)", id_src2->imm, reg_name(s->isa.instr.i.rs1, 4));
  int mmu_mode = isa_mmu_state();
  if (mmu_mode == MMU_DIRECT) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sh);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sd);
  } else if (mmu_mode == MMU_TRANSLATE) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb_mmu);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sh_mmu);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw_mmu);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sd_mmu);
  } else { assert(0); }
  return EXEC_ID_inv;
}

def_THelper(addi_dispatch) {
  def_INSTR_TAB("0000000 00000 00000 ??? ????? ????? ??", p_li_0);
  def_INSTR_TAB("0000000 00001 00000 ??? ????? ????? ??", p_li_1);
  def_INSTR_TAB("??????? ????? 00000 ??? ????? ????? ??", c_li);
  def_INSTR_TAB("0000000 00000 ????? ??? ????? ????? ??", c_mv);
  def_INSTR_TAB("??????? ????? ????? ??? ????? ????? ??", addi);
  return EXEC_ID_inv;
}

def_THelper(op_imm) {
  if (s->isa.instr.i.rd == s->isa.instr.i.rs1) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", c_addi_dispatch);
    def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", c_andi);
    def_INSTR_TAB("000000? ????? ????? 001 ????? ????? ??", c_slli);
    def_INSTR_TAB("010000? ????? ????? 101 ????? ????? ??", c_srai);
    def_INSTR_TAB("000000? ????? ????? 101 ????? ????? ??", c_srli);
  }
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi_dispatch);
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", slti);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltui);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", ori);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);
  def_INSTR_TAB("000000? ????? ????? 001 ????? ????? ??", slli);
  def_INSTR_TAB("000000? ????? ????? 101 ????? ????? ??", srli);
  def_INSTR_TAB("010000? ????? ????? 101 ????? ????? ??", srai);
  #ifdef CONFIG_RVB
  def_INSTR_TAB("001010? ????? ????? 001 ????? ????? ??", bseti);
  def_INSTR_TAB("0010100 00111 ????? 101 ????? ????? ??", orc_b);
  def_INSTR_TAB("010010? ????? ????? 001 ????? ????? ??", bclri);
  def_INSTR_TAB("010010? ????? ????? 101 ????? ????? ??", bexti);
  def_INSTR_TAB("0110000 00000 ????? 001 ????? ????? ??", clz);
  def_INSTR_TAB("0110000 00001 ????? 001 ????? ????? ??", ctz);
  def_INSTR_TAB("0110000 00010 ????? 001 ????? ????? ??", cpop);
  def_INSTR_TAB("0110000 00100 ????? 001 ????? ????? ??", sext_b);
  def_INSTR_TAB("0110000 00101 ????? 001 ????? ????? ??", sext_h);
  def_INSTR_TAB("011000? ????? ????? 101 ????? ????? ??", rori);
  def_INSTR_TAB("011010? ????? ????? 001 ????? ????? ??", binvi);
  def_INSTR_TAB("0110101 11000 ????? 101 ????? ????? ??", rev8);
  def_INSTR_TAB("0110100 00111 ????? 101 ????? ????? ??", revb);
  #endif
  return EXEC_ID_inv;
};

def_THelper(op_imm32) {
  if (s->isa.instr.i.rd == s->isa.instr.i.rs1) {
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", c_addiw_dispatch);
  }
  def_INSTR_TAB("0000000 00000 ????? 000 ????? ????? ??", p_sext_w);
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addiw);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", slliw);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srliw);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sraiw);
  #ifdef CONFIG_RVB
  def_INSTR_TAB("000010? ????? ????? 001 ????? ????? ??", slliuw);
  def_INSTR_TAB("0110000 00000 ????? 001 ????? ????? ??", clzw);
  def_INSTR_TAB("0110000 00001 ????? 001 ????? ????? ??", ctzw);
  def_INSTR_TAB("0110000 00010 ????? 001 ????? ????? ??", cpopw);
  def_INSTR_TAB("0110000 ????? ????? 101 ????? ????? ??", roriw);
  #endif
  return EXEC_ID_inv;
}

def_THelper(op) {
  if (s->isa.instr.r.rd == s->isa.instr.r.rs1) {
    def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", c_add);
    def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", c_sub);
    def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", c_xor);
    def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", c_or);
    def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", c_and);
  }
  def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", add);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sll);
  def_INSTR_TAB("0000000 ????? ????? 010 ????? ????? ??", slt);
  def_INSTR_TAB("0000000 ????? ????? 011 ????? ????? ??", sltu);
  def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", xor);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srl);
  def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", or);
  def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", and);
  def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", sub);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sra);
  #ifdef CONFIG_RVB
  def_INSTR_TAB("0000101 ????? ????? 001 ????? ????? ??", clmul);
  def_INSTR_TAB("0000101 ????? ????? 010 ????? ????? ??", clmulr);
  def_INSTR_TAB("0000101 ????? ????? 011 ????? ????? ??", clmulh);
  def_INSTR_TAB("0000101 ????? ????? 100 ????? ????? ??", min);
  def_INSTR_TAB("0000101 ????? ????? 101 ????? ????? ??", minu);
  def_INSTR_TAB("0000101 ????? ????? 110 ????? ????? ??", max);
  def_INSTR_TAB("0000101 ????? ????? 111 ????? ????? ??", maxu);
  def_INSTR_TAB("0010000 ????? ????? 010 ????? ????? ??", sh1add);
  def_INSTR_TAB("0010000 ????? ????? 100 ????? ????? ??", sh2add);
  def_INSTR_TAB("0010000 ????? ????? 110 ????? ????? ??", sh3add);
  def_INSTR_TAB("0010100 ????? ????? 001 ????? ????? ??", bset);
  def_INSTR_TAB("0100000 ????? ????? 100 ????? ????? ??", xnor);
  def_INSTR_TAB("0100000 ????? ????? 110 ????? ????? ??", orn);
  def_INSTR_TAB("0100000 ????? ????? 111 ????? ????? ??", andn);
  def_INSTR_TAB("0100100 ????? ????? 001 ????? ????? ??", bclr);
  def_INSTR_TAB("0100100 ????? ????? 101 ????? ????? ??", bext);
  def_INSTR_TAB("0110000 ????? ????? 001 ????? ????? ??", rol);
  def_INSTR_TAB("0110000 ????? ????? 101 ????? ????? ??", ror);
  def_INSTR_TAB("0110100 ????? ????? 001 ????? ????? ??", binv);
  def_INSTR_TAB("0000100 ????? ????? 100 ????? ????? ??", pack);
  def_INSTR_TAB("0000100 ????? ????? 111 ????? ????? ??", packh);
  def_INSTR_TAB("0010100 ????? ????? 010 ????? ????? ??", xpermn);
  def_INSTR_TAB("0010100 ????? ????? 100 ????? ????? ??", xpermb);
  #endif
  return EXEC_ID_inv;
}

def_THelper(op32) {
  if (s->isa.instr.r.rd == s->isa.instr.r.rs1) {
    def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", c_addw);
    def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", c_subw);
  }
  def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", addw);
  def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", subw);
  def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sllw);
  def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srlw);
  def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sraw);
  #ifdef CONFIG_RVB
  def_INSTR_TAB("0000100 ????? ????? 000 ????? ????? ??", adduw);
  // def_INSTR_TAB("0000100 ????? ????? 100 ????? ????? ??", zext_h);
  def_INSTR_TAB("0010000 ????? ????? 010 ????? ????? ??", sh1adduw);
  def_INSTR_TAB("0010000 ????? ????? 100 ????? ????? ??", sh2adduw);
  def_INSTR_TAB("0010000 ????? ????? 110 ????? ????? ??", sh3adduw);
  def_INSTR_TAB("0110000 ????? ????? 001 ????? ????? ??", rolw);
  def_INSTR_TAB("0110000 ????? ????? 101 ????? ????? ??", rorw);
  def_INSTR_TAB("0000100 ????? ????? 100 ????? ????? ??", packw);
  #endif
  return EXEC_ID_inv;
}

def_THelper(branch) {
  def_INSTR_TAB("??????? 00000 ????? 000 ????? ????? ??", c_beqz);
  def_INSTR_TAB("??????? 00000 ????? 001 ????? ????? ??", c_bnez);
  def_INSTR_TAB("??????? 00000 ????? 100 ????? ????? ??", p_bltz);
  def_INSTR_TAB("??????? 00000 ????? 101 ????? ????? ??", p_bgez);

  def_INSTR_TAB("??????? ????? 00000 100 ????? ????? ??", p_bgtz);
  def_INSTR_TAB("??????? ????? 00000 101 ????? ????? ??", p_blez);

  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", bltu);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);
  return EXEC_ID_inv;
};

def_THelper(jal_dispatch) {
  def_INSTR_TAB("??????? ????? ????? ??? 00000 ????? ??", c_j);
  def_INSTR_TAB("??????? ????? ????? ??? 00001 ????? ??", p_jal);
  def_INSTR_TAB("??????? ????? ????? ??? ????? ????? ??", jal);
  return EXEC_ID_inv;
}

def_THelper(jalr_dispatch) {
  def_INSTR_TAB("0000000 00000 00001 ??? 00000 ????? ??", p_ret);
  def_INSTR_TAB("0000000 00000 ????? ??? 00000 ????? ??", c_jr);
  def_INSTR_TAB("??????? ????? ????? ??? ????? ????? ??", jalr);
  return EXEC_ID_inv;
}

def_THelper(mem_fence) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", fence);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", fence_i);
  return EXEC_ID_inv;
}