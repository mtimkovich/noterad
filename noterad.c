#include <gtk/gtk.h>
#include <stdlib.h>

gchar *filename;
GtkWidget *window;
GtkWidget *textbox;

void new_file(GtkWidget *widget, gpointer data)
{
    GtkTextBuffer *textbuffer;

    textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), "", 0);
}

void save_file(GtkWidget *widget, gpointer data)
{
    printf("save\n");
}

void open_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkTextBuffer *textbuffer;
    GtkTextIter start;

    FILE *fp;
    gchar ch[] = {' ', '\0'};

    dialog = gtk_file_chooser_dialog_new ("Open...",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_OK,
            NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if ((fp = fopen(filename, "r")) == NULL) {
            fprintf(stderr, "Unable to open '%s'\n", filename);
            exit(EXIT_FAILURE);
        }
        textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), "", 0);

        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(textbuffer), &start);

        while ((ch[0] = fgetc(fp)) != EOF) {
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(textbuffer), &start, ch, 1);
        }

        if (fclose(fp) == EOF) {
            fprintf(stderr, "Unable to close '%s'\n", filename);
            exit(EXIT_FAILURE);
        }
        printf("Opened file: %s\n", filename);
    }
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[])
{
    GtkWidget *container;
    GtkWidget *menu_buttons;
    GtkWidget *button;
    GtkWidget *sw;

    gtk_init(&argc, &argv);

    // Set up window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 750, 450);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create Boxes
    container = gtk_vbox_new(FALSE, 3);
    menu_buttons = gtk_hbox_new(FALSE, 0);

    // Put menu_buttons in container
    gtk_box_pack_start(GTK_BOX(container), menu_buttons, FALSE, FALSE, 0);

    // Create new button
    button = gtk_button_new_with_label("New"); 
    gtk_widget_set_size_request(button, 90, 30);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(new_file), NULL);
    gtk_box_pack_start(GTK_BOX(menu_buttons), button, FALSE, FALSE, 3);

    // Create save button
    button = gtk_button_new_with_label("Save"); 
    gtk_widget_set_size_request(button, 90, 30);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(save_file), NULL);
    gtk_box_pack_start(GTK_BOX(menu_buttons), button, FALSE, FALSE, 3);

    // Create open button
    button = gtk_button_new_with_label("Open"); 
    gtk_widget_set_size_request(button, 90, 30);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(open_file), NULL);
    gtk_box_pack_start(GTK_BOX(menu_buttons), button, FALSE, FALSE, 3);

    // Create Scrolled window
    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    // Create textbox
    textbox = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textbox), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textbox), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textbox), TRUE);

    // Add textbox to scrolled window
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), textbox);

    gtk_box_pack_start(GTK_BOX(container), sw, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), container);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}



