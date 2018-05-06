/*
 * ============================================================================
 *
 *       Filename:  test.c
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
#include <stdlib.h>
#include <unistd.h>
#include "serial_flir.h"

int main(int argc, char *argv[])
{
    int fd, i, k, ret;
    unsigned char snapshot = 0;
    int bb;
    unsigned char cmd[512];
    unsigned int addr;

    if (argc > 1) {
        bb = atoi(argv[1]);
    } else {
        bb = 2;
    }
    fd = serial_init(bb);
    if (fd < 0) {
        return -1;
    }
    baud_rate(fd, 0x0007);
    close(fd);
    fd = serial_init(5);

    camera_part(fd);
    camera_serial_no(fd);

    //digital_output_mode(fd, 0x0303);  /* 8bit bmp */

    fb_init("/dev/fb0");

    task_init(120);  // 120 seconds
    //transfer_frame(fd);
    while (1) {
        ;
    }

    close(fd);
    return 0;
}
