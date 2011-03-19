#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

gchar *FILENAME;
gchar *short_name;
GtkWidget *window;
GtkWidget *textbox;
GtkTextBuffer *buffer;

char *get_short_name(char *string)
{
    char *p;
    char *name;

    name = a;

    if ((p = strrchr(name, '/')) != NULL) {
        name = p+1;
    }
    return name;
}

void save_as_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkTextIter start;
    GtkTextIter end;

    FILE *fp;

    dialog = gtk_file_chooser_dialog_new("Save file",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_OK,
            NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        FILENAME = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if ((fp = fopen(FILENAME, "w")) == NULL) {
            fprintf(stderr, "Unable to open '%s'\n", FILENAME);
            return;
        }

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);

        fprintf(fp, "%s", gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));

        if (fclose(fp) == EOF) {
            fprintf(stderr, "Unable to close '%s'\n", FILENAME);
            return;
        }
        printf("Saved file: %s\n", FILENAME);
        short_name = get_short_name(FILENAME);
//         gtk_window_set_title(GTK_WINDOW(window), 

        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);
    }
    gtk_widget_destroy(dialog);
}

void save_file(GtkWidget *widget, gpointer data)
{
    GtkTextIter start;
    GtkTextIter end;

    FILE *fp;

    if (FILENAME == NULL) {
        save_as_file(NULL, NULL);
        return;
    }

    if ((fp = fopen(FILENAME, "w")) == NULL) {
        fprintf(stderr, "Unable to open '%s'\n", FILENAME);
        return;
    }

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);

    fprintf(fp, "%s", gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Unable to close '%s'\n", FILENAME);
        return;
    }
    printf("Saved file: %s\n", FILENAME);
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);
}

int confirm_dialog(void)
{
    GtkWidget *dialog;
    GtkWidget *label;
    gint response;

    dialog = gtk_dialog_new();

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Cancel", 0, "No", 1, "Yes", 2, NULL);

    label = gtk_label_new("Save changes to file?");

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);

    gtk_widget_show_all(dialog);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == 2) {
        save_file(NULL, NULL);
    }
    return response;
}

void new_file(GtkWidget *widget, gpointer data)
{
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

    // Are you sure you want to quit?
    if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(buffer))) {
        if (confirm_dialog() == 0) {
            return;
        }
    }

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), "", 0);
}


void open_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkTextIter start;

    FILE *fp;
    // Convert the char to a string so it can get added to the text buffer
    gchar ch[] = {' ', '\0'};

    dialog = gtk_file_chooser_dialog_new("Open...",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_OK,
            NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

        // Are you sure you want to quit?
        if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(buffer))) {
            if (confirm_dialog() == 0) {
                return;
            }
        }

        FILENAME = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if ((fp = fopen(FILENAME, "r")) == NULL) {
            fprintf(stderr, "Unable to open '%s'\n", FILENAME);
            return;
        }
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), "", 0);

        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);

        while ((ch[0] = fgetc(fp)) != EOF) {
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &start, ch, 1);
        }

        if (fclose(fp) == EOF) {
            fprintf(stderr, "Unable to close '%s'\n", FILENAME);
            return;
        }
        printf("Opened file: %s\n", FILENAME);
        gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);
    }
    gtk_widget_destroy(dialog);
}


gboolean delete_event(GtkWidget *widget, gpointer data)
{
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
         
    // Are you sure you want to quit?
    if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(buffer))) {
        if (confirm_dialog() == 0) {
            return TRUE;
        }
    }

    gtk_main_quit();
    return FALSE;
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
    gtk_window_set_title(GTK_WINDOW(window), "Untitled - Noterad");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect_swapped(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);

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

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);

    // Add textbox to scrolled window
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), textbox);

    gtk_box_pack_start(GTK_BOX(container), sw, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), container);
    gtk_widget_show_all(window);

    gtk_main();
    return EXIT_SUCCESS;
}

