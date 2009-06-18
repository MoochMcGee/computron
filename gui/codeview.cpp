#include "codeview.h"
#include <QPainter>
#include <QTimer>

extern "C" {
	#include "../include/vomit.h"
	#include "../include/8086.h"
	#include "../disasm/include/disasm.h"
}

struct CodeView::Private
{
	uint16_t segment;
	uint16_t offset;

	QTimer syncTimer;
};

CodeView::CodeView( QWidget *parent )
	: QWidget( parent ),
	  d( new Private )
{
	d->segment = 0;
	d->offset = 0;

	connect( &d->syncTimer, SIGNAL(timeout()), SLOT(update()) );

	d->syncTimer.start( 500 );
}

CodeView::~CodeView()
{
	delete d;
	d = 0L;
}

void
CodeView::setAddress( uint16_t segment, uint16_t offset )
{
	d->segment = segment;
	d->offset = offset;
	update();
}

static int
dump_disasm( QString &output, unsigned int segment, unsigned int offset )
{
	char disasm[64];
	int width, i;
	char buf[512];
	char *p = buf;
	byte *opcode;

	opcode = mem_space + (segment << 4) + offset;
	width = insn_width( opcode );
	disassemble( opcode, offset, disasm, sizeof(disasm) );

	//p += sprintf( p, "%04X:%04X ", segment, offset );

	for( i = 0; i < (width ? width : 7); ++i )
	{
		p += sprintf( p, "%02X", opcode[i] );
	}
	for( i = 0; i < (14-((width?width:7)*2)); ++i )
	{
		p += sprintf( p, " " );
	}

	p += sprintf( p, " %s", disasm );

	output += buf;

	/* Recurse if this is a prefix instruction. */
	//if( *opcode == 0x26 || *opcode == 0x2E || *opcode == 0x36 || *opcode == 0x3E || *opcode == 0xF2 || *opcode == 0xF3 )
	//	width += dump_disasm( output, segment, offset + width );

	return width;
}

void
CodeView::paintEvent( QPaintEvent * )
{
	QPainter p( this );

	d->segment = cpu.base_CS;
	d->offset = cpu.base_IP;

	p.fillRect( rect(), Qt::white );

	word insn_ptr = 0;

	QFontMetrics fm(p.font());
	int textY = fm.height();

	p.setFont( QFont( "DEC Terminal" ));

	for( int y = 0; y < 14; ++y )
	{
		QString line;

		line.sprintf( "%04X:%04X  ", d->segment, d->offset + insn_ptr );
		insn_ptr += dump_disasm( line, d->segment, d->offset + insn_ptr );

		p.drawText( 0, textY, line );

		textY += fm.height();
	}
}

QSize
CodeView::sizeHint() const
{
	return QSize( 600, 270 );
}