#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

GtkWidget *window;
GtkWidget *hbox;
GtkWidget *textbox;
GtkTextBuffer *buffer;

char *FILENAME;
char *short_name;

void about_dialog(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;

    dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Noterad");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(c) Max Timkovich");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "A simple GTK text editor");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

char *get_short_name(char *string)
{
    char *p;
    char *name;

    name = string;

    if ((p = strrchr(name, '/')) != NULL) {
        name = p+1;
    }
    return name;
}

void set_title(void)
{
    const int TITLE_BUFFER = 40;
    char title[TITLE_BUFFER];

    if (FILENAME != NULL) {
        short_name = get_short_name(FILENAME);
    } else {
        short_name = "Untitled";
    }
    snprintf(title, TITLE_BUFFER, "%s - Noterad", short_name);
    gtk_window_set_title(GTK_WINDOW(window), title);
}


int save_as_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Save file",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_OK,
            NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        FILENAME = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        set_title();
        gtk_widget_destroy(dialog);
        return 1;
    } else {
        gtk_widget_destroy(dialog);
        return 0;
    }
}

void save_file(GtkWidget *widget, gpointer data)
{
    GtkTextIter start;
    GtkTextIter end;

    FILE *fp;

    if (FILENAME == NULL) {
        if (save_as_file(NULL, NULL) == 0) {
            return;
        }
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

    const int LABEL_SIZE = 60;
    char label_text[LABEL_SIZE];
    int response;

    dialog = gtk_dialog_new();

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);
    gtk_window_set_title(GTK_WINDOW(dialog), "");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Cancel", 0, "No", 1, "Yes", 2, NULL);

    snprintf(label_text, LABEL_SIZE, "Save changes to '%s'?", short_name);
    label = gtk_label_new(label_text);

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

    FILENAME = NULL;
    set_title();

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), "", 0);
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);
}


void open_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkTextIter start;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));

    // Are you sure you want to quit?
    if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(buffer))) {
        if (confirm_dialog() == 0) {
            return;
        }
    }

    FILE *fp;
    // Convert the char to a string so it can get added to the text buffer
    char ch[] = {' ', '\0'};

    dialog = gtk_file_chooser_dialog_new("Open...",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_OK,
            NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
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
        set_title();
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

void menubar(void)
{
    GtkWidget *menu_bar;
    GtkWidget *file;
    GtkWidget *file_menu;
    GtkWidget *help;
    GtkWidget *help_menu;
    GtkWidget *menu_item;
    GtkWidget *sep;
    GtkAccelGroup *accel_group;

    menu_bar = gtk_menu_bar_new();

    file_menu = gtk_menu_new();
    file = gtk_menu_item_new_with_mnemonic("_File");

    help_menu = gtk_menu_new();
    help = gtk_menu_item_new_with_mnemonic("_Help");

    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    // New
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(new_file), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, 
            GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Open
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(open_file), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, 
            GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Save
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(save_file), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, 
            GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // Save As
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(save_as_file), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, 
            GDK_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);

    // Separator
    sep = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep);

    // Quit
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_event), NULL);
    gtk_widget_add_accelerator(menu_item, "activate", accel_group, 
            GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    // About
    menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(about_dialog), NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);

    gtk_box_pack_start(GTK_BOX(hbox), menu_bar, FALSE, FALSE, 0);
}

int main(int argc, char *argv[])
{
    GtkWidget *container;
    GtkWidget *button;
    GtkWidget *sw;

    gtk_init(&argc, &argv);

    // Set up window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 750, 450);
    g_signal_connect_swapped(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);

    set_title();

    // Create Boxes
    container = gtk_vbox_new(FALSE, 0);
    hbox = gtk_hbox_new(FALSE, 0);

    // Put menu_buttons in container
    gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE, 0);

    menubar();

    // Create Scrolled window
    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    // Create textbox
    textbox = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textbox), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textbox), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textbox), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textbox), 1);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textbox), 1);

    gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(textbox), GTK_TEXT_WINDOW_LEFT, 1);
    gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(textbox), GTK_TEXT_WINDOW_RIGHT, 1);
    gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(textbox), GTK_TEXT_WINDOW_TOP, 0);
    gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(textbox), GTK_TEXT_WINDOW_BOTTOM, 1);

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textbox));
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(buffer), FALSE);

    // Add textbox to scrolled window
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), textbox);

    gtk_box_pack_start(GTK_BOX(container), sw, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), container);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

