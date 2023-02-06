#ifndef __SPI_SD_H_
#define __SPI_SD_H_

#define SD_VER_ERR     0X00
#define SD_VER_MMC     0X01
#define SD_VER_V1      0X02
#define SD_VER_V2      0X03
#define SD_VER_V2HC    0X04

#define INIT_ERROR                  (-0x01) //初始化错误
#define INIT_CMD0_ERROR             (-0x02) //CMD0错误
#define INIT_CMD1_ERROR             (-0x03) //CMD1错误
#define INIT_SDV2_ACMD41_ERROR	    (-0x04) //ACMD41错误
#define INIT_SDV1_ACMD41_ERROR	    (-0x05) //ACMD41错误

#define WRITE_CMD24_ERROR           (-0x06) //写块时产生CMD24错误
#define WRITE_BLOCK_ERROR           (-0x07) //写块错误

#define READ_BLOCK_ERROR            (-0x08) //读块错误

#define WRITE_CMD25_ERROR           (-0x09) //在连续多块写时产生CMD25错误
#define WRITE_NBLOCK_ERROR          (-0x0A) //连续多块写失败

#define READ_CMD18_ERROR            (-0x0B) //在连续多块读时产生CMD18错误
 
#define GET_CSD_ERROR               (-0x0C) //读CSD失败

#define TRY_TIME 200   //向SD卡写入命令之后，读取SD卡的回应次数，即读TRY_TIME次，如果在TRY_TIME次中读不到回应，产生超时错误，命令写入失败


int sd_init(void); //SD卡初始化

int sd_write_sector(unsigned int addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入地址为addr的扇区中
int sd_read_sector(unsigned int addr,unsigned char *buffer);	 //从地址为addr的扇区中读取数据到buffer数据缓冲区中
int sd_write_nsector(unsigned int nsec,unsigned int addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入起始地址为addr的nsec个连续扇区中
int sd_read_nsector(unsigned int nsec,unsigned int addr,unsigned char *buffer); //将buffer数据缓冲区中的数据写入起始地址为addr的nsec个连续扇区中
int sd_erase_nsector(unsigned int addr_sta,unsigned int addr_end);
int sd_get_total_sectors_num(void); //获取SD卡的总扇区数

#endif