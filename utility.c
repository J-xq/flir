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
#include <unistd.h>
#include "serial_flir.h"

/* read memory state:
 * FFFF: return total address
 * FFFE: return used bytes and total number of snapshots in flash
 * 00XX: return address and size(in bytes) of snapshot XX
 * 80XX: return 4bytes head ('BMP8' or 'SNAP') of snapshot XX
 */
int get_memory_address(int fd, int state)
{
    unsigned char cmd[128];
    unsigned int addr;
    int size;
    int i;

	cmd[0] = state >> 8;
    cmd[1] = state;
    cmd[2] = 0; cmd[3] = 0x13;
	send_command(fd, GET_MEMORY_ADDRESS, cmd, 4);

    addr = (cmd[8] << 24) | (cmd[9] << 16) | (cmd[10] << 8) | cmd[11];
    size = (cmd[12] << 24) | (cmd[13] << 16) | (cmd[14] << 8) | cmd[15];

    printf("read snapshot: %08X %08X\n", addr, size);

    return 0;
}

int get_memory_scale(int fd)
{
    unsigned char cmd[128];
    int i;
    cmd[0] = 0xff; cmd[1] = 0xff;
    send_command(fd, GET_NV_MEMORY_SIZE, cmd, 2);
    for(i = 0; i < 8; i++) {
        printf("%02X ", cmd[i + 8]);
    }
    printf("\n");
    return 0;
}

int baud_rate(int fd, int mode)
{
    unsigned char cmd[128];
    if (mode < 0) {   /* read baud rate */
        send_command(fd, BAUD_RATE, cmd, 0);
    } else {         /* set baud rate */
        cmd[0] = mode >> 8;
        cmd[1] = mode;
        send_command(fd, BAUD_RATE, cmd, 2);
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
    for(i = 0; i < 32; i++)
        printf("%c", cmd[i+8]);
    printf("\n");
    return 0;
}

int camera_serial_no(int fd)
{
    unsigned char cmd[128];
    int i, cnt;

    cnt = send_command(fd, SERIAL_NUMBER, cmd, 0);
    printf("SERIAL No.: ");
    for(i = 0; i < 8; i++) {
        printf("%02X ", cmd[i+8]);
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

/* Transfer a 8-bit BMP snapshot into the next available slot in the flash */
int transfer_frame(int fd)
{
    unsigned char cmd[128];
    
    cmd[0] = 0x16; cmd[1] = 0; cmd[2] = 0x00; cmd[3] = 0x01;
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

int read_memory(int fd, unsigned char *cmd, unsigned int addr, int size)
{
    int cnt;
    cmd[0] = addr >> 24;
    cmd[1] = addr >> 16;
    cmd[2] = addr >> 8;
    cmd[3] = addr;

    if (size > 255) {
        cmd[4] = 1;
        cmd[5] = 0;     /* read 256 bytes  */
    } else {
        cmd[4] = 0;
        cmd[5] = size;
    }
    cnt = send_command(fd, READ_MEMORY, cmd, 6);
    //for(i = 0; i < cnt; i++)
    //    printf("%02X ", cmd[i]);
    //printf("%X\n", size);
    return cnt;
}
