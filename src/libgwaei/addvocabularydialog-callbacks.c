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


void
lgw_addvocabularydialog_kanji_changed_cb (LgwAddVocabularyDialog *self,
                                          GtkEditable            *inner_editable)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_EDITABLE (inner_editable));

    //Declarations
    const gchar *TEXT = NULL;
    GtkEntry *entry = NULL;

    //Initializations
    entry = GTK_ENTRY (inner_editable);
    TEXT = gtk_entry_get_text (entry);

    lgw_addvocabularydialog_set_kanji (self, TEXT);
}


void
lgw_addvocabularydialog_reading_changed_cb (LgwAddVocabularyDialog *self,
                                            GtkEditable            *inner_editable)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_ENTRY (inner_editable));

    //Declarations
    const gchar *TEXT = NULL;
    GtkEntry *entry = NULL;

    //Initializations
    entry = GTK_ENTRY (inner_editable);
    TEXT = gtk_entry_get_text (entry);

    lgw_addvocabularydialog_set_reading (self, TEXT);
}


void
lgw_addvocabularydialog_definition_changed_cb (LgwAddVocabularyDialog *self,
                                               GtkTextBuffer          *inner_text_buffer)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_TEXT_BUFFER (inner_text_buffer));

    //Declarations
    gchar *text = NULL;
    GtkTextIter start;
    GtkTextIter end;

    //Initializations
    gtk_text_buffer_get_start_iter (inner_text_buffer, &start);
    gtk_text_buffer_get_end_iter (inner_text_buffer, &end);
    text = gtk_text_buffer_get_text (inner_text_buffer, &start, &end, FALSE);
    if (text == NULL) goto errored;

    lgw_addvocabularydialog_set_definition (self, text);

errored:

    if (text != NULL) g_free (text); text = NULL;
}


gboolean
lgw_addvocabularydialog_definition_focus_in_event_cb (LgwAddVocabularyDialog *self,
                                                      GdkEvent               *event,
                                                      GtkTextView            *inner_text_view)
{
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    g_return_if_fail (GTK_IS_TEXT_VIEW (inner_text_view));

    lgw_addvocabularydialog_sync_definition_text_buffer (self);

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

    return FALSE;
}


void
lgw_addvocabularydialog_response_cb (LgwAddVocabularyDialog *self,
                                     gint                    response_id,
                                     GtkDialog              *dialog)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    LgwVocabularyWordStore *vocabulary_word_store = NULL;
    LwVocabulary *vocabulary = NULL;
    LwWord *word = NULL;
    GList *wordlist = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_word_store = priv->data.store.vocabulary_word;
    if (vocabulary_word_store == NULL) goto errored;
    vocabulary = LW_VOCABULARY (vocabulary_word_store);
    if (vocabulary == NULL) goto errored;
    word = lgw_addvocabularydialog_steal_word (self);
    if (word == NULL) goto errored;
    wordlist = g_list_prepend (wordlist, word);
    if (wordlist == NULL) goto errored;

    if (response_id == LGW_ADDVOCABULARYDIALOG_RESPONSE_ADD)
    {
      lgw_vocabularywordstore_insert (vocabulary_word_store, NULL, wordlist);
    }

    if (priv->config.save_on_add)
    {
      lw_vocabulary_save (vocabulary, NULL);
    }

errored:

    if (wordlist != NULL) g_list_free (wordlist); wordlist = NULL;

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

