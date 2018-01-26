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

#include "vomit.h"
#include "vcpu.h"
#include "debug.h"
#include "iodevice.h"

void VCpu::_OUT_imm8_AL()
{
    out(fetchOpcodeByte(), regs.B.AL);
}

void VCpu::_OUT_imm8_AX()
{
    WORD port = fetchOpcodeByte();
    out(port, regs.B.AL);
    out(port + 1, regs.B.AH);
}

void VCpu::_OUT_imm8_EAX()
{
    WORD port = fetchOpcodeByte();
    out(port, regs.B.AL);
    out(port + 1, regs.B.AH);
    out(port + 2, vomit_LSB(regs.W.__EAX_high_word));
    out(port + 3, vomit_MSB(regs.W.__EAX_high_word));
}

void VCpu::_OUT_DX_AL()
{
    out(getDX(), regs.B.AL);
}

void VCpu::_OUT_DX_AX()
{
    out(getDX(), regs.B.AL);
    out(getDX() + 1, regs.B.AH);
}

void VCpu::_OUT_DX_EAX()
{
    out(getDX(), regs.B.AL);
    out(getDX() + 1, regs.B.AH);
    out(getDX() + 2, vomit_LSB(regs.W.__EAX_high_word));
    out(getDX() + 3, vomit_MSB(regs.W.__EAX_high_word));
}

void VCpu::_OUTSB()
{
    BYTE data;

    if (a16()) {
        data = readMemory8(currentSegment(), getSI());
        nextSI(1);
    } else {
        data = readMemory8(currentSegment(), getESI());
        nextESI(1);
    }

    out(getDX(), data);
}

void VCpu::_OUTSW()
{
    BYTE lsb;
    BYTE msb;

    if (a16()) {
        lsb = readMemory8(currentSegment(), regs.W.SI);
        msb = readMemory8(currentSegment(), regs.W.SI + 1);
        nextSI(2);
    } else {
        lsb = readMemory8(currentSegment(), regs.D.ESI);
        msb = readMemory8(currentSegment(), regs.D.ESI + 1);
        nextESI(2);
    }

    out(getDX(), lsb);
    out(getDX() + 1, msb);
}

void VCpu::_OUTSD()
{
    BYTE b1, b2, b3, b4;

    if (a16()) {
        b1 = readMemory8(currentSegment(), regs.W.SI);
        b2 = readMemory8(currentSegment(), regs.W.SI + 1);
        b3 = readMemory8(currentSegment(), regs.W.SI + 2);
        b4 = readMemory8(currentSegment(), regs.W.SI + 3);
        nextSI(4);
    } else {
        b1 = readMemory8(currentSegment(), regs.D.ESI);
        b2 = readMemory8(currentSegment(), regs.D.ESI + 1);
        b3 = readMemory8(currentSegment(), regs.D.ESI + 2);
        b4 = readMemory8(currentSegment(), regs.D.ESI + 3);
        nextESI(4);
    }

    out(getDX(), b1);
    out(getDX() + 1, b2);
    out(getDX() + 2, b3);
    out(getDX() + 3, b4);
}

void VCpu::_IN_AL_imm8()
{
    regs.B.AL = in(fetchOpcodeByte());
}

void VCpu::_IN_AX_imm8()
{
    WORD port = fetchOpcodeByte();
    regs.B.AL = in(port);
    regs.B.AH = in(port + 1);
}

void VCpu::_IN_EAX_imm8()
{
    WORD port = fetchOpcodeByte();
    BYTE b1 = in(port);
    BYTE b2 = in(port + 1);
    BYTE b3 = in(port + 2);
    BYTE b4 = in(port + 3);
    regs.W.AX = vomit_MAKEWORD(b1, b2);
    regs.W.__EAX_high_word = vomit_MAKEWORD(b3, b4);
}

void VCpu::_IN_AL_DX()
{
    regs.B.AL = in(getDX());
}

void VCpu::_IN_AX_DX()
{
    regs.B.AL = in(getDX());
    regs.B.AH = in(getDX() + 1);
}

void VCpu::_IN_EAX_DX()
{
    regs.B.AL = in(getDX());
    regs.B.AH = in(getDX() + 1);
    BYTE c = in(getDX() + 2);
    BYTE d = in(getDX() + 3);
    regs.W.__EAX_high_word = vomit_MAKEWORD(c, d);
}

void VCpu::out(WORD port, BYTE value)
{
#ifdef VOMIT_DEBUG
    if (options.iopeek) {
        if (port != 0x00E6 && port != 0x0020 && port != 0x3D4 && port != 0x03d5 && port != 0xe2 && port != 0xe0) {
            vlog(LogIO, "[%04X:%08X] VCpu::out: %02X --> %04X", getBaseCS(), getBaseEIP(), value, port);
        }
    }
#endif

    if (IODevice::writeDevices().contains(port)) {
        IODevice::writeDevices()[port]->out8(port, value);
        return;
    }

    if (!IODevice::shouldIgnorePort(port))
        vlog(LogAlert, "%04X:%08X Unhandled I/O write to port %04X, data %02X", getBaseCS(), getBaseEIP(), port, value);
}

BYTE VCpu::in(WORD port)
{
#ifdef VOMIT_DEBUG
    if (options.iopeek) {
        if (port != 0x00E6 && port != 0x0020 && port != 0x3D4 && port != 0x03d5) {
            vlog(LogIO, "[%04X:%08X] VCpu::in: %04X", getBaseCS(), getBaseEIP(), port);
        }
    }
#endif

    if (IODevice::readDevices().contains(port))
        return IODevice::readDevices()[port]->in8(port);

    if (!IODevice::shouldIgnorePort(port))
        vlog(LogAlert, "Unhandled I/O read from port %04X", port);

    return IODevice::JunkValue;
}
