/*
 * ============================================================================
 *
 *       Filename:  erase_flash.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2018 09:15:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "serial_flir.h"

/* base address and block size are get by GET_NV_MEMORY_SIZE command */
#define  BASEADDR   0x01000000
#define  BLKSIZE    (1<<17)

/* erase snapshot n. erase all when 0=0 */
int erase_flash(int fd, char n)
{
    unsigned char cmd[512];
    int cnt, i;
    int status = -1;
    unsigned int addr, blkaddr;
    int size, blkcnt;

    memcpy(cmd, "\x00\x00\x00\x13", 4);
    if (n >= 0) {
        cmd[0] = 0;
        cmd[1] = n;
    } else {
        return -1;
    }
    /* 
     * FFFF0013: read base address and total size( 02960000 00660000 )
     * FFFE0013: return number of used bytes and total number of snapshots
     * 00XX0013: return address and size of snapshots XX
     * 80XX0013: return 4-byte header ("BMP8" or "SNAP")
     */
    cnt = send_command(fd, GET_MEMORY_ADDRESS, cmd, 4);
    
    addr = (cmd[8] << 24) | (cmd[9] << 16) | (cmd[10] << 8) | cmd[11];
    size = (cmd[12] << 24) | (cmd[13] << 16) | (cmd[14] << 8) | cmd[15];

    printf("block info: %08X  %08X\n", addr, size);
    blkaddr = (addr-BASEADDR) >> 17;
    blkcnt = size >> 17;

    while(blkcnt-- >= 0) {
        cmd[0] = blkaddr >> 8;
        cmd[1] = blkaddr;
        send_command(fd, ERASE_MEMORY_BLOCK, cmd, 2);
        usleep(600000);
	    send_command(fd, MEMORY_STATUS, cmd, 0);
	    status = (cmd[8] << 8) | cmd[9];
        printf("BLOCK addr: %04X, status:%04X\n", blkaddr, status);
        blkaddr++;
        usleep(2000);
    }

    return status;
}

/* erase specified range */
int erase_range(int fd, unsigned start, int blkcnt)
{
    unsigned char cmd[512];
    int cnt, i;
    int status = -1;
    unsigned int blkaddr;

    printf("Erase from : %08X,  %08X blocks\n", start, blkcnt);
    blkaddr = (start-BASEADDR) >> 17;

    if ((start < 0x2A00000) || (blkcnt > 0x32)) {
        return -1;
    }
    while(blkcnt-- >= 0) {
        cmd[0] = blkaddr >> 8;
        cmd[1] = blkaddr;
        send_command(fd, ERASE_MEMORY_BLOCK, cmd, 2);
        usleep(600000);
	    send_command(fd, MEMORY_STATUS, cmd, 0);
	    status = (cmd[8] << 8) | cmd[9];
        printf("BLOCK addr: %04X, status:%04X\n", blkaddr, status);
        blkaddr++;
    }

    return status;
}

