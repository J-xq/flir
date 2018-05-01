/*
 * ============================================================================
 *
 *       Filename:  serial_flir.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2018 09:09:43 PM
 *       Revision:  none
 *       Compiler: 
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#ifndef _SERIAL_FLIR_H
#define _SERIAL_FLIR_H

#define NO_OP               0x00
#define SET_DEFAULTS        0x01
#define CAMERA_RESET        0x02
#define FACTORY_DEFAULTS    0x03
#define GET_SERIAL_NUMBER   0x04
#define GET_VERSION         0x05
#define BAUD_RATE           0x07

#define READ_SENSOR         0x20

#define DIGITAL_OUTPUT_MODE 0x12
#define SERIAL_NUMBER       0x65
#define CAMERA_PART         0x66
#define SHUTTER_POSITION    0x79
#   define  OPEN               0
#   define  CLOSE              1

#define TRANSFER_FRAME      0x82

#define MEMORY_STATUS       0xc4
#define READ_MEMORY         0xd2
#define ERASE_MEMORY_BLOCK  0xd4
#define GET_NV_MEMORY_SIZE  0xd5
#define GET_MEMORY_ADDRESS  0xd6

int serial_init(int);
int send_command(int fd, unsigned char CODE, char *data, int ncmd);
int read_picture(int fd, char n);
int erase_flash(int fd, char n);

/* utility */
int shutter_position(int fd, char mode);
int camera_part(int fd);
int camera_serial_no(int fd);
int digital_output_mode(int fd, int mode);
int transfer_frame(int fd, int snapshot);
int baud_rate(int fd, int mode);
int set_defaults(int fd);
int camera_reset(int fd);
int factory_defaults(int fd);

#endif
