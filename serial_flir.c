/*
 * ============================================================================
 *
 *       Filename:  serial_flir.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/04/2018 10:24:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include "crc.h"
#include "serial_flir.h"

int main(int argc, char *argv[])
{
    int fd, i, cnt;
    unsigned short crc;
    unsigned char cmd[512], val[512];
    unsigned int addr;
    int size;
    unsigned char snapshot = 2;

    fd = serial_init();

    cmd[0] = 0x03; cmd[1] = 0x03;
    send_command(fd, DIGITAL_OUTPUT_MODE, cmd, 2);
    /* 
    cmd[0] = 0xff; cmd[1] = 0xff;
    cnt = send_command(fd, GET_NV_MEMORY_SIZE, cmd, 2);
    printf("GET NV MEM SIZE:");
    for (i = 0; i < cnt; i++) {
        printf("%02X ", cmd[i]);
    }
    printf("\n");
    */
    //erase_flash(fd, snapshot);

    cmd[0] = 0; cmd[1] = 0;  /* 0000: open shutter */
    send_command(fd, SHUTTER_POSITION, cmd, 2);
    sleep(1);

    cmd[0] = 0x16; cmd[1] = snapshot; cmd[2] = 0x00; cmd[3] = 0x01;
    send_command(fd, TRANSFER_FRAME, cmd, 4);

    cmd[0] = 0; cmd[1] = 1;  /* 0001: close shutter */
    send_command(fd, SHUTTER_POSITION, cmd, 2);
    usleep(100000);

    read_picture(fd, snapshot);

    printf("\n");

    close(fd);
}
