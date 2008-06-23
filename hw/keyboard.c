/*
 * Basic AT keyboard controller
 * for VOMIT :^)
 */

#include "vomit.h"
#include "debug.h"

#define ATKBD_PARITY_ERROR  0x80
#define ATKBD_TIMEOUT       0x40
#define ATKBD_BUFFER_FULL   0x20
#define ATKBD_UNLOCKED      0x10
#define ATKBD_CMD_DATA      0x08
#define ATKBD_SYSTEM_FLAG   0x04
#define ATKBD_INPUT_STATUS  0x02
#define ATKBD_OUTPUT_STATUS 0x01

static byte keyboard_status( word );
static byte system_control_read( word );
static void system_control_write( word, byte );

static byte system_control_port_data;

void
keyboard_init()
{
	vm_listen( 0x61, system_control_read, system_control_write );
	vm_listen( 0x64, keyboard_status, 0L );

	system_control_port_data = 0;
}

byte
keyboard_status( word port )
{
	/* Keyboard not locked, POST completed successfully. */
	return ATKBD_UNLOCKED | ATKBD_SYSTEM_FLAG;
}

byte
system_control_read( word port )
{
	vlog( VM_KEYMSG, "%02X <- System control port", system_control_port_data );
	return system_control_port_data;
}

void
system_control_write( word port, byte data )
{
	system_control_port_data = data;
	vlog( VM_KEYMSG, "System control port <- %02X", data );
}
