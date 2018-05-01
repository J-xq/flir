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
    int fd, i, cnt;
    unsigned char snapshot = 2;
    int bb;

    if (argc > 1) {
        bb = atoi(argv[1]);
    } else {
        bb = 2;
    }
    fd = serial_init(bb);

    camera_reset(fd);

    factory_defaults(fd);
    camera_part(fd);
    camera_serial_no(fd);
    return 0;
    erase_flash(fd, snapshot);
    sleep(1);

    digital_output_mode(fd, 0x0303);  /* 8bit bmp */

    transfer_frame(fd, snapshot);
    sleep(1);

	read_picture(fd, snapshot);

    close(fd);
    return 0;
}
