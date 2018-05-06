/*
 * ============================================================================
 *
 *       Filename:  read_picture.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/05/2018 04:12:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serial_flir.h"

int read_picture(int fd, char n, unsigned char *buf)
{
    unsigned char cmd[512];
    int cnt, i;
    int picptr = 0;
	unsigned int addr;
	int size, savesize;
    int offset, width, height;
    int pic;

	//int x, y;
	//for(y = 0; y < 512; y++) {
	//	for(x = 0; x < 640; x++) {
	//		buf[1078 + y*640+x] = rand();
	//	}
	//}
	//return 0;

	cmd[0] = 0; cmd[1] = n; cmd[2] = 0; cmd[3] = 0x13; // read snapshot n
	cnt = send_command(fd, GET_MEMORY_ADDRESS, cmd, 4);

    addr = (cmd[8] << 24) | (cmd[9] << 16) | (cmd[10] << 8) | cmd[11];
    size = (cmd[12] << 24) | (cmd[13] << 16) | (cmd[14] << 8) | cmd[15];
    
    savesize = size;
    printf("snapshot addr: %08X %08X\n", addr, size);
    picptr = 0;

    while(size > 0) {
        read_memory(fd, cmd, addr, size);

        if (size > 255)
            memcpy(&buf[picptr], &cmd[8], 256);
        else
            memcpy(&buf[picptr], &cmd[8], size);

        size -= 256;
        addr += 256;
        picptr += 256;
    }

    offset = *(int *)(&buf[10]);
    width  = *(int *)(&buf[18]);
    height = *(int *)(&buf[22]);

    printf("%d %d %d\n", offset, width, height);

    //pic = open("./picture", O_CREAT|O_RDWR, 0644);
    //write(pic, buf, savesize);
    //close(pic);

    return 0;
}
