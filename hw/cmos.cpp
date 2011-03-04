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

#include "cmos.h"
#include "debug.h"
#include <QtCore/QDate>
#include <QtCore/QTime>

static CMOS the;

CMOS::CMOS()
    : IODevice("CMOS")
{
    m_registerIndex = 0;
    m_statusRegisterB = 0x00;
    listen(0x70, IODevice::WriteOnly);
    listen(0x71, IODevice::ReadWrite);
}

CMOS::~CMOS()
{
}

bool CMOS::inBinaryClockMode() const
{
    return m_statusRegisterB & 0x04;
}

bool CMOS::in24HourMode() const
{
    return m_statusRegisterB & 0x02;
}

BYTE CMOS::in8(WORD)
{
    BYTE value = 0;

    switch (m_registerIndex) {
    case 0x00: value = QTime::currentTime().second(); break;
    case 0x02: value = QTime::currentTime().minute(); break;
    case 0x04: value = QTime::currentTime().hour(); break;
    case 0x06: value = QDate::currentDate().dayOfWeek(); break;
    case 0x07: value = QDate::currentDate().day(); break;
    case 0x08: value = QDate::currentDate().month(); break;
    case 0x09: value = QDate::currentDate().year() % 100; break;
    case 0x0B: value = m_statusRegisterB; break;
    case 0x15: value = LSB(g_cpu->baseMemorySize() / 1024); break;
    case 0x16: value = MSB(g_cpu->baseMemorySize() / 1024); break;
    case 0x30:
    case 0x17: value = LSB(g_cpu->extendedMemorySize() / 1024); break;
    case 0x31:
    case 0x18: value = MSB(g_cpu->extendedMemorySize() / 1024); break;
    case 0x32: value = QDate::currentDate().year() / 100; break;
    default: vlog(VLOG_CMOS, "WARNING: Read unsupported register %02X", m_registerIndex);
    }

    if (!inBinaryClockMode()) {
        switch (m_registerIndex) {
        case 0x00:
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x32:
            BYTE bcd = (value / 10 << 4) | (value - (value / 10) * 10);;
            value = bcd;
            break;
        }
    }

    vlog(VLOG_CMOS, "Read register %02X (%02X)", m_registerIndex, value);
    return value;
}

void CMOS::out8(WORD, BYTE data)
{
    vlog(VLOG_CMOS, "Select register %02X", data);
    m_registerIndex = data;
}
