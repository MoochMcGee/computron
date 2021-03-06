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

#include <stdio.h>
#include "Common.h"
#include "CPU.h"
#include "debug.h"
#include "Tasking.h"

unsigned CPU::dumpDisassembledInternal(SegmentDescriptor& descriptor, DWORD offset)
{
    char buf[512];
    char* p = buf;
    BYTE* data = nullptr;

    try {
        data = memoryPointer(descriptor, offset);
    } catch (...) {
        data = nullptr;
    }

    if (!data) {
        vlog(LogCPU, "dumpDisassembled can't dump %04x:%08x", descriptor.index(), offset);
        return 0;
    }

    SimpleInstructionStream stream(data);
    auto insn = Instruction::fromStream(stream, m_operandSize32, m_addressSize32);

    if (x32())
        p += sprintf(p, "%04x:%08x ", descriptor.index(), offset);
    else
        p += sprintf(p, "%04x:%04x ", descriptor.index(), offset);

    for (unsigned i = 0; i < insn.length(); ++i)
        p += sprintf(p, "%02x", data[i]);

    for (unsigned i = 0; i < (32-(insn.length()*2)); ++i)
        p += sprintf(p, " ");

    if (insn.isValid())
        p += sprintf(p, " %s", qPrintable(insn.toString(offset, x32())));
    else
        p += sprintf(p, " <invalid instruction>");

    vlog(LogDump, buf);
    return insn.length();
}

unsigned CPU::dumpDisassembled(SegmentDescriptor& descriptor, DWORD offset, unsigned count)
{
    unsigned bytes = 0;
    for (unsigned i = 0; i < count; ++i) {
        bytes += dumpDisassembledInternal(descriptor, offset + bytes);
    }
    return bytes;
}

unsigned CPU::dumpDisassembled(LogicalAddress address, unsigned count)
{
    auto descriptor = getSegmentDescriptor(address.selector());
    return dumpDisassembled(descriptor, address.offset(), count);
}

#ifdef CT_TRACE
void CPU::dumpTrace()
{
#if 0
    fprintf(stderr,
        "%04X:%08X "
        "EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESP=%08X EBP=%08X ESI=%08X EDI=%08X "
        "CR0=%08X CR1=%08X CR2=%08X CR3=%08X CR4=%08X CR5=%08X CR6=%08X CR7=%08X "
        "DR0=%08X DR1=%08X DR2=%08X DR3=%08X DR4=%08X DR5=%08X DR6=%08X DR7=%08X "
        "DS=%04X ES=%04X SS=%04X FS=%04X GS=%04X "
        "C=%u P=%u A=%u Z=%u S=%u I=%u D=%u O=%u\n",
        getCS(), getEIP(),
        getEAX(), getEBX(), getECX(), getEDX(), getESP(), getEBP(), getESI(), getEDI(),
        getCR0(), getCR1(), getCR2(), getCR3(), getCR4(), getCR5(), getCR6(), getCR7(),
        getDR0(), getDR1(), getDR2(), getDR3(), getDR4(), getDR5(), getDR6(), getDR7(),
        getDS(), getES(), getSS(), getFS(), getGS(),
        getCF(), getPF(), getAF(), getZF(),
        getSF(), getIF(), getDF(), getOF()
    );
#else
    printf(
        "%04X:%08X %02X "
        "EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESP=%08X EBP=%08X ESI=%08X EDI=%08X "
        "CR0=%08X CR3=%08X CPL=%u IOPL=%u A20=%u "
        "DS=%04X ES=%04X SS=%04X FS=%04X GS=%04X "
        "C=%u P=%u A=%u Z=%u S=%u I=%u D=%u O=%u "
        "NT=%u VM=%u "
        "A%u O%u X%u S%u\n",
        getCS(), getEIP(),
        readMemory8(SegmentRegisterIndex::CS, getEIP()),
        getEAX(), getEBX(), getECX(), getEDX(), getESP(), getEBP(), getESI(), getEDI(),
        getCR0(), getCR3(), getCPL(), getIOPL(),
        isA20Enabled(),
        getDS(), getES(), getSS(), getFS(), getGS(),
        getCF(), getPF(), getAF(), getZF(),
        getSF(), getIF(), getDF(), getOF(),
        getNT(), getVM(),
        a16() ? 16 : 32,
        o16() ? 16 : 32,
        x16() ? 16 : 32,
        s16() ? 16 : 32
    );
#endif
}
#endif

void CPU::dumpSelector(const char* prefix, SegmentRegisterIndex segreg)
{
    auto& descriptor = cachedDescriptor(segreg);
    if (descriptor.isNull())
        vlog(LogDump, "%s: %04x: (null descriptor)", prefix, readSegmentRegister(segreg));
    else
        dumpDescriptor(descriptor, prefix);
}

const char* CPU::registerName(SegmentRegisterIndex index)
{
    switch (index) {
    case SegmentRegisterIndex::CS:
        return "cs";
    case SegmentRegisterIndex::DS:
        return "ds";
    case SegmentRegisterIndex::ES:
        return "es";
    case SegmentRegisterIndex::SS:
        return "ss";
    case SegmentRegisterIndex::FS:
        return "fs";
    case SegmentRegisterIndex::GS:
        return "gs";
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

const char* CPU::registerName(CPU::RegisterIndex8 registerIndex)
{
    switch (registerIndex) {
    case CPU::RegisterAL:
        return "al";
    case CPU::RegisterBL:
        return "bl";
    case CPU::RegisterCL:
        return "cl";
    case CPU::RegisterDL:
        return "dl";
    case CPU::RegisterAH:
        return "ah";
    case CPU::RegisterBH:
        return "bh";
    case CPU::RegisterCH:
        return "ch";
    case CPU::RegisterDH:
        return "dh";
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

const char* CPU::registerName(CPU::RegisterIndex16 registerIndex)
{
    switch (registerIndex) {
    case CPU::RegisterAX:
        return "ax";
    case CPU::RegisterBX:
        return "bx";
    case CPU::RegisterCX:
        return "cx";
    case CPU::RegisterDX:
        return "dx";
    case CPU::RegisterBP:
        return "bp";
    case CPU::RegisterSP:
        return "sp";
    case CPU::RegisterSI:
        return "si";
    case CPU::RegisterDI:
        return "di";
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

const char* CPU::registerName(CPU::RegisterIndex32 registerIndex)
{
    switch (registerIndex) {
    case CPU::RegisterEAX:
        return "eax";
    case CPU::RegisterEBX:
        return "ebx";
    case CPU::RegisterECX:
        return "ecx";
    case CPU::RegisterEDX:
        return "edx";
    case CPU::RegisterEBP:
        return "ebp";
    case CPU::RegisterESP:
        return "esp";
    case CPU::RegisterESI:
        return "esi";
    case CPU::RegisterEDI:
        return "edi";
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

void CPU::dumpWatches()
{
    for (WatchedAddress& watch : m_watches) {
        if (watch.size == ByteSize) {
            BYTE data = readUnmappedMemory8(watch.address);
            if (data != watch.lastSeenValue) {
                vlog(LogDump, "\033[32;1m%08X\033[0m [%-16s] %02X", watch.address, qPrintable(watch.name), data);
                watch.lastSeenValue = data;
            }
        } else if (watch.size == WordSize) {
            WORD data = readUnmappedMemory16(watch.address);
            if (data != watch.lastSeenValue) {
                vlog(LogDump, "\033[32;1m%08X\033[0m [%-16s] %04X", watch.address, qPrintable(watch.name), data);
                watch.lastSeenValue = data;
            }
        } else if (watch.size == DWordSize) {
            DWORD data = readUnmappedMemory32(watch.address);
            if (data != watch.lastSeenValue) {
                vlog(LogDump, "\033[32;1m%08X\033[0m [%-16s] %08X", watch.address, qPrintable(watch.name), data);
                watch.lastSeenValue = data;
            }
        }
    }
}

void CPU::dumpAll()
{
    if (getPE() && TR.selector != 0) {
        auto descriptor = getDescriptor(TR.selector);
        if (descriptor.isTSS()) {
            auto& tssDescriptor = descriptor.asTSSDescriptor();
            TSS tss(*this, tssDescriptor.base(), tssDescriptor.is32Bit());
            dumpTSS(tss);
        }
    }

    vlog(LogDump, "eax: %08x  ebx: %08x  ecx: %08x  edx: %08x", getEAX(), getEBX(), getECX(), getEDX());
    vlog(LogDump, "ebp: %08x  esp: %08x  esi: %08x  edi: %08x", getEBP(), getESP(), getESI(), getEDI());

    if (!getPE()) {
        vlog(LogDump, "ds: %04x  es: %04x ss: %04x  fs: %04x  gs: %04x", getDS(), getES(), getSS(), getFS(), getGS());
        vlog(LogDump, "cs: %04x eip: %08x", getCS(), getEIP());
    } else {
        dumpSelector("ds: ", SegmentRegisterIndex::DS);
        dumpSelector("es: ", SegmentRegisterIndex::ES);
        dumpSelector("ss: ", SegmentRegisterIndex::SS);
        dumpSelector("fs: ", SegmentRegisterIndex::FS);
        dumpSelector("gs: ", SegmentRegisterIndex::GS);
        dumpSelector("cs: ", SegmentRegisterIndex::CS);
        vlog(LogDump, "eip: %08x", getEIP());
    }
    vlog(LogDump, "cpl: %u  iopl: %u  a20: %u", getCPL(), getIOPL(), isA20Enabled());
    vlog(LogDump, "a%u[%u] o%u[%u] s%u x%u",
         m_effectiveAddressSize32 ? 32 : 16,
         m_addressSize32 ? 32 : 16,
         m_effectiveOperandSize32 ? 32 : 16,
         m_operandSize32 ? 32 : 16,
         s16() ? 16 : 32,
         x16() ? 16 : 32);

    vlog(LogDump, "cr0: %08x  cr3: %08x", getCR0(), getCR3());
    vlog(LogDump, "idtr: {base=%08x, limit=%04x}", IDTR.base.get(), IDTR.limit);
    vlog(LogDump, "gdtr: {base=%08x, limit=%04x}", GDTR.base.get(), GDTR.limit);
    vlog(LogDump, "ldtr: {base=%08x, limit=%04x, (selector=%04x)}", LDTR.base.get(), LDTR.limit, LDTR.selector);
    vlog(LogDump, "  tr: {base=%08x, limit=%04x, (selector=%04x, %u-bit)}", TR.base.get(), TR.limit, TR.selector, TR.is32Bit ? 32 : 16);

    vlog(LogDump, "cf=%u pf=%u af=%u zf=%u sf=%u if=%u df=%u of=%u tf=%u nt=%u vm=%u", getCF(), getPF(), getAF(), getZF(), getSF(), getIF(), getDF(), getOF(), getTF(), getNT(), getVM());

    dumpDisassembled(cachedDescriptor(SegmentRegisterIndex::CS), currentBaseInstructionPointer());
}

static inline BYTE n(BYTE b)
{
    if (b < 0x20 || ((b > 127) && (b < 160)))
        return '.';
    return b;
}

void CPU::dumpFlatMemory(DWORD address)
{
    address &= 0xFFFFFFF0;

    BYTE* p = &m_memory[address];
    int rows = 16;

    for (int i = 0; i < rows; ++i) {
        vlog(LogDump,
            "%08X   %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X   %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
            (address+i*16),
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15],
            n(p[0]), n(p[1]), n(p[2]), n(p[3]), n(p[4]), n(p[5]), n(p[6]), n(p[7]),
            n(p[8]), n(p[9]), n(p[10]), n(p[11]), n(p[12]), n(p[13]), n(p[14]), n(p[15])
        );
        p+=16;
    }

    p = &m_memory[address];
    for (int i = 0; i < rows; ++i) {
        fprintf(stderr,
            "db 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]
        );
        p+=16;
    }
}

void CPU::dumpRawMemory(BYTE* p)
{
    int rows = 16;
    vlog(LogDump, "Raw dump %p", p);
    for (int i = 0; i < rows; ++i) {
        fprintf(stderr,
            "db 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]
        );
        p+=16;
    }
}

void CPU::dumpMemory(SegmentDescriptor& descriptor, DWORD offset, int rows)
{
    offset &= 0xFFFFFFF0;
    BYTE* p;

    try {
        p = memoryPointer(descriptor, offset);
    } catch (...) {
        p = nullptr;
    }

    if (!p) {
        vlog(LogCPU, "dumpMemory can't dump %04x:%08x", descriptor.index(), offset);
        return;
    }

    for (int i = 0; i < rows; ++i) {
        vlog(LogDump,
            "%04x:%04x   %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X   %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
            descriptor.index(), (offset+i*16),
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15],
            n(p[0]), n(p[1]), n(p[2]), n(p[3]), n(p[4]), n(p[5]), n(p[6]), n(p[7]),
            n(p[8]), n(p[9]), n(p[10]), n(p[11]), n(p[12]), n(p[13]), n(p[14]), n(p[15])
        );
        p+=16;
    }

    p = memoryPointer(descriptor, offset);
    for (int i = 0; i < rows; ++i) {
        fprintf(stderr,
            "db 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]
        );
        p+=16;
    }
}

void CPU::dumpMemory(LogicalAddress address, int rows)
{
    auto descriptor = getSegmentDescriptor(address.selector());
    return dumpMemory(descriptor, address.offset(), rows);
}

static inline WORD isrSegment(const CPU& cpu, BYTE isr)
{
    return cpu.readUnmappedMemory16(isr * 4 + 2);
}

static inline WORD isrOffset(const CPU& cpu, BYTE isr)
{
    return cpu.readUnmappedMemory16(isr * 4);
}

void CPU::dumpIVT()
{
    // XXX: For alignment reasons, we're skipping INT FF
    for (int i = 0; i < 0xFF; i += 4) {
        vlog(LogDump,
            "%02x>  %04x:%04x\t%02x>  %04x:%04x\t%02x>  %04x:%04x\t%02X>  %04x:%04x",
            i + 0, isrSegment(*this, i + 0), isrOffset(*this, i + 0),
            i + 1, isrSegment(*this, i + 1), isrOffset(*this, i + 1),
            i + 2, isrSegment(*this, i + 2), isrOffset(*this, i + 2),
            i + 3, isrSegment(*this, i + 3), isrOffset(*this, i + 3)
        );
    }
}

void CPU::dumpDescriptor(const Descriptor& descriptor, const char* prefix)
{
    if (descriptor.isNull())
        vlog(LogCPU, "%s%04x (null descriptor)", prefix, descriptor.index());
    else if (descriptor.isSegmentDescriptor())
        dumpDescriptor(descriptor.asSegmentDescriptor(), prefix);
    else
        dumpDescriptor(descriptor.asSystemDescriptor(), prefix);
}

void CPU::dumpDescriptor(const SegmentDescriptor& descriptor, const char* prefix)
{
    if (descriptor.isNull())
        vlog(LogCPU, "%s%04x (null descriptor)", prefix, descriptor.index());
    else if (descriptor.isCode())
        dumpDescriptor(descriptor.asCodeSegmentDescriptor(), prefix);
    else
        dumpDescriptor(descriptor.asDataSegmentDescriptor(), prefix);
}

void CPU::dumpDescriptor(const Gate& gate, const char* prefix)
{
    vlog(LogCPU, "%s%04x (gate) { type: %s (%02x), entry:%04x:%06x, params:%u, bits:%u, p:%u, dpl:%u }",
        prefix,
        gate.index(),
        gate.typeName(),
        (BYTE)gate.type(),
        gate.selector(),
        gate.offset(),
        gate.parameterCount(),
        gate.D() ? 32 : 16,
        gate.present(),
        gate.DPL()
    );
    if (gate.isCallGate()) {
        vlog(LogCPU, "Call gate points to:");
        dumpDescriptor(getDescriptor(gate.selector()), prefix);
    }
}

void CPU::dumpDescriptor(const SystemDescriptor& descriptor, const char* prefix)
{
    if (descriptor.isGate()) {
        dumpDescriptor(descriptor.asGate(), prefix);
        return;
    }
    if (descriptor.isLDT()) {
        vlog(LogCPU, "%s%04x (system segment) { type: LDT (%02x), base:%08x e-limit:%08x, p:%u }",
            prefix,
            descriptor.index(),
            (BYTE)descriptor.type(),
            descriptor.asLDTDescriptor().base(),
            descriptor.asLDTDescriptor().effectiveLimit(),
            descriptor.present()
        );
        return;
    }
    vlog(LogCPU, "%s%04x (system segment) { type: %s (%02x), bits:%u, p:%u, dpl:%u }",
        prefix,
        descriptor.index(),
        descriptor.typeName(),
        (BYTE)descriptor.type(),
        descriptor.D() ? 32 : 16,
        descriptor.present(),
        descriptor.DPL()
    );
}

void CPU::dumpDescriptor(const CodeSegmentDescriptor& segment, const char* prefix)
{
    vlog(LogCPU, "%s%04x (%s segment) { type: code, base:%08x, e-limit:%08x, bits:%u, p:%u, g:%s, dpl:%u, a:%u, readable:%u, conforming:%u }",
        prefix,
        segment.index(),
        segment.isGlobal() ? "global" : " local",
        segment.base(),
        segment.effectiveLimit(),
        segment.D() ? 32 : 16,
        segment.present(),
        segment.granularity() ? "4k" : "1b",
        segment.DPL(),
        segment.accessed(),
        segment.conforming(),
        segment.readable()
    );
}

void CPU::dumpDescriptor(const DataSegmentDescriptor& segment, const char* prefix)
{
    vlog(LogCPU, "%s%04x (%s segment) { type: data, base:%08x, e-limit:%08x, bits:%u, p:%u, g:%s, dpl:%u, a:%u, writable:%u, expandDown:%u }",
        prefix,
        segment.index(),
        segment.isGlobal() ? "global" : " local",
        segment.base(),
        segment.effectiveLimit(),
        segment.D() ? 32 : 16,
        segment.present(),
        segment.granularity() ? "4k" : "1b",
        segment.DPL(),
        segment.accessed(),
        segment.writable(),
        segment.expandDown()
    );
}

void CPU::dumpSegment(WORD index)
{
    dumpDescriptor(getDescriptor(index));
}

void CPU::dumpStack(ValueSize valueSize, unsigned count)
{
    DWORD sp = currentStackPointer();
    for (unsigned i = 0; i < count; ++i) {
        if (valueSize == DWordSize) {
            DWORD value = readMemory32(SegmentRegisterIndex::SS, sp);
            vlog(LogDump, "%04x:%08x (+%04x) %08x", getSS(), sp, sp - currentStackPointer(), value);
            sp += 4;
        }
    }
}
