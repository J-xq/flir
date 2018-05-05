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
    int fd, i, ret;
    unsigned char snapshot = 0;
    int bb;

    if (argc > 1) {
        bb = atoi(argv[1]);
    } else {
        bb = 2;
    }
    fd = serial_init(bb);

    baud_rate(fd, 0x0007);
    close(fd);
    fd = serial_init(5);

    camera_part(fd);
    camera_serial_no(fd);

    digital_output_mode(fd, 0x0303);  /* 8bit bmp */
    
    transfer_frame(fd);

	read_picture(fd, 15);

    close(fd);
    return 0;
}
