// Computron x86 PC Emulator
// Copyright (C) 2003-2018 Andreas Kling <awesomekling@gmail.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY ANDREAS KLING ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANDREAS KLING OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "Common.h"
#include "debug.h"

class CPU;

struct TSS32 {
    WORD backlink, __blh;
    DWORD esp0;
    WORD ss0, __ss0h;
    DWORD esp1;
    WORD ss1, __ss1h;
    DWORD esp2;
    WORD ss2, __ss2h;
    DWORD CR3, EIP, EFlags;
    DWORD EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI;
    WORD ES, __esh;
    WORD CS, __csh;
    WORD SS, __ssh;
    WORD DS, __dsh;
    WORD FS, __fsh;
    WORD GS, __gsh;
    WORD LDT, __ldth;
    WORD trace, iomapbase;
} __attribute__ ((packed));

struct TSS16 {
    WORD backlink;
    WORD sp0;
    WORD ss0;
    WORD sp1;
    WORD ss1;
    WORD sp2;
    WORD ss2;
    WORD IP;
    WORD flags;
    WORD AX, CX, DX, BX, SP, BP, SI, DI;
    WORD ES;
    WORD CS;
    WORD SS;
    WORD DS;
    WORD FS;
    WORD GS;
    WORD LDT;
} __attribute__ ((packed));

class TSS {
public:
    TSS(CPU&, LinearAddress, bool is32Bit);

    bool is32Bit() const { return m_is32Bit; }

    WORD getIOMapBase() const;

    DWORD getRingESP(BYTE) const;
    WORD getRingSS(BYTE) const;

    DWORD getESP0() const;
    DWORD getESP1() const;
    DWORD getESP2() const;
    WORD getSS0() const;
    WORD getSS1() const;
    WORD getSS2() const;

    WORD getBacklink() const;
    WORD getLDT() const;
    DWORD getEIP() const;

    void setCR3(DWORD);
    DWORD getCR3() const;

    WORD getCS() const;
    WORD getDS() const;
    WORD getES() const;
    WORD getSS() const;
    WORD getFS() const;
    WORD getGS() const;

    DWORD getEAX() const;
    DWORD getEBX() const;
    DWORD getECX() const;
    DWORD getEDX() const;
    DWORD getESI() const;
    DWORD getEDI() const;
    DWORD getESP() const;
    DWORD getEBP() const;
    DWORD getEFlags() const;

    void setCS(WORD);
    void setDS(WORD);
    void setES(WORD);
    void setSS(WORD);
    void setFS(WORD);
    void setGS(WORD);
    void setLDT(WORD);
    void setEIP(DWORD);
    void setEAX(DWORD);
    void setEBX(DWORD);
    void setECX(DWORD);
    void setEDX(DWORD);
    void setEBP(DWORD);
    void setESP(DWORD);
    void setESI(DWORD);
    void setEDI(DWORD);
    void setEFlags(DWORD);
    void setBacklink(WORD);

private:
    TSS16& tss16();
    TSS32& tss32();
    const TSS16& tss16() const;
    const TSS32& tss32() const;

    BYTE* m_pointer { nullptr };
    bool m_is32Bit { false };
};
