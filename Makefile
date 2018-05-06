CC = arm-linux-gcc
TARGET = serial_flir

SYSROOT = /home/fang/LibreELEC.tv/build.LibreELEC-RPi2.arm-9.0-devel/toolchain/arm-linux/sysroot/usr

SRC = test.c crc.c serial_init.c utility.c \
	  send_command.c read_picture.c erase_flash.c graphics.c font_8x16.c

CFLAGS = -I$(SYSROOT)/include \
		 -I$(SYSROOT)/include/gtk-2.0 \
		 -I$(SYSROOT)/include/atk-1.0 \
		 -I$(SYSROOT)/include/cairo \
		 -I$(SYSROOT)/include/pango-1.0 \
		 -I$(SYSROOT)/include/gdk-pixbuf-2.0 \
		 -I$(SYSROOT)/include/glib-2.0 \
		 -I$(SYSROOT)/lib/glib-2.0/include \
		 -I$(SYSROOT)/lib/gtk-2.0/include

LDFLAGS = -L$(SYSROOT)/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 \
		  -lglib-2.0 -lgobject-2.0 -lcairo -lpango-1.0 -latk-1.0 \
		  -Wl,-rpath,$(SYSROOT)/lib

$(TARGET) : $(SRC)
	$(CC) -o $@ $^

clean:
	$(RM) $(TARGET)
