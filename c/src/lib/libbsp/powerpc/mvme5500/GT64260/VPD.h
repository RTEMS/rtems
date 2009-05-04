/* The mapping of the Configuration VPD
 *
 * (C) 2004, NSLS, Brookhaven National Laboratory,
 *           S. Kate Feng, <feng1@bnl.gov>
 *  under the Deaprtment of Energy contract DE-AC02-98CH10886
 */

extern unsigned char ConfVPD_buff[200];

typedef struct ConfVpdRec {
  char VendorId[8];
  char pad0[4];
  char BrdId[13];
  char pad1[2];
  char ManAssmNum[12];
  char pad2[2];
  char SerialNum[7];
  char pad3[2];
  /*  char IntClk[4];*/
  uint32_t IntClk;
  char pad4[3];
  /*char ExtClk[4];*/
  uint32_t ExtClk;
  char pad5[3];
  char EnetAddr0[7];
  char pad6[2];
  char EnetAddr1[7];
  char pad7[20];
} ConfVpdRec_t;

#define VPD_BOARD_ID   8
#define VPD_ENET0_OFFSET  0x40
#define VPD_ENET1_OFFSET  0x49

/*
4D4F544F 524F4C41 0200010D 4D564D45  MOTOROLA....MVME
35353030 2D303136 33020C30 312D5733  5500-0163..01-W3
38323946 30314403 07373035 31383238  829F01D..7051828
05053B9A CA000106 0507F281 55010807  ..;.........U...
ethernet address
xxxxxxxx xxxxxxxx xxxxxxxx xxxx3701  ................
09043734 35350A04 87A6E98C 0B0C0089  ..7455..."=.....
00181002 02101000 78070B0C FFFFFFFF  ........x.......
10020210 10017805 0E0FFFFF FFFFFFFF  ......x.........
FFFFFF01 FF01FFFF FF0F0400 03000019  ................
0A010107 02030000 000100FF FFFFFFFF  ................
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF  ................
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF  ................
00000000 00000000 00000000 00000000  ................
00000000 00000000 00000000 00000000  ................
00000000 00000000 00000000 00000000  ................
00000000 00000000 00000000 00000000  ................

For the MVME5500 :

Product Identifier                 : MVME5500-0163
Manufacturing Assembly Number      : 01-W3829F01D
Serial Number                      : 7051828
Internal Clock Speed (Hertz)       : 3B9ACA00 (&1000000000)
External Clock Speed (Hertz)       : 07F28155 (&133333333)
Ethernet Address                   : xx xx xx xx xx xx xx
Ethernet Address                   : xx xx xx xx xx xx xx
Microprocessor Type                : 7455
SROM/EEPROM CRC                    : D3223DD0 (&-752730672)
Flash0 Memory Configuration        : 00 89 00 18 10 02 02 10
                                   : 10 00 78 07
Flash1 Memory Configuration        : FF FF FF FF 10 02 02 10
                                   : 10 01 78 05
L2 Cache Configuration             : FF FF FF FF FF FF FF FF
                                   : FF 01 FF 01 FF FF FF
VPD Revision                       : 00 03 00 00
L3 Cache Configuration             : 01 01 07 02 03 00 00 00
                                   : 01 00
                                                                               
*/
