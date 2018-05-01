/*
 * ============================================================================
 *
 *       Filename:  utility.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2018 02:26:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include <stdio.h>
#include "serial_flir.h"

int baud_rate(int fd, int mode)
{
    unsigned char cmd[128];
    if (mode < 0) {
        send_command(fd, BAUD_RATE, cmd, 0);
    } else {
        cmd[0] = mode >> 8;
        cmd[1] = mode;
        send_command(fd, BAUD_RATE, cmd, 0);
    }

    printf("baud rate: %02X%02X\n", cmd[8], cmd[9]);
    return 0;
}
int shutter_position(int fd, char mode)
{
    unsigned char cmd[128];

    cmd[0] = 0; cmd[1] = mode;  /*  0000: open shutter */
    send_command(fd, SHUTTER_POSITION, cmd, 2);

    return 0;
}

int camera_part(int fd)
{
    unsigned char cmd[128];
    int i, cnt;

    cnt = send_command(fd, CAMERA_PART, cmd, 0);
    printf("CAM_PART: ");
    for(i = 0; i < cnt; i++)
        printf("%02X ", cmd[i]);
    printf("\n");
    return 0;
}

int camera_serial_no(int fd)
{
    unsigned char cmd[128];
    int i, cnt;

    cnt = send_command(fd, SERIAL_NUMBER, cmd, 0);
    printf("SERIAL No.: ");
    for(i = 0; i < cnt; i++) {
        printf("%02X ", cmd[i]);
    }
    printf("\n");
    return 0;
}

int digital_output_mode(int fd, int mode)
{
    unsigned char cmd[128];

    cmd[0] = mode >> 8;
    cmd[1] = mode;
    send_command(fd, DIGITAL_OUTPUT_MODE, cmd, 2);
    return 0;
}

int transfer_frame(int fd, int snapshot)
{
    unsigned char cmd[128];
    
    cmd[0] = 0x16; cmd[1] = snapshot; cmd[2] = 0x00; cmd[3] = 0x01;
    send_command(fd, TRANSFER_FRAME, cmd, 4);

    return 0;
}

int set_defaults(int fd)
{
    unsigned char cmd[128];
    
    send_command(fd, SET_DEFAULTS, cmd, 0);
    return 0;
}

int camera_reset(int fd)
{
    unsigned char cmd[128];
    int i, cnt;
    
    cnt = send_command(fd, CAMERA_RESET, cmd, 0);
	printf("Camera RESET: ");
    for(i = 0; i < cnt; i++)
        printf("%02X ", cmd[i]);
	printf("\n");
    return 0;
}

int factory_defaults(int fd)
{
    unsigned char cmd[128];
    int i, cnt;

    cnt = send_command(fd, FACTORY_DEFAULTS, cmd, 0);
    for(i = 0; i < cnt; i++)
        printf("%02X ", cmd[i]);
    printf("\n");
    return 0;
}

