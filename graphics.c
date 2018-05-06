/*
 * ============================================================================
 *
 *       Filename:  graphics.c
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
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>

#include "serial_flir.h"
#include "font.h"

static struct fb_fix_screeninfo finfo;
static struct fb_var_screeninfo vinfo;
static unsigned char *fbmem;
static unsigned char *flashmem;

extern int fdserial;

void drawpixel(unsigned int x, unsigned int y, int color)
{
	unsigned int ptr;

	if((x >= vinfo.xres) || (y >= vinfo.yres))
		return;
	ptr = (y * vinfo.xres+ x)*(vinfo.bits_per_pixel/8);
	*(unsigned int *)(fbmem + ptr) = color;
}

void put_char(int x, int y, int c, int colidx)
{
    int i, j, bits;

    for(i = 0; i < font_vga_8x16.height; i++) {
        bits = font_vga_8x16.data [font_vga_8x16.height * c + i];
        for (j = 0; j < font_vga_8x16.width; j++, bits <<= 1)
            if (bits & 0x80)   // TODO: 0x80 should replaced by 2^width
                drawpixel(x + j, y + i, colidx);
            else
                drawpixel(x + j, y + i, 0);
    }
}

void put_string(int x, int y, char *s, int colidx)
{
    static char oldstr[256];
    int i = 0;

    do {
        if(oldstr[i] != s[i])
            put_char(x, y, s[i], colidx);
        oldstr[i] = s[i];
        x += font_vga_8x16.width;
        i++;
    } while (s[i] != '\0');
}

#define     OFFSET_X   20
#define     OFFSET_Y   10

int draw_picture(unsigned char *buf, int width, int height)
{
    unsigned int x, y;
    int color;
    unsigned char c1 = 0;
    char strtime[256];
    time_t ticks;

    for(y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            c1 = *buf++;
            c1 = rand();
            color = (c1 << 16) | (c1 << 8) | c1 | 0xff000000;
            drawpixel(x + OFFSET_X, y + OFFSET_Y, color);
        }
    }
    ticks = time(NULL);
    sprintf((char *)strtime, "%.24s", ctime(&ticks));
    put_string(10, 250, strtime, 0xffffff00);
}

int fb_init(char *dev)
{
    int fd;
    long screensize = 0;

	fd = open (dev, O_RDWR);

	if (fd < 0){
		printf("Error : Can not open framebuffer device\n");
		exit(1);
	}
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
	printf("Resolution x = %d, y = %d\n", vinfo.xres, vinfo.yres);

	screensize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);
	fbmem = (char *)mmap(0, screensize, PROT_READ|PROT_WRITE,
			MAP_SHARED, fd,0);
	if (fbmem == NULL) {
		printf ("Error: failed to map framebuffer device to memory.\n");
		exit (-1);
	}

    return 0;
}

/* snapshot camera, send to client */
void timer_handler(int sig_num)
{
    static int snapshot = 0;
	printf("message\n");

	draw_picture(&flashmem[1078], 640, 512);

	snapshot = get_memory_address(fdserial, 0xfffe);
	if (snapshot <= 0) {
		printf("ERROR snapshot number\n");
		exit(-1);
	}
    read_picture(fdserial, snapshot - 1, flashmem);

	if (snapshot >= 20) {
        /* erase flash */
		sleep(1);
	}
	transfer_frame(fdserial);
}

int task_init(int interval)
{
    struct itimerval ival;
    struct sigaction act;

    act.sa_handler = timer_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    ival.it_interval.tv_sec = interval;       /* every interval */
    ival.it_interval.tv_usec = 0;
    ival.it_value.tv_sec = 5;           /* time to next expiration */
    ival.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &ival, NULL);

    flashmem = malloc(512*640 + 4096);
	printf("task set\n");
	return 0;
}
