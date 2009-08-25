/* 8086/misc.c
 * Miscellaneous instructions
 *
 */

#include "vomit.h"
#include "debug.h"
#include <unistd.h>
#include <stdlib.h>

void
_NOP()
{
}

void
_HLT()
{
	cpu.state = CPU_HALTED;
	vlog( VM_CPUMSG, "%04X:%04X Halted", cpu.base_CS, cpu.base_IP );
}

void
_XLAT()
{
	cpu.regs.B.AL = mem_getbyte( *(cpu.CurrentSegment), cpu.regs.W.BX + cpu.regs.B.AL );
}

void
_CS()
{
	byte opcode;
	cpu.SegmentPrefix = cpu.CS;
	cpu.CurrentSegment = &cpu.SegmentPrefix;
	opcode = cpu_pfq_getbyte();
	cpu_optable[opcode]();
	cpu.CurrentSegment = &cpu.DS;
	if( cpu.TF && cpu.IF )
	{
		int_call( 1 );
	}
}

void
_DS()
{
	byte opcode;
	cpu.SegmentPrefix = cpu.DS;
	cpu.CurrentSegment = &cpu.SegmentPrefix;
	opcode = cpu_pfq_getbyte();
	cpu_optable[opcode]();
	cpu.CurrentSegment = &cpu.DS;
	if( cpu.TF && cpu.IF )
	{
		int_call( 1 );
	}
}

void
_ES()
{
	byte opcode;
	cpu.SegmentPrefix = cpu.ES;
	cpu.CurrentSegment = &cpu.SegmentPrefix;
	opcode = cpu_pfq_getbyte();
	cpu_optable[opcode]();
	cpu.CurrentSegment = &cpu.DS;
	if( cpu.TF && cpu.IF )
	{
		int_call( 1 );
	}
}

void
_SS() {
	byte opcode;
	cpu.SegmentPrefix = cpu.SS;
	cpu.CurrentSegment = &cpu.SegmentPrefix;
	opcode = cpu_pfq_getbyte();
	cpu_optable[opcode]();
	cpu.CurrentSegment = &cpu.DS;
	if( cpu.TF && cpu.IF )
	{
		int_call( 1 );
	}
}

void
_LAHF()
{
	cpu.regs.B.AH = cpu.CF | (cpu.PF * 4) | (cpu.AF * 16) | (cpu.ZF * 64) | (cpu.SF * 128) | 2;
}

void
_SAHF()
{
	cpu.CF = (cpu.regs.B.AH & 0x01) != 0;
	cpu.PF = (cpu.regs.B.AH & 0x04) != 0;
	cpu.AF = (cpu.regs.B.AH & 0x10) != 0;
	cpu.ZF = (cpu.regs.B.AH & 0x40) != 0;
	cpu.SF = (cpu.regs.B.AH & 0x80) != 0;
}

void
_XCHG_AX_reg16()
{
	word tmpax = cpu.regs.W.AX;
	cpu.regs.W.AX = *treg16[cpu.opcode & 7];
	*treg16[cpu.opcode & 7] = tmpax;
}

void
_XCHG_reg8_RM8()
{
	/* FIXME: Yuck... */
	byte rm = cpu_pfq_getbyte();
	byte value = modrm_read8( rm );
	byte tmp = *treg8[rmreg(rm)];
	*treg8[rmreg(rm)] = value;
	modrm_update8( tmp );
}

void
_XCHG_reg16_RM16()
{
	/* FIXME: Yuck... */
	byte rm = cpu_pfq_getbyte();
	word value = modrm_read16( rm );
	word tmp = *treg16[rmreg(rm)];
	*treg16[rmreg(rm)] = value;
	modrm_update16( tmp );
}

void
_DEC_reg16()
{
	dword i = *treg16[cpu.opcode & 7];

	/* Overflow if we'll wrap. */
	cpu.OF = i == 0x8000;

	i--;
	cpu_setAF(i, *treg16[cpu.opcode & 7], 1);
	cpu_update_flags16( i );
	--*treg16[cpu.opcode & 7];
}

void
_INC_reg16()
{
	dword i = *treg16[cpu.opcode & 7];

	/* Overflow if we'll wrap. */
	cpu.OF = i == 0x7FFF;

	i++;
	cpu_setAF(i,*treg16[cpu.opcode & 7],1);
	cpu_update_flags16( i );
	++*treg16[cpu.opcode & 7];
}

void
_INC_RM16()
{
	word value = modrm_read16( cpu.rmbyte );
	dword i = value;

	/* Overflow if we'll wrap. */
	cpu.OF = value == 0x7FFF;

	i++;
	cpu_setAF( i, value, 1 );
	cpu_update_flags16( i );
	modrm_update16( value + 1 );
}

void
_DEC_RM16()
{
	word value = modrm_read16( cpu.rmbyte );
	dword i = value;

	/* Overflow if we'll wrap. */
	cpu.OF = value == 0x8000;

	i--;
	cpu_setAF( i, value, 1 ); // XXX: i can be (dword)(-1)...
	cpu_update_flags16( i );
	modrm_update16( value - 1 );
}

word
signext (byte b) {
	word w = 0x0000 + b;
	if ((w&0x80)>0)
		return (w | 0xff00);
	else
		return (w & 0x00ff);
}