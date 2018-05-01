CC = gcc
TARGET = serial_flir
SRC = test.c crc.c serial_init.c utility.c \
	  send_command.c read_picture.c erase_flash.c

$(TARGET) : $(SRC)
	$(CC) -o $@ $^

clean:
	$(RM) $(TARGET)
