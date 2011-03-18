#include <gtk/gtk.h>

void save_file(GtkWidget *widget, gpointer data)
{
    printf("save\n");
}

void open_file(GtkWidget *widget, gpointer data)
{
    printf("open\n");
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *container;
    GtkWidget *menu_buttons;
    GtkWidget *save_button;
    GtkWidget *open_button;
    GtkWidget *textbox;
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

    // Create save button
    save_button = gtk_button_new_with_label("Save"); 
    gtk_widget_set_size_request(save_button, 90, 30);
    g_signal_connect(G_OBJECT(save_button), "clicked", G_CALLBACK(save_file), NULL);
    gtk_box_pack_start(GTK_BOX(menu_buttons), save_button, FALSE, FALSE, 3);

    // Create open button
    open_button = gtk_button_new_with_label("Open"); 
    gtk_widget_set_size_request(open_button, 90, 30);
    g_signal_connect(G_OBJECT(open_button), "clicked", G_CALLBACK(open_file), NULL);
    gtk_box_pack_start(GTK_BOX(menu_buttons), open_button, FALSE, FALSE, 3);

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



