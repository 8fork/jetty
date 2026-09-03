#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdio.h>
#include <string.h>

// font definition
const char *font = "Cousine Nerd Font";
const char *fontsize = "15";
char fontstr[128];

// other config options
int padding = 10;
const char *windowname = "jetty";

// cursor type
/* X1 for block
   X2 for underline
   X3 for line
   0X for no blink
   1X for blink */
int cursor = 01;

// color definitions
const char *bg = "#121212";
const char *fg = "#fdfdfd";

const char *color0  = "#121212";
const char *color1  = "#ee1212";
const char *color2  = "#129944";
const char *color3  = "#ee9900";
const char *color4  = "#1290ee";
const char *color5  = "#db93b7";
const char *color6  = "#77eeee";
const char *color7  = "#fdfdfd";

const char *color8  = "#000000";
const char *color9  = "#ff7777";
const char *color10 = "#77ddaa";
const char *color11 = "#ffdd44";
const char *color12 = "#77daff";
const char *color13 = "#f9c2f0";
const char *color14 = "#99ffff";
const char *color15 = "#ffffff";

// functions

static gboolean onkeypress(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    VteTerminal *terminal = VTE_TERMINAL(widget);

    GdkModifierType modifiers = gtk_accelerator_get_default_mod_mask();
    gboolean ctrl_shift_pressed = ((event->state & modifiers) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK));

    if (ctrl_shift_pressed) {
        if (event->keyval == GDK_KEY_C || event->keyval == GDK_KEY_c) {
            vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
            return TRUE;
        }
        if (event->keyval == GDK_KEY_V || event->keyval == GDK_KEY_v) {
            vte_terminal_paste_clipboard(terminal);
            return TRUE;
        }
    }
    return FALSE;
}

static void onchildexit(VteTerminal *vte, gint status, gpointer user_data) {
    gtk_main_quit();
}

// main

int main(int argc, char *argv[]) {
    sprintf(fontstr, "%s %s", font, fontsize);
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), windowname);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    char css_buffer[128];
    snprintf(css_buffer, sizeof(css_buffer), "window { background-color: %s; }", bg);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_buffer, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(css_provider);

    GtkWidget *terminal = vte_terminal_new();
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(onkeypress), NULL);
    gtk_widget_set_margin_top(terminal, padding);
    gtk_widget_set_margin_bottom(terminal, padding);
    gtk_widget_set_margin_start(terminal, padding);
    gtk_widget_set_margin_end(terminal, padding);
    gtk_container_add(GTK_CONTAINER(window), terminal);

    int blink_mode = cursor / 10;
    int shape_mode = cursor % 10;

    if (blink_mode == 1) {
        vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal), VTE_CURSOR_BLINK_ON);
    } else {
        vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal), VTE_CURSOR_BLINK_OFF);
    }

    switch (shape_mode) {
        case 2:
            vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), VTE_CURSOR_SHAPE_UNDERLINE);
            break;
        case 3:
            vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), VTE_CURSOR_SHAPE_IBEAM);
            break;
        case 1:
        default:
            vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), VTE_CURSOR_SHAPE_BLOCK);
            break;
    }

    PangoFontDescription *font_desc = pango_font_description_from_string(fontstr);
    vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
    pango_font_description_free(font_desc);

    GdkRGBA bg_rgba, fg_rgba;
    gdk_rgba_parse(&bg_rgba, bg);
    gdk_rgba_parse(&fg_rgba, fg);

    GdkRGBA palette[16];
    gdk_rgba_parse(&palette[0],  color0);
    gdk_rgba_parse(&palette[1],  color1);
    gdk_rgba_parse(&palette[2],  color2);
    gdk_rgba_parse(&palette[3],  color3);
    gdk_rgba_parse(&palette[4],  color4);
    gdk_rgba_parse(&palette[5],  color5);
    gdk_rgba_parse(&palette[6],  color6);
    gdk_rgba_parse(&palette[7],  color7);
    gdk_rgba_parse(&palette[8],  color8);
    gdk_rgba_parse(&palette[9],  color9);
    gdk_rgba_parse(&palette[10], color10);
    gdk_rgba_parse(&palette[11], color11);
    gdk_rgba_parse(&palette[12], color12);
    gdk_rgba_parse(&palette[13], color13);
    gdk_rgba_parse(&palette[14], color14);
    gdk_rgba_parse(&palette[15], color15);

    vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg_rgba, &bg_rgba, palette, 16);
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal), 512);

    char *default_shell = g_strdup(g_getenv("SHELL"));
    if (!default_shell) default_shell = g_strdup("/bin/sh");
    char *command[] = { default_shell, NULL };

    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,
        command,
        NULL,
        G_SPAWN_DEFAULT,
        NULL, NULL,
        NULL,
        -1,
        NULL,
        NULL, NULL
    );

    g_signal_connect(terminal, "child-exited", G_CALLBACK(onchildexit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    g_free(default_shell);
    return 0;
}
