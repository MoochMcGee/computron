/*
 * Copyright (C) 2003-2018 Andreas Kling <awesomekling@gmail.com>
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

#include "CPU.h"

void CPU::_LODSB(Instruction&)
{
    if (a16()) {
        regs.B.AL = readMemory8(currentSegment(), getSI());
        nextSI(1);
    } else {
        regs.B.AL = readMemory8(currentSegment(), getESI());
        nextESI(1);
    }
}

void CPU::_LODSW(Instruction&)
{
    if (a16()) {
        regs.W.AX = readMemory16(currentSegment(), getSI());
        nextSI(2);
    } else {
        regs.W.AX = readMemory16(currentSegment(), getESI());
        nextESI(2);
    }
}

void CPU::_LODSD(Instruction&)
{
    if (a16()) {
        regs.D.EAX = readMemory32(currentSegment(), getSI());
        nextSI(4);
    } else {
        regs.D.EAX = readMemory32(currentSegment(), getESI());
        nextESI(4);
    }
}

void CPU::_STOSB(Instruction&)
{
    if (a16()) {
        writeMemory8(SegmentRegisterIndex::ES, getDI(), getAL());
        nextDI(1);
    } else {
        writeMemory8(SegmentRegisterIndex::ES, getEDI(), getAL());
        nextEDI(1);
    }
}

void CPU::_STOSW(Instruction&)
{
    if (a16()) {
        writeMemory16(SegmentRegisterIndex::ES, getDI(), getAX());
        nextDI(2);
    } else {
        writeMemory16(SegmentRegisterIndex::ES, getEDI(), getAX());
        nextEDI(2);
    }
}

void CPU::_STOSD(Instruction&)
{
    if (a16()) {
        writeMemory32(SegmentRegisterIndex::ES, getDI(), getEAX());
        nextDI(4);
    } else {
        writeMemory32(SegmentRegisterIndex::ES, getEDI(), getEAX());
        nextEDI(4);
    }
}

void CPU::_CMPSB(Instruction&)
{
    WORD src;
    WORD dest;

    if (a16()) {
        src = readMemory8(currentSegment(), getSI());
        dest = readMemory8(SegmentRegisterIndex::ES, getDI());
        nextSI(1);
        nextDI(1);
    } else {
        src = readMemory8(currentSegment(), getESI());
        dest = readMemory8(SegmentRegisterIndex::ES, getEDI());
        nextESI(1);
        nextEDI(1);
    }

    cmpFlags8(src - dest, src, dest);
}

void CPU::_CMPSW(Instruction&)
{
    DWORD src;
    DWORD dest;

    if (a16()) {
        src = readMemory16(currentSegment(), getSI());
        dest = readMemory16(SegmentRegisterIndex::ES, getDI());
        nextSI(2);
        nextDI(2);
    } else {
        src = readMemory16(currentSegment(), getESI());
        dest = readMemory16(SegmentRegisterIndex::ES, getEDI());
        nextESI(2);
        nextEDI(2);
    }

    cmpFlags16(src - dest, src, dest);
}

void CPU::_CMPSD(Instruction&)
{
    QWORD src;
    QWORD dest;

    if (a16()) {
        src = readMemory32(currentSegment(), getSI());
        dest = readMemory32(SegmentRegisterIndex::ES, getDI());
        nextSI(4);
        nextDI(4);
    } else {
        src = readMemory32(currentSegment(), getESI());
        dest = readMemory32(SegmentRegisterIndex::ES, getEDI());
        nextESI(4);
        nextEDI(4);
    }

    cmpFlags32(src - dest, src, dest);
}

void CPU::_SCASB(Instruction&)
{
    WORD dest;

    if (a16()) {
        dest = readMemory8(SegmentRegisterIndex::ES, getDI());
        nextDI(1);
    } else {
        dest = readMemory8(SegmentRegisterIndex::ES, getEDI());
        nextEDI(1);
    }

    cmpFlags8(getAL() - dest, getAL(), dest);
}

void CPU::_SCASW(Instruction&)
{
    DWORD dest;

    if (a16()) {
        dest = readMemory16(SegmentRegisterIndex::ES, getDI());
        nextDI(2);
    } else {
        dest = readMemory16(SegmentRegisterIndex::ES, getEDI());
        nextEDI(2);
    }

    cmpFlags16(getAX() - dest, getAX(), dest);
}

void CPU::_SCASD(Instruction&)
{
    QWORD dest;

    if (a16()) {
        dest = readMemory32(SegmentRegisterIndex::ES, getDI());
        nextDI(4);
    } else {
        dest = readMemory32(SegmentRegisterIndex::ES, getEDI());
        nextEDI(4);
    }

    cmpFlags32(getEAX() - dest, getEAX(), dest);
}

void CPU::_MOVSB(Instruction&)
{
    if (a16()) {
        BYTE tmpb = readMemory8(currentSegment(), getSI());
        writeMemory8(SegmentRegisterIndex::ES, getDI(), tmpb);
        nextSI(1);
        nextDI(1);
    } else {
        BYTE tmpb = readMemory8(currentSegment(), getESI());
        writeMemory8(SegmentRegisterIndex::ES, getEDI(), tmpb);
        nextESI(1);
        nextEDI(1);
    }
}

void CPU::_MOVSW(Instruction&)
{
    if (a16()) {
        WORD tmpw = readMemory16(currentSegment(), getSI());
        writeMemory16(SegmentRegisterIndex::ES, getDI(), tmpw);
        nextSI(2);
        nextDI(2);
    } else {
        WORD tmpw = readMemory16(currentSegment(), getESI());
        writeMemory16(SegmentRegisterIndex::ES, getEDI(), tmpw);
        nextESI(2);
        nextEDI(2);
    }
}

void CPU::_MOVSD(Instruction&)
{
    if (a16()) {
        DWORD tmpw = readMemory32(currentSegment(), regs.W.SI);
        writeMemory32(SegmentRegisterIndex::ES, regs.W.DI, tmpw);
        nextSI(4);
        nextDI(4);
    } else {
        DWORD tmpw = readMemory32(currentSegment(), regs.D.ESI);
        writeMemory32(SegmentRegisterIndex::ES, regs.D.EDI, tmpw);
        nextESI(4);
        nextEDI(4);
    }
}
