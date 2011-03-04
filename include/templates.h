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

#ifndef __templates_h__
#define __templates_h__

#define DEFAULT_RM8_reg8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		BYTE value = cpu->readModRM8(rm); \
		cpu->updateModRM8(helper ## 8(cpu, value, *cpu->treg8[vomit_modRMRegisterPart(rm)])); \
	}

#define DEFAULT_RM16_reg16(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		WORD value = cpu->readModRM16(rm); \
		cpu->updateModRM16(helper ## 16(cpu, value, *cpu->treg16[vomit_modRMRegisterPart(rm)])); \
	}

#define DEFAULT_reg8_RM8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		BYTE value = cpu->readModRM8(rm); \
		*cpu->treg8[vomit_modRMRegisterPart(rm)] = helper ## 8(cpu, *cpu->treg8[vomit_modRMRegisterPart(rm)], value); \
	}

#define DEFAULT_reg16_RM16(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		WORD value = cpu->readModRM16(rm); \
		*cpu->treg16[vomit_modRMRegisterPart(rm)] = helper ## 16(cpu, *cpu->treg16[vomit_modRMRegisterPart(rm)], value); \
	}

#define DEFAULT_reg32_RM32(helper, name) \
        void name(VCpu* cpu) { \
                BYTE rm = cpu->fetchOpcodeByte(); \
                DWORD value = cpu->readModRM32(rm); \
                *cpu->treg32[vomit_modRMRegisterPart(rm)] = helper ## 32(cpu, *cpu->treg32[vomit_modRMRegisterPart(rm)], value); \
        }

#define DEFAULT_RM8_imm8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE value = cpu->readModRM8(cpu->rmbyte); \
		cpu->updateModRM8(helper ## 8(cpu, value, cpu->fetchOpcodeByte())); \
	}

#define DEFAULT_RM16_imm16(helper, name) \
	void name(VCpu* cpu) { \
		WORD value = cpu->readModRM16(cpu->rmbyte); \
		cpu->updateModRM16(helper ## 16(cpu, value, cpu->fetchOpcodeWord())); \
	}

#define DEFAULT_RM32_imm32(helper, name) \
	void name(VCpu* cpu) { \
		DWORD value = cpu->readModRM32(cpu->rmbyte); \
                cpu->updateModRM32(helper ## 32(cpu, value, cpu->fetchOpcodeDWord())); \
	}

#define DEFAULT_RM16_imm8(helper, name) \
	void name(VCpu* cpu) { \
		WORD value = cpu->readModRM16(cpu->rmbyte); \
		cpu->updateModRM16(helper ## 16(cpu, value, vomit_signExtend(cpu->fetchOpcodeByte()))); \
	}

#define DEFAULT_RM32_imm8(helper, name) \
        void name(VCpu* cpu) { \
                DWORD value = cpu->readModRM32(cpu->rmbyte); \
                cpu->updateModRM32(helper ## 32(cpu, value, vomit_signExtend(cpu->fetchOpcodeByte()))); \
        }

#define DEFAULT_AL_imm8(helper, name) \
	void name(VCpu* cpu) { \
                cpu->regs.B.AL = helper ## 8(cpu, cpu->getAL(), cpu->fetchOpcodeByte()); \
	}

#define DEFAULT_AX_imm16(helper, name) \
	void name(VCpu* cpu) { \
                cpu->regs.W.AX = helper ## 16(cpu, cpu->getAX(), cpu->fetchOpcodeWord()); \
	}

#define DEFAULT_EAX_imm32(helper, name) \
        void name(VCpu* cpu) { \
                cpu->regs.D.EAX = helper ## 32(cpu, cpu->getEAX(), cpu->fetchOpcodeDWord()); \
        }

#define READONLY_RM8_reg8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		helper ## 8(cpu, cpu->readModRM8(rm), *cpu->treg8[vomit_modRMRegisterPart(rm)]); \
	}

#define READONLY_RM16_reg16(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		helper ## 16(cpu, cpu->readModRM16(rm), *cpu->treg16[vomit_modRMRegisterPart(rm)]); \
	}

#define READONLY_RM32_reg32(helper, name) \
        void name(VCpu* cpu) { \
                BYTE rm = cpu->fetchOpcodeByte(); \
                helper ## 32(cpu, cpu->readModRM32(rm), *cpu->treg32[vomit_modRMRegisterPart(rm)]); \
        }

#define READONLY_reg8_RM8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		helper ## 8(cpu, *cpu->treg8[vomit_modRMRegisterPart(rm)], cpu->readModRM8(rm)); \
	}

#define READONLY_reg16_RM16(helper, name) \
	void name(VCpu* cpu) { \
		BYTE rm = cpu->fetchOpcodeByte(); \
		helper ## 16(cpu, *cpu->treg16[vomit_modRMRegisterPart(rm)], cpu->readModRM16(rm)); \
	}

#define READONLY_reg32_RM32(helper, name) \
        void name(VCpu* cpu) { \
                BYTE rm = cpu->fetchOpcodeByte(); \
                helper ## 32(cpu, *cpu->treg32[vomit_modRMRegisterPart(rm)], cpu->readModRM32(rm)); \
        }

#define READONLY_RM8_imm8(helper, name) \
	void name(VCpu* cpu) { \
		BYTE value = cpu->readModRM8(cpu->rmbyte); \
		helper ## 8(cpu, value, cpu->fetchOpcodeByte()); \
	}

#define READONLY_RM16_imm16( helper, name ) \
	void name(VCpu* cpu) { \
		WORD value = cpu->readModRM16(cpu->rmbyte); \
		helper ## 16(cpu, value, cpu->fetchOpcodeWord()); \
	}

#define READONLY_RM32_imm8( helper, name ) \
    void name(VCpu* cpu) { \
            DWORD value = cpu->readModRM32(cpu->rmbyte); \
            helper ## 32(cpu, value, vomit_signExtend(cpu->fetchOpcodeByte())); \
    }

#define READONLY_RM32_imm32( helper, name ) \
	void name(VCpu* cpu) { \
		DWORD value = cpu->readModRM32(cpu->rmbyte); \
		helper ## 32(cpu, value, cpu->fetchOpcodeDWord()); \
	}

#define READONLY_RM16_imm8(helper, name) \
	void name(VCpu* cpu) { \
		WORD value = cpu->readModRM16(cpu->rmbyte); \
		helper ## 16(cpu, value, vomit_signExtend(cpu->fetchOpcodeByte())); \
	}

#define READONLY_AL_imm8(helper, name) \
	void name(VCpu* cpu) { \
                helper ## 8(cpu, cpu->getAL(), cpu->fetchOpcodeByte()); \
	}

#define READONLY_AX_imm16(helper, name) \
	void name(VCpu* cpu) { \
                helper ## 16(cpu, cpu->getAX(), cpu->fetchOpcodeWord()); \
	}

#define READONLY_EAX_imm32(helper, name) \
        void name(VCpu* cpu) { \
                helper ## 32(cpu, cpu->getEAX(), cpu->fetchOpcodeDWord()); \
        }

#define DEFAULT_RM32_reg32(helper, name) \
        void name(VCpu* cpu) { \
                BYTE rm = cpu->fetchOpcodeByte(); \
                DWORD value = cpu->readModRM32(rm); \
                cpu->updateModRM32(helper ## 32(cpu, value, *cpu->treg32[vomit_modRMRegisterPart(rm)])); \
        }

#endif /* __templates_h__ */
