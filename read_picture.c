/*
 * ============================================================================
 *
 *       Filename:  read_picture.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2018 01:25:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "serial_flir.h"

int read_picture(int fd, char n)
{
    unsigned char cmd[512];
    int cnt, i;
    int pic = 0;
	unsigned int addr;
	int size;

	cmd[0] = 0; cmd[1] = n; cmd[2] = 0; cmd[3] = 0x13; // read snapshot n
	cnt = send_command(fd, GET_MEMORY_ADDRESS, cmd, 4);

    addr = (cmd[8] << 24) | (cmd[9] << 16) | (cmd[10] << 8) | cmd[11];
    size = (cmd[12] << 24) | (cmd[13] << 16) | (cmd[14] << 8) | cmd[15];

    printf("read snapshot: %08X %08X\n", addr, size);

    pic = open("./picture", O_CREAT|O_RDWR, 0644);
    while(size > 0) {
        read_memory(fd, cmd, addr, size);

        if (size > 255)
            write(pic, &cmd[8], 256);
        else
            write(pic, &cmd[8], size);

        size -= 256;
        addr += 256;
    }
    close(pic);
    return 0;
}
