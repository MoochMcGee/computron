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

#include "MemoryProvider.h"
#include "iodevice.h"
#include "OwnPtr.h"
#include <QtCore/QObject>
#include <QtGui/QColor>

class VGA final : public QObject, public IODevice, public MemoryProvider {
    Q_OBJECT
public:
    explicit VGA(Machine&);
    virtual ~VGA();

    // IODevice
    virtual void reset() override;
    virtual BYTE in8(WORD port) override;
    virtual void out8(WORD port, BYTE data) override;

    // MemoryProvider
    virtual void writeMemory8(DWORD address, BYTE value) override;
    virtual BYTE readMemory8(DWORD address) override;

    BYTE* plane(int index) const;

    void setPaletteDirty(bool);
    bool isPaletteDirty();

    BYTE readRegister(BYTE index);
    BYTE readRegister2(BYTE index);
    BYTE readSequencer(BYTE index);

    void writeRegister(BYTE index, BYTE value);

    QColor color(int index) const;
    QColor paletteColor(int paletteIndex) const;

    BYTE currentVideoMode() const;

    WORD startAddress() const;

    void willRefreshScreen();
    void didRefreshScreen();

    bool inChain4Mode() const;

signals:
    void paletteChanged();

private:
    void synchronizeColors();

    struct Private;
    OwnPtr<Private> d;
};
