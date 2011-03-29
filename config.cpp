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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vomit.h"
#include "floppy.h"
#include "debug.h"

static bool reloading = false;

typedef struct
{
    char* name;
    WORD sectors_per_track;
    WORD heads;
    DWORD sectors;
    WORD bytes_per_sector;
    BYTE media_type;
} disk_type_t;

static disk_type_t floppy_types[] =
{
    { "1.44M", 18, 2, 2880, 512, 4 },
    { "720kB",  9, 2, 1440, 512, 3 },
    { "1.2M",  15, 2, 2400, 512, 2 },
    { "360kB",  9, 2,  720, 512, 1 },
    { "320kB",  8, 2,  640, 512, 0 },
    { "160kB",  8, 1,  320, 512, 0 },
    { 0L,       0, 0,    0,   0, 0 }
};

void unspeakable_abomination()
{
    char curline[256], *curtok;
    char lfname[MAX_FN_LENGTH];
    WORD lseg, loff, ldrv, lspt, lhds, lsect, lsectsize;

    FILE* fconf = fopen("vm.conf", "r");
    if (!fconf) {
        vlog(LogConfig, "Couldn't load vm.conf");
        return;
    }

    while (!feof(fconf)) {
        if (!fgets(curline, 256, fconf))
            break;
        if ((curline[0] != '#') && (strlen(curline) > 1)) {
            curtok = strtok(curline, " \t\n");
            if (strcmp(curtok,"loadfile") == 0) {
                strcpy(lfname, strtok(NULL, " \t\n"));
                lseg = strtol(strtok(NULL, ": \t\n"),NULL,16);
                loff = strtol(strtok(NULL, " \t\n"),NULL,16);

                vlog(LogInit, "Loading %s to %04X:%04X", lfname, lseg, loff);

                FILE* ftmp = fopen(lfname, "rb");
                if (!ftmp) {
                    vlog(LogConfig, "Error while processing \"loadfile\" line.");
                    vm_exit(1);
                }

                void* destination = g_cpu->memoryPointer(lseg, loff);
                size_t bytesRead = fread(destination, 1, MAX_FILESIZE, ftmp);

                if (!bytesRead) {
                    vlog(LogConfig, "Failure reading from %s", lfname);
                    vm_exit(1);
                }
                fclose(ftmp);

#ifdef VOMIT_DETECT_UNINITIALIZED_ACCESS
                for (DWORD address = lseg * 16 + loff; address < lseg * 16 + loff + bytesRead; ++address)
                    g_cpu->markDirty(address);
#endif
            } else if (!reloading && !strcmp(curtok, "setbyte")) {
                lseg = strtol(strtok(NULL,": \t\n"), NULL, 16);
                loff = strtol(strtok(NULL," \t\n"), NULL, 16);
                curtok = strtok(NULL, " \t\n");

                //vlog(LogInit, "Entering at %04X:%04X", lseg, loff);

                while (curtok) {
                    BYTE tb = strtol(curtok,NULL,16);
                    //vlog(LogInit, "  [%04X] = %02X", loff, tb);
                    g_cpu->writeMemory16(lseg, loff++, tb);
                    curtok = strtok(NULL, " \t\n");
                }
                curtok=curline;
            } else if (!strcmp(curtok, "floppy")) {
                char type[32];

                ldrv = strtol(strtok(0L, " \t\n"), 0L, 10);
                strcpy(type, strtok(0L, " \t\n"));
                strcpy(lfname, strtok(0L, "\n"));

                disk_type_t* dt;
                for (dt = floppy_types; dt->name; ++dt) {
                    if (!strcmp(type, dt->name))
                        break;
                }

                if (!dt->name) {
                    vlog(LogInit, "Invalid floppy type: \"%s\"", type);
                    continue;
                }

                strcpy(drv_imgfile[ldrv], lfname);
                drv_spt[ldrv] = dt->sectors_per_track;
                drv_heads[ldrv] = dt->heads;
                drv_sectors[ldrv] = dt->sectors;
                drv_sectsize[ldrv] = dt->bytes_per_sector;
                drv_type[ldrv] = dt->media_type;
                drv_status[ldrv] = 1;
                /* TODO: What should the media type be? */
                drv_type[ldrv] = 0;

                vlog(LogInit, "Floppy %d: %s (%dspt, %dh, %ds (%db))", ldrv, lfname, dt->sectors_per_track, dt->heads, dt->sectors, dt->bytes_per_sector);
            }
            else if (strcmp(curtok,"drive")==0) {    /* segfaults ahead! */
                ldrv = strtol(strtok(NULL, " \t\n"), NULL, 16);
                strcpy(lfname, strtok(NULL, " \t\n"));
                lspt = strtol(strtok(NULL, " \t\n"), NULL, 16);
                lhds = strtol(strtok(NULL, " \t\n"), NULL, 16);
                lsect = strtol(strtok(NULL, " \t\n"), NULL, 16);
                lsectsize = strtol(strtok(NULL, " \t\n"), NULL, 16);

                vlog(LogInit, "Drive %d: %s (%dspt, %dh, %ds (%db))", ldrv, lfname, lspt, lhds, lsect, lsectsize);

                ldrv += 2;

                strcpy(drv_imgfile[ldrv], lfname);
                drv_spt[ldrv] = lspt;
                drv_heads[ldrv] = lhds;
                drv_sectors[ldrv] = lsect;
                drv_sectsize[ldrv] = lsectsize;
                drv_status[ldrv] = 1;
            } else if (!strcmp(curtok, "fixed")) {
                ldrv = strtol(strtok(NULL, " \t\n"), NULL, 16);
                strcpy(lfname, strtok(NULL, " \t\n"));

                long megabytes = strtol(strtok(NULL, " \t\n"), NULL, 10);

                vlog(LogInit, "Fixed drive %d: %s (%ld MB)", ldrv, lfname, megabytes);

                ldrv += 2;

                strcpy(drv_imgfile[ldrv], lfname);
                drv_spt[ldrv] = 63;
                drv_heads[ldrv] = 16;
                drv_sectsize[ldrv] = 512;
                drv_status[ldrv] = 1;
                drv_sectors[ldrv] = (megabytes * 1048576) / drv_sectsize[ldrv];
            } else if (!reloading && !strcmp(curtok, "memory")) {
                curtok = strtok(NULL, " \t\n");
                g_cpu->m_baseMemorySize = strtol(curtok, NULL, 10) * 1024;
                vlog(LogInit, "Memory size: %d kilobytes", g_cpu->baseMemorySize() / 1024);
            } else if (!reloading && !strcmp(curtok, "ememory")) {
                curtok = strtok(NULL, " \t\n");
                g_cpu->m_extendedMemorySize = strtol(curtok, NULL, 10) * 1024;
                vlog(LogInit, "Extended memory size: %d kilobytes", g_cpu->extendedMemorySize() / 1024);
            } else if (!reloading && !strcmp(curtok, "entry")) {
                WORD entry_cs, entry_ip;
                curtok = strtok(NULL, ": \t\n");
                entry_cs = (WORD)strtol(curtok, NULL, 16);
                curtok = strtok(NULL, " \t\n");
                entry_ip = (WORD)strtol(curtok, NULL, 16);
                g_cpu->jump16(entry_cs, entry_ip);
            } else if (!reloading && !strcmp(curtok, "addint")) {
                BYTE isr = strtol(strtok(NULL, " \t\n"), NULL, 16);
                WORD segment = strtol(strtok(NULL, ": \t\n"), NULL, 16);
                WORD offset = strtol(strtok(NULL, " \t\n"), NULL, 16);
                vlog(LogInit, "Software interrupt %02X at %04X:%04X", isr, segment, offset);
                g_cpu->setInterruptHandler(isr, segment, offset);
            } else if (!reloading && !strcmp(curtok, "io_ignore")) {
                curtok = strtok(NULL, " \t\n");
                WORD port = strtol(curtok, NULL, 16);
                vlog(LogInit, "Ignoring I/O port 0x%04X", port);
                vomit_ignore_io_port(port);
            }
        }
    }
    fclose(fconf);
}

void
vm_loadconf()
{
    reloading = false;
    unspeakable_abomination();
}

void
config_reload()
{
    reloading = true;
    unspeakable_abomination();
}
