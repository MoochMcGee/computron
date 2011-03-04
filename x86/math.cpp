/*
 * Copyright (C) 2003-2011 Andreas Kling <kling@webkit.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREAS KLING ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANDREAS KLING OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "vcpu.h"
#include "templates.h"

WORD cpu_add8(VCpu* cpu, BYTE dest, BYTE src)
{
    WORD result = dest + src;
    cpu->mathFlags8(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(7))&1);
    return result;
}

DWORD cpu_add16(VCpu* cpu, WORD dest, WORD src)
{
    DWORD result = dest + src;
    cpu->mathFlags16(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(15))&1);
    return result;
}

QWORD cpu_add32(VCpu* cpu, DWORD dest, DWORD src)
{
    QWORD result = dest + src;
    cpu->mathFlags32(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(31))&1);
    return result;
}

WORD cpu_adc8(VCpu* cpu, WORD dest, WORD src)
{
    WORD result;
    src += cpu->getCF();
    result = dest + src;

    cpu->mathFlags8(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(7))&1);
    return result;
}

DWORD cpu_adc16(VCpu* cpu, WORD dest, WORD src)
{
    DWORD result;
    src += cpu->getCF();
    result = dest + src;

    cpu->mathFlags16(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(15))&1);
    return result;
}

QWORD cpu_adc32(VCpu* cpu, DWORD dest, DWORD src)
{
    QWORD result;
    src += cpu->getCF();
    result = dest + src;

    cpu->mathFlags32(result, dest, src);
    cpu->setOF(((
             ((result)^(dest)) &
             ((result)^(src))
             )>>(31))&1);
    return result;
}

WORD cpu_sub8(VCpu* cpu, BYTE dest, BYTE src)
{
    WORD result = dest - src;
    cpu->cmpFlags8(result, dest, src);
    return result;
}

DWORD cpu_sub16(VCpu* cpu, WORD dest, WORD src)
{
    DWORD result = dest - src;
    cpu->cmpFlags16(result, dest, src);
    return result;
}

QWORD cpu_sub32(VCpu* cpu, DWORD dest, DWORD src)
{
    QWORD result = dest - src;
    cpu->cmpFlags32(result, dest, src);
    return result;
}

WORD cpu_sbb8(VCpu* cpu, BYTE dest, BYTE src)
{
    WORD result;
    src += cpu->getCF();
    result = dest - src;
    cpu->cmpFlags8(result, dest, src);
    return result;
}

DWORD cpu_sbb16(VCpu* cpu, WORD dest, WORD src)
{
    DWORD result;
    src += cpu->getCF();
    result = dest - src;
    cpu->cmpFlags16(result, dest, src);
    return result;
}

QWORD cpu_sbb32(VCpu* cpu, DWORD dest, DWORD src)
{
    DWORD result;
    src += cpu->getCF();
    result = dest - src;
    cpu->cmpFlags32(result, dest, src);
    return result;
}

WORD cpu_mul8(VCpu* cpu, BYTE acc, BYTE multi)
{
    WORD result = acc * multi;
    cpu->mathFlags8(result, acc, multi);
    return result;
}

DWORD cpu_mul16(VCpu* cpu, WORD acc, WORD multi)
{
    DWORD result = acc * multi;
    cpu->mathFlags16(result, acc, multi);
    return result;
}

QWORD cpu_mul32(VCpu* cpu, DWORD acc, DWORD multi)
{
    QWORD result = acc * multi;
    cpu->mathFlags32(result, acc, multi);
    return result;
}

SIGNED_WORD cpu_imul8(VCpu* cpu, SIGNED_BYTE acc, SIGNED_BYTE multi)
{
    SIGNED_WORD result = acc * multi;
    cpu->mathFlags8(result, acc, multi);
    return result;
}


SIGNED_DWORD cpu_imul16(VCpu* cpu, SIGNED_WORD acc, SIGNED_WORD multi)
{
    SIGNED_DWORD result = acc * multi;
    cpu->mathFlags16(result, acc, multi);
    return result;
}

DEFAULT_RM8_reg8(cpu_add, _ADD_RM8_reg8)
DEFAULT_RM16_reg16(cpu_add, _ADD_RM16_reg16)
DEFAULT_RM32_reg32(cpu_add, _ADD_RM32_reg32)
DEFAULT_reg8_RM8(cpu_add, _ADD_reg8_RM8)
DEFAULT_reg16_RM16(cpu_add, _ADD_reg16_RM16)
DEFAULT_reg32_RM32(cpu_add, _ADD_reg32_RM32)
DEFAULT_RM8_imm8(cpu_add, _ADD_RM8_imm8)
DEFAULT_RM16_imm16(cpu_add, _ADD_RM16_imm16)
DEFAULT_RM32_imm32(cpu_add, _ADD_RM32_imm32)
DEFAULT_RM16_imm8(cpu_add, _ADD_RM16_imm8)
DEFAULT_RM32_imm8(cpu_add, _ADD_RM32_imm8)
DEFAULT_AL_imm8(cpu_add, _ADD_AL_imm8)
DEFAULT_AX_imm16(cpu_add, _ADD_AX_imm16)
DEFAULT_EAX_imm32(cpu_add, _ADD_EAX_imm32)

DEFAULT_RM8_reg8(cpu_adc, _ADC_RM8_reg8)
DEFAULT_RM16_reg16(cpu_adc, _ADC_RM16_reg16)
DEFAULT_RM32_reg32(cpu_adc, _ADC_RM32_reg32)
DEFAULT_reg8_RM8(cpu_adc, _ADC_reg8_RM8)
DEFAULT_reg16_RM16(cpu_adc, _ADC_reg16_RM16)
DEFAULT_reg32_RM32(cpu_adc, _ADC_reg32_RM32)
DEFAULT_RM8_imm8(cpu_adc, _ADC_RM8_imm8)
DEFAULT_RM16_imm16(cpu_adc, _ADC_RM16_imm16)
DEFAULT_RM32_imm32(cpu_adc, _ADC_RM32_imm32)
DEFAULT_RM16_imm8(cpu_adc, _ADC_RM16_imm8)
DEFAULT_RM32_imm8(cpu_adc, _ADC_RM32_imm8)
DEFAULT_AL_imm8(cpu_adc, _ADC_AL_imm8)
DEFAULT_AX_imm16(cpu_adc, _ADC_AX_imm16)
DEFAULT_EAX_imm32(cpu_adc, _ADC_EAX_imm32)

DEFAULT_RM8_reg8(cpu_sub, _SUB_RM8_reg8)
DEFAULT_RM16_reg16(cpu_sub, _SUB_RM16_reg16)
DEFAULT_RM32_reg32(cpu_sub, _SUB_RM32_reg32)
DEFAULT_reg8_RM8(cpu_sub, _SUB_reg8_RM8)
DEFAULT_reg16_RM16(cpu_sub, _SUB_reg16_RM16)
DEFAULT_reg32_RM32(cpu_sub, _SUB_reg32_RM32)
DEFAULT_RM8_imm8(cpu_sub, _SUB_RM8_imm8)
DEFAULT_RM16_imm16(cpu_sub, _SUB_RM16_imm16)
DEFAULT_RM32_imm32(cpu_sub, _SUB_RM32_imm32)
DEFAULT_RM16_imm8(cpu_sub, _SUB_RM16_imm8)
DEFAULT_RM32_imm8(cpu_sub, _SUB_RM32_imm8)
DEFAULT_AL_imm8(cpu_sub, _SUB_AL_imm8)
DEFAULT_AX_imm16(cpu_sub, _SUB_AX_imm16)
DEFAULT_EAX_imm32(cpu_sub, _SUB_EAX_imm32)

DEFAULT_RM8_reg8(cpu_sbb, _SBB_RM8_reg8)
DEFAULT_RM16_reg16(cpu_sbb, _SBB_RM16_reg16)
DEFAULT_RM32_reg32(cpu_sbb, _SBB_RM32_reg32)
DEFAULT_reg8_RM8(cpu_sbb, _SBB_reg8_RM8)
DEFAULT_reg16_RM16(cpu_sbb, _SBB_reg16_RM16)
DEFAULT_reg32_RM32(cpu_sbb, _SBB_reg32_RM32)
DEFAULT_RM8_imm8(cpu_sbb, _SBB_RM8_imm8)
DEFAULT_RM16_imm16(cpu_sbb, _SBB_RM16_imm16)
DEFAULT_RM32_imm32(cpu_sbb, _SBB_RM32_imm32)
DEFAULT_RM16_imm8(cpu_sbb, _SBB_RM16_imm8)
DEFAULT_RM32_imm8(cpu_sbb, _SBB_RM32_imm8)
DEFAULT_AL_imm8(cpu_sbb, _SBB_AL_imm8)
DEFAULT_AX_imm16(cpu_sbb, _SBB_AX_imm16)
DEFAULT_EAX_imm32(cpu_sbb, _SBB_EAX_imm32)

READONLY_RM8_reg8(cpu_sub, _CMP_RM8_reg8)
READONLY_RM16_reg16(cpu_sub, _CMP_RM16_reg16)
READONLY_RM32_reg32(cpu_sub, _CMP_RM32_reg32)
READONLY_reg8_RM8(cpu_sub, _CMP_reg8_RM8)
READONLY_reg16_RM16(cpu_sub, _CMP_reg16_RM16)
READONLY_reg32_RM32(cpu_sub, _CMP_reg32_RM32)
READONLY_RM8_imm8(cpu_sub, _CMP_RM8_imm8)
READONLY_RM16_imm16(cpu_sub, _CMP_RM16_imm16)
READONLY_RM32_imm32(cpu_sub, _CMP_RM32_imm32)
READONLY_RM16_imm8(cpu_sub, _CMP_RM16_imm8)
READONLY_RM32_imm8(cpu_sub, _CMP_RM32_imm8)
READONLY_AL_imm8(cpu_sub, _CMP_AL_imm8)
READONLY_AX_imm16(cpu_sub, _CMP_AX_imm16)
READONLY_EAX_imm32(cpu_sub, _CMP_EAX_imm32)

void _MUL_RM8(VCpu* cpu)
{
    BYTE value = cpu->readModRM8(cpu->rmbyte);
    cpu->regs.W.AX = cpu_mul8(cpu, cpu->regs.B.AL, value);

    if (cpu->regs.B.AH == 0x00) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _MUL_RM16(VCpu* cpu)
{
    WORD value = cpu->readModRM16(cpu->rmbyte);
    DWORD result = cpu_mul16(cpu, cpu->regs.W.AX, value);
    cpu->regs.W.AX = result & 0xFFFF;
    cpu->regs.W.DX = (result >> 16) & 0xFFFF;

    if (cpu->regs.W.DX == 0x0000) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _MUL_RM32(VCpu* cpu)
{
    WORD value = cpu->readModRM32(cpu->rmbyte);
    QWORD result = cpu_mul32(cpu, cpu->regs.W.AX, value);
    cpu->setEAX(result & 0xFFFFFFFF);
    cpu->setEDX((result >> 32) & 0xFFFFFFFF);

    if (cpu->getEDX() == 0x00000000) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _IMUL_RM8(VCpu* cpu)
{
    SIGNED_BYTE value = (SIGNED_BYTE)cpu->readModRM8(cpu->rmbyte);
    cpu->regs.W.AX = (SIGNED_WORD)cpu_imul8(cpu, cpu->regs.B.AL, value);

    if (cpu->regs.B.AH == 0x00 || cpu->regs.B.AH == 0xFF) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _IMUL_reg16_RM16_imm8(VCpu* cpu)
{
    BYTE rm = cpu->fetchOpcodeByte();
    BYTE imm = cpu->fetchOpcodeByte();
    SIGNED_WORD value = (SIGNED_WORD)cpu->readModRM16(rm);
    SIGNED_WORD result = cpu_imul16(cpu, value, imm);

    *cpu->treg16[vomit_modRMRegisterPart(rm)] = result;

    if ((result & 0xFF00) == 0x00 || (result & 0xFF00) == 0xFF) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _IMUL_RM16(VCpu* cpu)
{
    SIGNED_WORD value = cpu->readModRM16(cpu->rmbyte);
    SIGNED_DWORD result = cpu_imul16(cpu, cpu->regs.W.AX, value);
    cpu->regs.W.AX = result;
    cpu->regs.W.DX = result >> 16;

    if (cpu->regs.W.DX == 0x0000 || cpu->regs.W.DX == 0xFFFF) {
        cpu->setCF(0);
        cpu->setOF(0);
    } else {
        cpu->setCF(1);
        cpu->setOF(1);
    }
}

void _DIV_RM8(VCpu* cpu)
{
    BYTE value = cpu->readModRM8(cpu->rmbyte);
    WORD tAX = cpu->regs.W.AX;

    if (value == 0) {
        cpu->exception(0);
        return;
    }

    cpu->regs.B.AL = (BYTE)(tAX / value); // Quote
    cpu->regs.B.AH = (BYTE)(tAX % value); // Remainder
}

void _DIV_RM16(VCpu* cpu)
{
    WORD value = cpu->readModRM16(cpu->rmbyte);
    DWORD tDXAX = cpu->regs.W.AX + (cpu->regs.W.DX << 16);

    if (value == 0) {
        cpu->exception(0);
        return;
    }

    cpu->regs.W.AX = (WORD)(tDXAX / value); // Quote
    cpu->regs.W.DX = (WORD)(tDXAX % value); // Remainder
}

void _DIV_RM32(VCpu* cpu)
{
    DWORD value = cpu->readModRM32(cpu->rmbyte);
    QWORD tEDXEAX = cpu->getEAX() | (cpu->getEDX() << 16);

    if (value == 0) {
        cpu->exception(0);
        return;
    }

    cpu->setEAX(tEDXEAX / value); // Quote
    cpu->setEDX(tEDXEAX % value); // Remainder
}

void _IDIV_RM8(VCpu* cpu)
{
    SIGNED_BYTE value = (SIGNED_BYTE)cpu->readModRM8(cpu->rmbyte);
    SIGNED_WORD tAX = (SIGNED_WORD)cpu->regs.W.AX;

    if (value == 0) {
        cpu->exception(0);
        return;
    }

    cpu->regs.B.AL = (SIGNED_BYTE)(tAX / value); // Quote
    cpu->regs.B.AH = (SIGNED_BYTE)(tAX % value); // Remainder
}

void _IDIV_RM16(VCpu* cpu)
{
    SIGNED_WORD value = cpu->readModRM16(cpu->rmbyte);
    SIGNED_DWORD tDXAX = (cpu->regs.W.AX + (cpu->regs.W.DX << 16));

    if (value == 0) {
        cpu->exception(0);
        return;
    }

    cpu->regs.W.AX = (SIGNED_WORD)(tDXAX / value); // Quote
    cpu->regs.W.DX = (SIGNED_WORD)(tDXAX % value); // Remainder
}
