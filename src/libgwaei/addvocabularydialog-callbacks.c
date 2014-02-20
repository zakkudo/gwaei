/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Zachary Dovel. All Rights Reserved.

    LICENSE:
    This file is part of gWaei.

    gWaei is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gWaei is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with gWaei.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

//!
//!  @file addvocabularydialog-callbacks.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/addvocabularydialog-private.h>

/*
void 
gw_addvocabularydialog_add_cb (GtkWidget *widget, gpointer data)
{
    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;
    GwAddVocabularyWindowClass *klass;
    GtkListStore *wordstore;
    const gchar *kanji, *furigana, *definitions;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;
    klass = GW_ADDVOCABULARYWINDOW_CLASS (G_OBJECT_GET_CLASS (window));

    kanji = gw_addvocabularydialog_get_kanji (window);
    furigana = gw_addvocabularydialog_get_furigana (window);
    definitions = gw_addvocabularydialog_get_definitions (window);
    wordstore = gw_addvocabularydialog_get_wordstore (window);
    priv->wordstore = GW_VOCABULARYWORDSTORE (wordstore);

    gw_vocabularywordstore_load (priv->wordstore, NULL);
    gw_vocabularywordstore_new_word (priv->wordstore, &(priv->iter), NULL, kanji, furigana, definitions);

    if (klass->last_selected_list_name != NULL)
      g_free (klass->last_selected_list_name);
    klass->last_selected_list_name = g_strdup (gw_addvocabularydialog_get_list (window));

    priv->valid = TRUE;

    g_signal_emit (G_OBJECT (window), klass->signalid[GW_ADDVOCABULARYWINDOW_CLASS_SIGNALID_WORD_ADDED], 0);

    gtk_widget_destroy (GTK_WIDGET (window));
}


void 
gw_addvocabularydialog_cancel_cb (GtkWidget *widget, gpointer data)
{
    GwAddVocabularyWindow *window;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);

    gtk_widget_destroy (GTK_WIDGET (window));
}


void
gw_addvocabularydialog_kanji_changed_cb (GtkWidget *widget, gpointer data)
{
    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;

    if (priv->kanji_text != NULL) g_free (priv->kanji_text);
    priv->kanji_text = g_strdup (gtk_entry_get_text (priv->kanji_entry));
    g_strstrip (priv->kanji_text);

    gw_addvocabularydialog_validate (window);
}


void
gw_addvocabularydialog_furigana_changed_cb (GtkWidget *widget, gpointer data)
{
    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;

    if (priv->furigana_text != NULL) g_free (priv->furigana_text);
    priv->furigana_text = g_strdup (gtk_entry_get_text (priv->furigana_entry));
    g_strstrip (priv->furigana_text);

    gw_addvocabularydialog_validate (window);
}


void
gw_addvocabularydialog_definitions_paste_done_cb (GtkTextBuffer *buffer,
                                                  GtkClipboard  *clipboard,
                                                  gpointer       data      )
{
    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;
    GtkTextIter start, end;
    gchar *text;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;

    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);


    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    g_strdelimit (text, "\n", ' ');
    g_strstrip (text);

    gtk_text_buffer_set_text (buffer, text, -1);

    priv->pasted = TRUE;

    g_free (text);
}


void
gw_addvocabularydialog_definitions_event_after_cb (GtkWidget *widget, GdkEvent *event, gpointer data)
{

    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;
    GtkTextIter start, end;
    GtkTextBuffer *buffer;
    gboolean valid;
    gboolean activate;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;

    if (priv->pasted == TRUE) { priv->pasted = FALSE; return; }

    buffer = gtk_text_view_get_buffer (priv->definitions_textview);
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);

    if (priv->definitions_text != NULL) g_free (priv->definitions_text);
    priv->definitions_text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    activate = (strchr (priv->definitions_text, '\n') != NULL);
    g_strstrip (priv->definitions_text);

    gchar *source_ptr, *target_ptr;
    source_ptr = target_ptr = priv->definitions_text;
    while (*target_ptr != '\0')
    {
      if (*target_ptr == '\n') target_ptr++;
      else if (source_ptr == target_ptr)
      {
        source_ptr = ++target_ptr;
      }
      else
      {
        *(source_ptr++) = *(target_ptr++);
      }
    }
    *source_ptr = '\0';

    valid = gw_addvocabularydialog_validate (window);

    if (activate)
    {
      G_GNUC_EXTENSION g_signal_handlers_block_by_func (widget, gw_addvocabularydialog_definitions_event_after_cb, data);
      gtk_text_buffer_set_text (buffer, priv->definitions_text, -1);
      G_GNUC_EXTENSION g_signal_handlers_unblock_by_func (widget, gw_addvocabularydialog_definitions_event_after_cb, data);
      if (valid)
      {
        gtk_button_clicked (priv->add_button);
      }
    }
}


void
gw_addvocabularydialog_list_changed_cb (GtkWidget *widget, gpointer data)
{
    GwAddVocabularyWindow *window;
    GwAddVocabularyWindowPrivate *priv;
    GtkEntry *entry;

    window = GW_ADDVOCABULARYWINDOW (gtk_widget_get_ancestor (GTK_WIDGET (data), GW_TYPE_ADDVOCABULARYWINDOW));
    g_return_if_fail (window != NULL);
    priv = window->priv;
    entry = GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->vocabulary_list_combobox)));

    if (priv->list_text != NULL) g_free (priv->list_text);
    priv->list_text = g_strdup (gtk_entry_get_text (entry));
    g_strstrip (priv->list_text);

    gw_addvocabularydialog_validate (window);
}

*/

void
lgw_addvocabularydialog_kanji_changed_cb (LgwAddVocabularyDialog *self,
                                          GtkEditable            *inner_entry)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_ENTRY (inner_entry));

    printf("BREAK kanji_changed\n");
}


void
lgw_addvocabularydialog_reading_changed_cb (LgwAddVocabularyDialog *self,
                                            GtkEditable            *inner_entry)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_ENTRY (inner_entry));

    printf("BREAK reading_changed\n");
}


void
lgw_addvocabularydialog_definition_changed_cb (LgwAddVocabularyDialog *self,
                                               GtkTextBuffer          *inner_text_buffer)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_TEXT_BUFFER (inner_text_buffer));

    lgw_addvocabularydialog_sync_definition_text_buffer (self);

    printf("BREAK definition_changed\n");
}


gboolean
lgw_addvocabularydialog_definition_focus_in_event_cb (LgwAddVocabularyDialog *self,
                                                      GdkEvent               *event,
                                                      GtkTextView            *inner_text_view)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_TEXT_VIEW (inner_text_view));

    lgw_addvocabularydialog_sync_definition_text_buffer (self);

    printf("BREAK focus_in_event\n");

    return FALSE;
}


gboolean
lgw_addvocabularydialog_definition_focus_out_event_cb (LgwAddVocabularyDialog *self,
                                                       GdkEvent               *event,
                                                       GtkTextView            *inner_text_view)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_TEXT_VIEW (inner_text_view));

    lgw_addvocabularydialog_sync_definition_text_buffer (self);

    printf("BREAK focus_out_event\n");

    return FALSE;
}


void
lgw_addvocabularydialog_response_cb (LgwAddVocabularyDialog *self,
                                     gint                    response_id,
                                     GtkDialog              *dialog)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_DIALOG (dialog));

    lgw_addvocabularydialog_sync_definition_text_buffer (self);

    printf("BREAK response is %d\n", response_id);
}


void
lgw_addvocabularydialog_connect_signals (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_KANJI_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_KANJI_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.kanji_entry),
          "changed",
          G_CALLBACK (lgw_addvocabularydialog_kanji_changed_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_READING_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_READING_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.reading_entry),
          "changed",
          G_CALLBACK (lgw_addvocabularydialog_reading_changed_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_CHANGED] == 0)
    {
        priv->data.signalid[SIGNALID_DEFINITION_CHANGED] = g_signal_connect_swapped (
          G_OBJECT (priv->data.definition.text_buffer),
          "changed",
          G_CALLBACK (lgw_addvocabularydialog_definition_changed_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_FOCUS_IN_EVENT] == 0)
    {
        priv->data.signalid[SIGNALID_DEFINITION_FOCUS_IN_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.definition_text_view),
          "focus-in-event",
          G_CALLBACK (lgw_addvocabularydialog_definition_focus_in_event_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_FOCUS_OUT_EVENT] == 0)
    {
        priv->data.signalid[SIGNALID_DEFINITION_FOCUS_OUT_EVENT] = g_signal_connect_swapped (
          G_OBJECT (priv->ui.definition_text_view),
          "focus-out-event",
          G_CALLBACK (lgw_addvocabularydialog_definition_focus_out_event_cb),
          self
        );
    }

    if (priv->data.signalid[SIGNALID_RESPONSE] == 0)
    {
        priv->data.signalid[SIGNALID_RESPONSE] = g_signal_connect_swapped (
          G_OBJECT (self),
          "response",
          G_CALLBACK (lgw_addvocabularydialog_response_cb),
          self
        );
    }
}


void
lgw_addvocabularydialog_disconnect_signals (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.signalid[SIGNALID_KANJI_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.kanji_entry),
        priv->data.signalid[SIGNALID_KANJI_CHANGED]
      );
      priv->data.signalid[SIGNALID_KANJI_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_READING_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.reading_entry),
        priv->data.signalid[SIGNALID_READING_CHANGED]
      );
      priv->data.signalid[SIGNALID_READING_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_CHANGED] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->data.definition.text_buffer),
        priv->data.signalid[SIGNALID_DEFINITION_CHANGED]
      );
      priv->data.signalid[SIGNALID_DEFINITION_CHANGED] = 0;
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_FOCUS_IN_EVENT] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.definition_text_view),
        priv->data.signalid[SIGNALID_DEFINITION_FOCUS_IN_EVENT]
      );
      priv->data.signalid[SIGNALID_DEFINITION_FOCUS_IN_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_DEFINITION_FOCUS_OUT_EVENT] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (priv->ui.definition_text_view),
        priv->data.signalid[SIGNALID_DEFINITION_FOCUS_OUT_EVENT]
      );
      priv->data.signalid[SIGNALID_DEFINITION_FOCUS_OUT_EVENT] = 0;
    }

    if (priv->data.signalid[SIGNALID_RESPONSE] != 0)
    {
      g_signal_handler_disconnect (
        G_OBJECT (self),
        priv->data.signalid[SIGNALID_RESPONSE]
      );
      priv->data.signalid[SIGNALID_RESPONSE] = 0;
    }
}

