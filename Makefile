CC = gcc
TARGET = serial_flir

SRC = flir_gtk.c crc.c serial_init.c utility.c \
	  send_command.c draw_picture.c erase_flash.c

$(TARGET) : $(SRC)
	$(CC) -o $@ $^  `pkg-config gtk+-2.0 --cflags --libs`

clean:
	$(RM) $(TARGET)
