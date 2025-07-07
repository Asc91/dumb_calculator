#include "../include/eval.h"
#include "../include/helper.h"
#include "../include/rpn.h"
#include "../include/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "glib.h"
#include <gtk/gtk.h>  

gboolean result_shown = FALSE;
gboolean error_shown = FALSE;

void all_clear_cb(GSimpleAction *action, GVariant *parameter,
                  gpointer user_data)
{
  GtkEntry *entry = GTK_ENTRY(user_data);
  gtk_entry_set_text(entry, "");
}

void clear_on_input_cb(GtkEditable *editable,
                       const gchar *new_text,
                       gint new_text_length,
                       gint *position,
                       gpointer user_data)
{
  GtkBuilder *builder = GTK_BUILDER(user_data);
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "calc_display"));
  GtkTextView *error_box = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "calc_error_box"));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(error_box);

  if (new_text_length == 0)
  {
    return; // No input, do nothing
  }else {
    // entered operator after result, continue expression
    if(!isdigit(new_text[0]) && !isalpha(new_text[0]) && result_shown){
      const gchar *current = gtk_entry_get_text(entry);
      *position = g_utf8_strlen(current, -1);
      result_shown = FALSE;
    }else {
      // new input, clear previous result 
      if (result_shown)
      {
        gtk_entry_set_text(entry, "");
        result_shown = FALSE;
      }
    }
    // if error is shown, clear it
    if (error_shown)
      {
        gtk_text_buffer_set_text(buffer, "", -1);
        error_shown = FALSE;
      }
  }
  
}

void del_buffer_cb(GSimpleAction *action, GVariant *parameter,
                   gpointer user_data)
{
  GtkEntry *entry = GTK_ENTRY(user_data);
  const gchar *old_text = gtk_entry_get_text(entry);

  gsize size = strlen(old_text);
  if (size == 0)
  {
    return;
  }
  gchar *new_text = g_strndup(old_text, size - 1);
  gtk_entry_set_text(entry, new_text);

  g_free(new_text);
}

void eval_cb(GSimpleAction *action, GVariant *parameter,
             gpointer user_data) 
{
  GtkBuilder *builder = GTK_BUILDER(user_data);
  GtkEntry *entry = GTK_ENTRY(gtk_builder_get_object(builder, "calc_display"));
  GtkTextView *error_box = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "calc_error_box"));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(error_box);

  const gchar *input = gtk_entry_get_text(entry);

  token_t result;
  err_t err = eval_expression(g_strdup(input), &result);
  if (err != OK) {
    gtk_text_buffer_set_text(buffer, get_error_message(err), -1);
    error_shown = TRUE;
  }else {
    gtk_entry_set_text(entry, g_strdup_printf("%.2lf", result.val.num));
    result_shown = TRUE;
  }
}

void enter_key_cb(GtkEntry *entry, gpointer user_data)
{
  eval_cb(NULL, NULL, user_data);  
}

static void push_buffer_cb(GSimpleAction *action, GVariant *parameter,
                           gpointer user_data)
{
  const gchar *input = g_variant_get_string(parameter, NULL);

  GtkEntry *entry = GTK_ENTRY(user_data);
  const gchar *old_text = gtk_entry_get_text(entry);

  gchar *new_text = g_strconcat(old_text, input, NULL);
  gtk_entry_set_text(entry, new_text);

  g_free(new_text);
}
void toggle_op_pad_cb(GSimpleAction *action, GVariant *state,
                      gpointer user_data)
{
  GtkStack *op_stack = GTK_STACK(user_data);
  gboolean is_bitwise = g_variant_get_boolean(state);
  gtk_stack_set_visible_child_name(op_stack,
                                   is_bitwise ? "bitwise_op_pad" : "arithmetic_op_pad");
  g_simple_action_set_state(action, state);
}

// main
int main() {
  GtkBuilder *builder;
  GObject *window;
  GError *error = NULL;
  GObject *entry;
  GObject *op_stack;

  gtk_init(NULL, NULL);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_resource(builder, "/asc91/dumb-calc/calc-gtk.ui", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  window = gtk_builder_get_object(builder, "calc_window");
  entry = gtk_builder_get_object(builder, "calc_display");
  op_stack = gtk_builder_get_object(builder, "calc_op_stack");

  gtk_widget_show_all(GTK_WIDGET(window));

  GSimpleAction *all_clear =
      g_simple_action_new("all_clear", NULL);
  g_signal_connect(all_clear, "activate", G_CALLBACK(all_clear_cb), entry);
  g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION_WINDOW(window)),
                          G_ACTION(all_clear));

  GSimpleAction *del_buffer =
      g_simple_action_new("del_buffer", NULL);
  g_signal_connect(del_buffer, "activate", G_CALLBACK(del_buffer_cb), entry);
  g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION_WINDOW(window)),
                          G_ACTION(del_buffer));

  GSimpleAction *eval = g_simple_action_new("eval", NULL);
  g_signal_connect(eval, "activate", G_CALLBACK(eval_cb), builder);
  g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION_WINDOW(window)), G_ACTION(eval));
  g_signal_connect(GTK_ENTRY(entry), "activate", G_CALLBACK(enter_key_cb), builder);

  GSimpleAction *push_buffer =
      g_simple_action_new("push_buffer", G_VARIANT_TYPE_STRING);
  g_signal_connect(push_buffer, "activate", G_CALLBACK(push_buffer_cb), entry);
  g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION_WINDOW(window)),
                          G_ACTION(push_buffer));

  GVariant *toggle_state = g_variant_new_boolean(FALSE);
  GSimpleAction *toggle_op_pad =
      g_simple_action_new_stateful("toggle_op_pad", NULL,
                                   toggle_state);
  g_signal_connect(toggle_op_pad, "change-state", G_CALLBACK(toggle_op_pad_cb), op_stack);
  g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION_WINDOW(window)),
                          G_ACTION(toggle_op_pad));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(GTK_ENTRY(entry), "insert-text", G_CALLBACK(clear_on_input_cb), builder);
  gtk_main();
  return 0;
}