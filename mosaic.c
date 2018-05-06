#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

#define	BLOCKSIZE	64

unsigned char *fbp = 0;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

struct coord
{
	int x, y;
};

void drawpixel(unsigned int x, unsigned int y, int color)
{
	unsigned int ptr;

	if((x >= vinfo.xres_virtual) || (y >= vinfo.yres_virtual))
		return;
	ptr = (y * vinfo.xres+ x)*(vinfo.bits_per_pixel/8);
	*(unsigned int *)(fbp + ptr) = color;
}

void *drawblock(void *param)
{
	struct coord *p = (struct coord*)param;
	int i, j, color;

	while(1) {
		color = rand();
		for(i = 0; i < BLOCKSIZE; i++)
			for(j = 0; j < BLOCKSIZE; j++)
				drawpixel(p->x + i, p->y + j, color);
		usleep((unsigned)rand()/500);
	}
}

int main (int argc, char *argv[])
{
	int fd = 0;
	long screensize = 0;
	int i, j, k, color = 0;
	pthread_t threadId;
	struct coord xy[500];

	fd = open ("/dev/fb0", O_RDWR);

	if (fd < 0){
		printf("Error : Can not open framebuffer device\n");
		exit(1);
	}
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
	printf("Resolution x = %d, y = %d\n", vinfo.xres, vinfo.yres);
	printf("Virtual x = %d, y = %d\n", vinfo.xres_virtual, vinfo.yres_virtual);
	printf("Offset x = %d, y = %d\n", vinfo.xoffset, vinfo.yoffset);
	printf("bits = %d\n", vinfo.bits_per_pixel);

	screensize = vinfo.xres_virtual*vinfo.yres_virtual*(vinfo.bits_per_pixel/8);
	fbp =(char *)mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fd,0);
    if (fbp == NULL) {
		printf ("Error: failed to map framebuffer device to memory.\n");
		exit (-1);
	}

	k = 0;
	for(i = 0; i < vinfo.xres; i += BLOCKSIZE) {
		for(j = 0; j < vinfo.yres; j += BLOCKSIZE) {
			xy[k].x = i;
			xy[k].y = j;
			pthread_create (&threadId, NULL, drawblock, &xy[k]);
			k++;
		}
	}
	sleep(5);
	vinfo.yoffset = 240;
	ioctl(fd, FBIOPAN_DISPLAY, &vinfo);
	sleep(5);
	munmap (fbp, screensize);
	close (fd);

	return 0;
}

