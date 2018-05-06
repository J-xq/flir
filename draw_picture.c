/*
 * ============================================================================
 *
 *       Filename:  draw_picture.c
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
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>

#include "serial_flir.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

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

#define     OFFSET_X   20
#define     OFFSET_Y   10

int draw_picture(unsigned char *buf, int width, int height)
{
    unsigned int x, y;
    int color;
    unsigned char c1 = 0;
    for(y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            c1 = *buf++;
            c1 = rand();
            color = (c1 << 16) | (c1 << 8) | c1 | 0xff000000;
            drawpixel(x + OFFSET_X, y + OFFSET_Y, color);
        }
    }
}

/*  Redraw the screen every n-miliseconds */
gint gtk_timer(GtkWidget *widget)
{
    GdkGC *gc;
    GdkColor color;
    GdkFont *font;
    int x, y, c1;
	unsigned char *p = &flashmem[1078];
	static int snapshot = -1;

    gc = gdk_gc_new (widget->window);

	for(y = 0; y < 512; y++) {
		for(x = 0; x < 640; x++) {
			c1 = *p++;
			color.red = c1 << 8;
			color.green = c1 << 8;
			color.blue = c1 << 8;
			gdk_gc_set_rgb_fg_color(gc ,&color);
			gdk_draw_point(widget->window,
                  gc, x, y);
		}
	}

    font = gdk_font_load("lucidasans-bolditalic-14");

    color.red = 0xffff;
	color.green = 0;
	color.blue = 0;
    gdk_gc_set_rgb_fg_color(gc ,&color);
    gdk_draw_string (widget->window,
            font,
            gc,
            20, 20, "hello");

	snapshot++;
	if (snapshot > 20) {
		/* erase flash */
		snapshot = 0;
	}
    read_picture(fdserial, snapshot);

    return TRUE;
}

/*  Redraw the screen from the backing pixmap */
gboolean expose_callback(GtkWidget *widget, GdkEventExpose *e, gpointer data)
{
    GdkGC *gc;
    GdkColor color;
    GdkFont *font;
    int x, y, c1;
	unsigned char *p = &flashmem[1078];

    gc = gdk_gc_new (widget->window);

	for(y = 0; y < 512; y++) {
		for(x = 0; x < 640; x++) {
			c1 = *p++;
			color.red = c1 << 8;
			color.green = c1 << 8;
			color.blue = c1 << 8;
			gdk_gc_set_rgb_fg_color(gc ,&color);
			gdk_draw_point(widget->window,
                  gc, x, y);
		}
	}

    font = gdk_font_load("lucidasans-bolditalic-14");

    color.red = 0xffff;
	color.green = 0;
	color.blue = 0;
    gdk_gc_set_rgb_fg_color(gc ,&color);
    gdk_draw_string (widget->window,
            font,
            gc,
            20, 20, "hello");

    return TRUE;
}

int read_picture(int fd, char n)
{
    unsigned char cmd[512];
    int cnt, i;
    int picptr = 0;
	unsigned int addr;
	int size;
    int offset, width, height;

	int x, y;
	for(y = 0; y < 512; y++) {
		for(x = 0; x < 640; x++) {
			flashmem[1078 + y*640+x] = rand();
		}
	}
	return 0;


	cmd[0] = 0; cmd[1] = n; cmd[2] = 0; cmd[3] = 0x13; // read snapshot n
	cnt = send_command(fd, GET_MEMORY_ADDRESS, cmd, 4);

    addr = (cmd[8] << 24) | (cmd[9] << 16) | (cmd[10] << 8) | cmd[11];
    size = (cmd[12] << 24) | (cmd[13] << 16) | (cmd[14] << 8) | cmd[15];
    
    printf("snapshot addr: %08X %08X\n", addr, size);
    picptr = 0;
    while(size > 0) {
        read_memory(fd, cmd, addr, size);

        if (size > 255)
            memcpy(&flashmem[picptr], &cmd[8], 256);
        else
            memcpy(&flashmem[picptr], &cmd[8], size);

        size -= 256;
        addr += 256;
        picptr += 256;
        break;
    }

    offset = *(int *)(&flashmem[10]);
    width  = *(int *)(&flashmem[18]);
    height = *(int *)(&flashmem[22]);

    printf("%d %d %d\n", offset, width, height);

    //draw_picture(&flashmem[offset], width, height);

    return 0;
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

    read_picture(fdserial, snapshot);
    snapshot++;
    if (snapshot > 20) {
        /* erase flash */
        snapshot = 0;
    }
}

int task_init(int interval)
{
    struct itimerval ival;
    struct sigaction act;

    flashmem = malloc(512*640 + 4096);
	printf("task set\n");
	return 0;

    act.sa_handler = timer_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    ival.it_interval.tv_sec = interval;       /* every interval */
    ival.it_interval.tv_usec = 0;
    ival.it_value.tv_sec = interval;           /* time to next expiration */
    ival.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &ival, NULL);

}
