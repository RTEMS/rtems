/* MotLoad VPD format */

/*
VPD = "MOTOROLA" , { field }

field = type, length, { data }
*/

/*
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <unistd.h>
#include <rtems.h>
#include <string.h>
#include <sys/fcntl.h>
#include <bsp.h>

#include "vpd.h"

/* FIXME XXX: all of the early i2c reading stuff should
 * be moved elsewhere. This file should only deal with
 * VPD.
 */
#include <rtems/libi2c.h>
#define dev			BSP_I2C_BUS_DESCRIPTOR
extern rtems_libi2c_bus_t *dev;

#define init		dev->ops->init
#define start		dev->ops->send_start
#define addr		dev->ops->send_addr
#define write_bytes	dev->ops->write_bytes

#define I2C_READ		1
#define I2C_WRITE		0

static ssize_t	(*read_bytes)(int fd, void *buf, size_t len) = 0;

static ssize_t early_fp = 0;

static ssize_t early_read(int fd, void *buf, size_t len)
{
ssize_t rval;
rtems_libi2c_bus_t *d = (rtems_libi2c_bus_t*)fd;
uint8_t            fp[2];

	/* Always send the file-pointer. Some i2c controllers
	 * (mpc8540) have 'pipelined' operation and do extra
	 * read cycles so that a sequence of
	 * 'read_bytes', 'read_bytes' does NOT return the true
	 * sequence of bytes :-(
	 */
	start(d);
	addr(d, BSP_VPD_I2C_ADDR, I2C_WRITE);
	fp[0] = early_fp>>8;
	fp[1] = early_fp;
	write_bytes(dev,fp,2);

	start(d);
	addr(d, BSP_VPD_I2C_ADDR, I2C_READ);
	rval = d->ops->read_bytes(d, buf, len);
	if ( rval > 0 )
		early_fp += rval;
	return rval;
}

static int early_close(int fd)
{
rtems_libi2c_bus_t *d = (rtems_libi2c_bus_t*)fd;
	return d->ops->send_stop(d);
}

static int read_buf(int fd, void *buf, size_t len)
{
int got, rval = 0;
	while ( len > 0 ) {
		if ( (got = read_bytes(fd, buf+rval, len)) <= 0 )
			return -1;
		len -= got;
		rval+= got;
	}
	return rval;
}

/* Not Efficient but simple */
int
BSP_vpdRetrieveFields(VpdBuf data)
{
VpdBuf	      b, b1;
VpdKey	      k;
int		      l,fd = -1, put, got;
int           rval = -1;
unsigned char mot[9];
static int	  (*stop)(int fd);

	memset(mot,0,sizeof(mot));

	if ( 0 && _System_state_Is_up(_System_state_Get()) ) {
		read_bytes = read;
		stop       = close;
		fd         = open(BSP_I2C_VPD_EEPROM_DEV_NAME, 0);
		if ( fd < 0 )
			return -1;
	} else {
		fd = (int)dev;
/*
		init(dev);
 *
 *	 Hangs - probably would need a delay here - just leave motload settings
 */

		read_bytes = early_read;
		stop       = early_close;
	}

	if ( read_bytes(fd, mot, 8) < 8 ) {
		goto bail;
	}

	if ( strcmp((char*)mot,"MOTOROLA") )
		goto bail;

	l = 0;
	do {

		/* skip field -- this is not done the first time since l=0 */
		while ( l > sizeof(mot) ) {
			got = read_buf(fd, mot, sizeof(mot));
			if ( got < 1 )
				goto bail;
			l -= got;
		}
		if ( read_buf(fd, mot, l) < 0 )
			goto bail;

		/* now get a new header */
		if ( read_buf(fd, mot, 2) < 2 )
			goto bail;

		k = mot[0];
		l = mot[1];

		for ( b = data; b->key != End; b++ ) {
			if ( b->key == k && (signed char)b->instance >= 0 ) {
				if ( 0 == b->instance--  ) {
					/* found 'instance' of field 'type' */

					/* limit to buffer size */
					put = b->buflen > l ? l : b->buflen;
					if ( read_buf(fd, b->buf, put) < put )
						goto bail;

					/* if this instance is multiply requested, copy the data */
					for ( b1 = b + 1; b1->key != End; b1++ ) {
						if ( b1->key == k && 0 == b1->instance ) {
							b1->instance--;
							/* we dont' handle the case where
							 * the first buffer couldn't hold the entire
							 * item but this one could...
							 */
							memcpy(b1->buf, b->buf, put);
							b1->found = mot[1];
						}
					}

					l -= put;
					b->found = mot[1];
				}
			}
		}

	} while ( k != End );

	rval = 0;

bail:

	stop(fd);
	return rval;
}

int
BSP_vpdRetrieveKey(VpdKey k, void *buf, int len, int inst)
{
	VpdBufRec	d[] = {
		{ key: k, buf: buf, buflen: len, instance: inst },
		VPD_END
	};
	return  BSP_vpdRetrieveFields(d) ? -1 : d[0].found;
}

/*
005C3000  4D4F544F 524F4C41 0200010D 4D564D45  MOTOROLA....MVME
005C3010  35353030 2D303136 31020C30 312D5733  5500-0161..01-W3
005C3020  38323946 30324803 07373237 39303630  829F02H..7279060
005C3030  05053B9A CA000106 0507F281 55010807  ..;.........U...
005C3040  0001AF12 CA780008 070001AF 12CA7901  .....x........y.
005C3050  09043734 35350A04 D3223DD0 0B0C0089  ..7455..."=.....
005C3060  00181002 02101000 78070B0C FFFFFFFF  ........x.......
005C3070  10020210 10017805 0E0FFFFF FFFFFFFF  ......x.........
005C3080  FFFFFF01 FF01FFFF FF0F0400 03000019  ................
005C3090  0A010107 02030000 000100FF FFFFFFFF  ................
005C30A0  FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF  ................
005C30B0  FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF  ................
005C30C0  00000000 00000000 00000000 00000000  ................
005C30D0  00000000 00000000 00000000 00000000  ................
005C30E0  00000000 00000000 00000000 00000000  ................
005C30F0  00000000 00000000 00000000 00000000  ................

Product Identifier                 : MVME5500-0161
Manufacturing Assembly Number      : 01-W3829F02H
Serial Number                      : 7279060
Internal Clock Speed (Hertz)       : 3B9ACA00 (&1000000000)
External Clock Speed (Hertz)       : 07F28155 (&133333333)
Ethernet Address                   : 00 01 AF 12 CA 78 00
Ethernet Address                   : 00 01 AF 12 CA 79 01
Microprocessor Type                : 7455
SROM/EEPROM CRC                    : D3223DD0 (&-752730672)
Flash Memory Configuration         : 00 89 00 18 10 02 02 10
                                   : 10 00 78 07
Flash Memory Configuration         : FF FF FF FF 10 02 02 10
                                   : 10 01 78 05
L2 Cache Configuration             : FF FF FF FF FF FF FF FF
                                   : FF 01 FF 01 FF FF FF
VPD Revision                       : 00 03 00 00
L3 Cache Configuration             : 01 01 07 02 03 00 00 00
                                   : 01 00




0x01889128:  4d4f544f  524f4c41  02000f04  00030000  MOTOROLA........
0x01889138:  060507f2  81550101  0d4d564d  45363130  .....U...MVME610
0x01889148:  302d3031  3631020c  30312d57  33383738  0-0161..01-W3878
0x01889158:  46303144  03073732  33313137  300e0fff  F01D..7231170...
0x01889168:  ffffffff  ffffffff  01ff01ff  ffff190a  ................
0x01889178:  01000002  03000000  01000904  37343537  ............7457
0x01889188:  0a047277  144d0807  0001af13  b53c0008  ..rw.M.......<..
0x01889198:  070001af  13b53d01  0b0c0089  00031002  ......=.........
0x018891a8:  02101000  78080b0c  00890003  10020210  ....x...........
0x018891b8:  10017808  ffffffff  ffffffff  ffffffff  ..x.............
0x018891c8:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x018891d8:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x018891e8:  ffffffff  ffffffff                      ........
0x00000000 (0)

0x003895c0:  4d4f544f  524f4c41  02000f04  00030000  MOTOROLA........
0x003895d0:  060503f9  40aa0101  0d4d564d  45333130  ....@....MVME310
0x003895e0:  302d3132  3633020c  30312d57  33383933  0-1263..01-W3893
0x003895f0:  46303143  03073734  35383831  340e0fff  F01C..7458814...
0x00389600:  ffffffff  ffffffff  01ff01ff  ffff0904  ................
0x00389610:  38353430  0a047129  d2d60807  0001af16  8540..q)........
0x00389620:  e5c50008  070001af  16e5c601  08070001  ................
0x00389630:  af16e5c7  020b0c00  017e2310  02021010  .........~#.....
0x00389640:  006409ff  ffffffff  ffffffff  ffffffff  .d..............
0x00389650:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x00389660:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x00389670:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x00389680:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x00389690:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x003896a0:  ffffffff  ffffffff  ffffffff  ffffffff  ................
0x003896b0:  ffffffff  ffffffff  ffffffff  ffffffff  ................

MVME3100> vpdDisplay
VPD Revision                       : 00 03 00 00
External Clock Speed (Hertz)       : 03F940AA (&66666666)
Product Identifier                 : MVME3100-1263
Manufacturing Assembly Number      : 01-W3893F01C
Serial Number                      : 7458814
L2 Cache Configuration             : FF FF FF FF FF FF FF FF
                                   : FF 01 FF 01 FF FF FF
Microprocessor Type                : 8540
SROM/EEPROM CRC                    : 7129D2D6 (&1898566358)
Ethernet Address                   : 00 01 AF 16 E5 C5 00
Ethernet Address                   : 00 01 AF 16 E5 C6 01
Ethernet Address                   : 00 01 AF 16 E5 C7 02
Flash Memory Configuration         : 00 01 7E 23 10 02 02 10
								   : 10 00 64 09

*/
