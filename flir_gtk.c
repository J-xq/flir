#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial_flir.h"

extern
gboolean expose_callback(GtkWidget *widget, GdkEventExpose *e, gpointer data);

extern gint gtk_timer(gpointer data);

int main(int argc, char *argv[]) {

    GtkWidget *window;
    GtkWidget *draw;
    gint tag;

    int fd, i, ret;
    unsigned char snapshot = 0;
    int bb;

    //if (argc > 1) {
    //    bb = atoi(argv[1]);
    //} else {
    //    bb = 2;
    //}
    //fd = serial_init(bb);
    //baud_rate(fd, 0x0007);
    //close(fd);
    //fd = serial_init(5);

    //camera_part(fd);
    //camera_serial_no(fd);

    //ret = fb_init("/dev/fb0");
    //digital_output_mode(fd, 0x0303);  /* 8bit bmp */

    task_init(3);
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    draw = gtk_drawing_area_new();
    gtk_widget_set_size_request(draw, 640, 512);

    gtk_container_add(GTK_CONTAINER(window), draw);  
    g_signal_connect (G_OBJECT (draw), "expose-event",
                      G_CALLBACK (expose_callback), NULL);
    gtk_widget_show_all(window);

    tag = gtk_timeout_add(5000, gtk_timer, draw);

    gtk_main();

    gtk_timeout_remove(tag); 
    return 0;
}
