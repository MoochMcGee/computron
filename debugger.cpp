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

#include "debugger.h"
#include "Common.h"
#include "debug.h"
#include "CPU.h"
#include "pic.h"
#include "machine.h"
#include "pic.h"
#include <QDebug>
#include <QStringBuilder>
#include <QStringList>
#include <QLatin1Literal>
#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

Debugger::Debugger(CPU& cpu)
    : m_cpu(cpu)
{
}

Debugger::~Debugger()
{
}

void Debugger::enter()
{
    m_active = true;
    cpu().recomputeMainLoopNeedsSlowStuff();
}

void Debugger::exit()
{
    m_active = false;
    cpu().recomputeMainLoopNeedsSlowStuff();
}

static QString doPrompt(const CPU& cpu)
{
    static QString brightMagenta("\033[35;1m");
    static QString brightCyan("\033[34;1m");
    static QString defaultColor("\033[0m");

    QString s;

    if (cpu.getPE())
        s.sprintf("%04X:%08X", cpu.getCS(), cpu.getEIP());
    else
        s.sprintf("%04X:%04X", cpu.getCS(), cpu.getIP());

    QString prompt = brightMagenta % QLatin1Literal("CT ") % brightCyan % s % defaultColor % QLatin1Literal("> ");

#ifdef HAVE_READLINE
    char* line = readline(prompt.toLatin1().constData());
#else
    char* line = (char*)malloc(1024 * sizeof(char));
    fgets(line, 1024, stdin);
#endif

    QString command = line ? QString::fromUtf8(line) : QString::fromLatin1("end-of-file");
    free(line);
    return command;
}

void Debugger::handleCommand(const QString& rawCommand)
{
    QStringList arguments = rawCommand.split(QChar(' '), QString::SkipEmptyParts);

    if (arguments.isEmpty())
        return;

    QString command = arguments.takeFirst();
    QString lowerCommand = command.toLower();

    if (lowerCommand == "q" || lowerCommand == "quit" || lowerCommand == "end-of-file")
        return handleQuit();

    if (lowerCommand == "r" || lowerCommand == "dump-registers")
        return handleDumpRegisters();

    if (lowerCommand == "i" || lowerCommand == "dump-ivt")
        return handleDumpIVT();

    if (lowerCommand == "reconf")
        return handleReconfigure();

    if (lowerCommand == "t" || lowerCommand == "tracing")
        return handleTracing(arguments);

    if (lowerCommand == "s" || lowerCommand == "step")
        return handleStep();

    if (lowerCommand == "c" || lowerCommand == "continue")
        return handleContinue();

    if (lowerCommand == "d" || lowerCommand == "dump-memory")
        return handleDumpMemory(arguments);

    if (lowerCommand == "u")
        return handleDumpUnassembled(arguments);

    if (lowerCommand == "seg")
        return handleDumpSegment(arguments);

    if (lowerCommand == "m")
        return handleDumpFlatMemory(arguments);

    if (lowerCommand == "b")
        return handleBreakpoint(arguments);

    if (lowerCommand == "sel")
        return handleSelector(arguments);

    if (lowerCommand == "k" || lowerCommand == "stack")
        return handleStack(arguments);

    if (lowerCommand == "gdt") {
        cpu().dumpGDT();
        return;
    }

    if (lowerCommand == "ldt") {
        cpu().dumpLDT();
        return;
    }

    if (lowerCommand == "sti") {
        vlog(LogDump, "IF <- 1");
        cpu().setIF(1);
        return;
    }

    if (lowerCommand == "cli") {
        vlog(LogDump, "IF <- 0");
        cpu().setIF(0);
        return;
    }

    if (lowerCommand == "irq")
        return handleIRQ(arguments);

    if (lowerCommand == "picmasks") {
        cpu().machine().masterPIC().dumpMask();
        cpu().machine().slavePIC().dumpMask();
        return;
    }

    if (lowerCommand == "unmask") {
        cpu().machine().masterPIC().unmaskAll();
        cpu().machine().slavePIC().unmaskAll();
        return;
    }

    if (lowerCommand == "slon") {
        options.stacklog = true;
        return;
    }

    if (lowerCommand == "sloff") {
        options.stacklog = false;
        return;
    }

#ifdef DISASSEMBLE_EVERYTHING
    if (lowerCommand == "de1") {
        options.disassembleEverything = true;
        return;
    }
    if (lowerCommand == "de0") {
        options.disassembleEverything = false;
        return;
    }
#endif

    printf("Unknown command: %s\n", command.toUtf8().constData());
}

void Debugger::handleIRQ(const QStringList& arguments)
{
    if (arguments.size() != 1)
        goto usage;

    if (arguments[0] == "off") {
        printf("Ignoring all IRQs\n");
        PIC::setIgnoreAllIRQs(true);
        return;
    }

    if (arguments[0] == "on") {
        printf("Allowing all IRQs\n");
        PIC::setIgnoreAllIRQs(false);
        return;
    }

usage:
    printf("usage: irq <on|off>\n");
}

void Debugger::handleBreakpoint(const QStringList& arguments)
{
    if (arguments.size() != 3) {
        printf("usage: b <add|del> <segment> <offset>\n");
        if (!cpu().breakpoints().empty()) {
            printf("\nCurrent breakpoints:\n");
            for (auto& breakpoint : cpu().breakpoints()) {
                printf("    %04x:%08x\n", breakpoint.selector(), breakpoint.offset());
            }
            printf("\n");
        }
        return;
    }
    WORD selector = arguments.at(1).toUInt(0, 16);
    DWORD offset = arguments.at(2).toUInt(0, 16);
    LogicalAddress address(selector, offset);
    if (arguments[0] == "add") {
        printf("add breakpoint: %04x:%08x\n", selector, offset);
        cpu().breakpoints().insert(address);
    }
    if (arguments[0] == "del") {
        printf("delete breakpoint: %04x:%08x\n", selector, offset);
        cpu().breakpoints().erase(address);
    }
    cpu().recomputeMainLoopNeedsSlowStuff();
}

void Debugger::doConsole()
{
    ASSERT(isActive());

    printf("\n");
    cpu().dumpAll();
    printf(">>> Entering Computron debugger @ %04x:%08x\n", cpu().getBaseCS(), cpu().currentBaseInstructionPointer());

    while (isActive()) {
        QString rawCommand = doPrompt(cpu());
        handleCommand(rawCommand);
    }
}

void Debugger::handleQuit()
{
    hard_exit(0);
}

void Debugger::handleDumpRegisters()
{
    cpu().dumpAll();
}

void Debugger::handleDumpIVT()
{
    cpu().dumpIVT();
}

void Debugger::handleReconfigure()
{
    // FIXME: Implement.
}

void Debugger::handleStep()
{
    cpu().executeOneInstruction();
    cpu().dumpAll();
    cpu().dumpWatches();
    vlog(LogDump, "Next instruction:");
    cpu().dumpDisassembled(cpu().cachedDescriptor(SegmentRegisterIndex::CS), cpu().getEIP());
}

void Debugger::handleContinue()
{
    exit();
}

void Debugger::handleSelector(const QStringList& arguments)
{
    if (arguments.size() == 0) {
        vlog(LogDump, "usage: sel <selector>");
        return;
    }
    WORD select = arguments.at(0).toUInt(0, 16);
    cpu().dumpDescriptor(cpu().getDescriptor(select));
}

void Debugger::handleStack(const QStringList& arguments)
{
    UNUSED_PARAM(arguments);
    cpu().dumpStack(DWordSize, 16);
}

void Debugger::handleDumpMemory(const QStringList& arguments)
{
    WORD selector = cpu().getCS();
    DWORD offset = cpu().getEIP();

    if (arguments.size() == 1)
        offset = arguments.at(0).toUInt(0, 16);
    else if (arguments.size() == 2) {
        selector = arguments.at(0).toUInt(0, 16);
        offset = arguments.at(1).toUInt(0, 16);
    }

    cpu().dumpMemory(LogicalAddress(selector, offset), 16);
}

void Debugger::handleDumpUnassembled(const QStringList& arguments)
{
    WORD selector = cpu().getCS();
    DWORD offset = cpu().getEIP();

    if (arguments.size() == 1)
        offset = arguments.at(0).toUInt(0, 16);
    else if (arguments.size() == 2) {
        selector = arguments.at(0).toUInt(0, 16);
        offset = arguments.at(1).toUInt(0, 16);
    }

    DWORD bytesDisassembled = cpu().dumpDisassembled(LogicalAddress(selector, offset), 20);
    vlog(LogDump, "Next offset: %08x", offset + bytesDisassembled);
}

void Debugger::handleDumpSegment(const QStringList& arguments)
{
    WORD segment = cpu().getCS();

    if (arguments.size() >= 1)
        segment = arguments.at(0).toUInt(0, 16);

    cpu().dumpSegment(segment);
}

void Debugger::handleDumpFlatMemory(const QStringList& arguments)
{
    DWORD address = cpu().getEIP();

    if (arguments.size() == 1)
        address = arguments.at(0).toUInt(0, 16);

    cpu().dumpFlatMemory(address);
}

void Debugger::handleTracing(const QStringList& arguments)
{
    if (arguments.size() == 1) {
        unsigned value = arguments.at(0).toUInt(0, 16);
        options.trace = value != 0;
        cpu().recomputeMainLoopNeedsSlowStuff();
        return;
    }

    printf("Usage: tracing <0|1>\n");
}
