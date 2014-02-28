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
//! @file addvocabularydialog.c
//!
//! @brief To be written
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


static LgwAddVocabularyDialogClass *_klass = NULL;
static LgwAddVocabularyDialogClassPrivate *_klasspriv = NULL;


G_DEFINE_TYPE (LgwAddVocabularyDialog, lgw_addvocabularydialog, GTK_TYPE_DIALOG)

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_addvocabularydialog_new (GtkWindow *window)
{
  g_return_val_if_fail (window != NULL, NULL);
    //Declarations
    LgwAddVocabularyDialog *dialog = NULL;

    //Initializations
    dialog = LGW_ADDVOCABULARYDIALOG (g_object_new (LGW_TYPE_ADDVOCABULARYDIALOG, "transient-for", window, NULL));

    return GTK_WIDGET (dialog);
}


static void 
lgw_addvocabularydialog_init (LgwAddVocabularyDialog *self)
{
    self->priv = LGW_ADDVOCABULARYDIALOG_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwAddVocabularyDialogPrivate));

    self->priv->data.word = lw_word_new ();
}


static void
lgw_addvocabularydialog_dispose (GObject *object)
{
    //Declarations
    LgwAddVocabularyDialog *self = NULL;

    //Initializations
    self = LGW_ADDVOCABULARYDIALOG (object);

    lgw_addvocabularydialog_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_addvocabularydialog_parent_class)->dispose (object);
}


static void 
lgw_addvocabularydialog_finalize (GObject *object)
{
    LgwAddVocabularyDialog *self;
    LgwAddVocabularyDialogPrivate *priv;

    self = LGW_ADDVOCABULARYDIALOG (object);
    priv = self->priv;

    if (priv->data.word != NULL) lw_word_free (priv->data.word); priv->data.word = NULL;
    if (priv->data.store.vocabulary_list != NULL) g_object_unref (priv->data.store.vocabulary_list); priv->data.store.vocabulary_list = NULL;

    G_OBJECT_CLASS (lgw_addvocabularydialog_parent_class)->finalize (object);
}


static void
lgw_addvocabularydialog_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwAddVocabularyDialog *self = NULL;
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_ADDVOCABULARYDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_VOCABULARYLISTSTORE:
        lgw_addvocabularydialog_set_liststore (self, g_value_get_object (value));
        break;
      case PROP_VOCABULARYWORDSTORE:
        lgw_addvocabularydialog_set_wordstore (self, g_value_get_object (value));
        break;
      case PROP_KANJI:
        lgw_addvocabularydialog_set_kanji (self, g_value_get_string (value));
        break;
      case PROP_READING:
        lgw_addvocabularydialog_set_reading (self, g_value_get_string (value));
        break;
      case PROP_DEFINITION:
        lgw_addvocabularydialog_set_definition (self, g_value_get_string (value));
        break;
      case PROP_SAVE_ON_ADD:
        lgw_addvocabularydialog_set_save_on_add (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_addvocabularydialog_get_property (GObject      *object,
                                      guint         property_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwAddVocabularyDialog *self = NULL;
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_ADDVOCABULARYDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_VOCABULARYLISTSTORE:
        g_value_set_object (value, lgw_addvocabularydialog_get_liststore (self));
        break;
      case PROP_VOCABULARYWORDSTORE:
        g_value_set_object (value, lgw_addvocabularydialog_get_wordstore (self));
        break;
      case PROP_KANJI:
        g_value_set_string (value, lgw_addvocabularydialog_get_kanji (self));
        break;
      case PROP_READING:
        g_value_set_string (value, lgw_addvocabularydialog_get_reading (self));
        break;
      case PROP_DEFINITION:
        g_value_set_string (value, lgw_addvocabularydialog_get_definition (self));
        break;
      case PROP_SAVE_ON_ADD:
        g_value_set_boolean (value, lgw_addvocabularydialog_get_save_on_add (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_addvocabularydialog_constructed (GObject *object)
{
    //Declarations
    LgwAddVocabularyDialog *self;
    LgwAddVocabularyDialogPrivate *priv;
    GtkDialog *dialog = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_addvocabularydialog_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_ADDVOCABULARYDIALOG (object);
    priv = self->priv;
    dialog = GTK_DIALOG (self);

    {
      GtkBox *box = GTK_BOX (gtk_dialog_get_content_area (dialog));

      {
        GtkWidget *layout_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
        priv->ui.layout_box = GTK_BOX (layout_box);
        gtk_container_set_border_width (GTK_CONTAINER (layout_box), 6);
        gtk_box_pack_start (box, layout_box, TRUE, TRUE, 0);
        gtk_widget_show (layout_box);

        {
          {
            GtkWidget *image = NULL;
            GtkIconTheme *theme = gtk_icon_theme_get_default ();
            if (gtk_icon_theme_has_icon (theme, "list-add-symbolic"))
            {
              image = gtk_image_new_from_icon_name ("list-add-symbolic", GTK_ICON_SIZE_DIALOG);
            }
            else
            {
              image = gtk_image_new_from_icon_name ("add", GTK_ICON_SIZE_DIALOG);
            }
            if (image != NULL)
            {
              gtk_misc_set_alignment (GTK_MISC (image), .5, 0);
              gtk_box_pack_start (priv->ui.layout_box, image, FALSE, FALSE, 0);
              gtk_widget_show (image);
            }
          }

          {
            GtkWidget *grid = gtk_grid_new ();
            priv->ui.grid = GTK_GRID (grid);
            gtk_grid_set_row_spacing (priv->ui.grid, 6);
            gtk_grid_set_column_spacing (priv->ui.grid, 6);
            gtk_box_pack_start (priv->ui.layout_box, grid, TRUE, TRUE, 0);
            gtk_widget_show (grid);

            {
              {
                gchar *markup = g_markup_printf_escaped ("%s:", gettext("List"));
                GtkWidget *label = gtk_label_new (NULL);
                priv->ui.list_label = GTK_LABEL (label);
                gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
                gtk_misc_set_padding (GTK_MISC (label), 6, 6);
                gtk_label_set_markup (priv->ui.list_label, markup);
                gtk_grid_attach (priv->ui.grid, label, 0, 0, 1, 1);
                if (markup != NULL) g_free (markup); markup = NULL;
              }

              {
                GtkWidget *combo_box = gtk_combo_box_new_with_entry ();
                priv->ui.combo_box = GTK_COMBO_BOX (combo_box);
                gtk_combo_box_set_id_column (priv->ui.combo_box, LGW_VOCABULARYLISTSTORE_COLUMN_NAME);
                gtk_combo_box_set_entry_text_column (priv->ui.combo_box, LGW_VOCABULARYLISTSTORE_COLUMN_NAME);
                gtk_grid_attach (priv->ui.grid, combo_box, 1, 0, 1, 1);
              }
            }

            {
              {
                gchar *markup = g_markup_printf_escaped ("%s:", gettext("Word"));
                GtkWidget *label = gtk_label_new (NULL);
                gtk_misc_set_alignment (GTK_MISC (label), 1, 0);
                gtk_misc_set_padding (GTK_MISC (label), 6, 6);
                gtk_label_set_markup (GTK_LABEL (label), markup);
                gtk_grid_attach (priv->ui.grid, label, 0, 1, 1, 1);
                gtk_widget_show (label);
                if (markup != NULL) g_free (markup); markup = NULL;
              }

              {
                GtkWidget *entry = gtk_entry_new ();
                priv->ui.kanji_entry = GTK_ENTRY (entry);
                gtk_entry_set_placeholder_text (priv->ui.kanji_entry, gettext("Word"));
                gtk_entry_set_width_chars (priv->ui.kanji_entry, 20);
                gtk_grid_attach (priv->ui.grid, entry, 1, 1, 1, 1);
                gtk_widget_show (entry);
              }
            }

            {
              {
                gchar *markup = g_markup_printf_escaped ("%s:", gettext("Reading"));
                GtkWidget *label = gtk_label_new (NULL);
                gtk_misc_set_alignment (GTK_MISC (label), 1, 0);
                gtk_misc_set_padding (GTK_MISC (label), 6, 6);
                gtk_label_set_markup (GTK_LABEL (label), markup);
                gtk_grid_attach (priv->ui.grid, label, 0, 2, 1, 1);
                gtk_widget_show (label);
                if (markup != NULL) g_free (markup); markup = NULL;
              }

              {
                GtkWidget *entry = gtk_entry_new ();
                priv->ui.reading_entry = GTK_ENTRY (entry);
                gtk_entry_set_placeholder_text (priv->ui.reading_entry, gettext("Reading"));
                gtk_entry_set_width_chars (priv->ui.reading_entry, 20);
                gtk_grid_attach (priv->ui.grid, entry, 1, 2, 1, 1);
                gtk_widget_show (entry);
              }
            }

            {
              {
                gchar *markup = g_markup_printf_escaped ("%s:", gettext("Definition"));
                GtkWidget *label = gtk_label_new (NULL);
                gtk_misc_set_alignment (GTK_MISC (label), 1, 0);
                gtk_misc_set_padding (GTK_MISC (label), 6, 6);
                gtk_label_set_markup (GTK_LABEL (label), markup);
                gtk_grid_attach (priv->ui.grid, label, 0, 3, 1, 1);
                gtk_widget_show (label);
                if (markup != NULL) g_free (markup); markup = NULL;
              }

              {
                GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
                gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_ETCHED_IN);
                gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_NEVER);

                {
                  GtkTextBuffer *text_buffer = gtk_text_buffer_new (NULL);
                  gtk_text_buffer_set_text (text_buffer, gettext("Definition"), -1);
                  priv->data.definition.placeholder_text_buffer = text_buffer;
                }

                {
                  GtkTextBuffer *text_buffer = gtk_text_buffer_new (NULL);
                  priv->data.definition.text_buffer = text_buffer;

                  {
                    GtkWidget *text_view = gtk_text_view_new_with_buffer (text_buffer);
                    gtk_widget_set_size_request (text_view, 250, 50);
                    priv->ui.definition_text_view = GTK_TEXT_VIEW (text_view);
                    gtk_text_view_set_wrap_mode (priv->ui.definition_text_view, GTK_WRAP_WORD);
                    //gtk_entry_set_placeholder_text (priv->ui.definition_text_view, gettext("Definition"));
                    gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
                    gtk_container_set_border_width (GTK_CONTAINER (text_view), 4);
                    gtk_widget_show (text_view);
                  }
                }
                gtk_widget_show (scrolled_window);
                gtk_grid_attach (priv->ui.grid, scrolled_window, 1, 3, 1, 1);

              }
            }
          }
        }
      }
    }

    gtk_dialog_add_buttons (
      dialog,
      gettext("_Cancel"),
      LGW_ADDVOCABULARYDIALOG_RESPONSE_CANCEL,
      gettext("_Add"),
      LGW_ADDVOCABULARYDIALOG_RESPONSE_ADD,
      NULL
    );
    gtk_dialog_set_default_response (dialog, LGW_ADDVOCABULARYDIALOG_RESPONSE_ADD);

    gtk_window_set_title (GTK_WINDOW (dialog), gettext("Add New Vocabulary"));
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

    lgw_addvocabularydialog_connect_signals (self);
}


static void
lgw_addvocabularydialog_class_init (LgwAddVocabularyDialogClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_addvocabularydialog_constructed;
    object_class->set_property = lgw_addvocabularydialog_set_property;
    object_class->get_property = lgw_addvocabularydialog_get_property;
    object_class->finalize = lgw_addvocabularydialog_finalize;
    object_class->dispose = lgw_addvocabularydialog_dispose;

    g_type_class_add_private (object_class, sizeof(LgwAddVocabularyDialogPrivate));

    klass->priv = g_new0 (LgwAddVocabularyDialogClassPrivate, 1);

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
        "vocabulary-list-store",
        "prop",
        "object",
        LGW_TYPE_VOCABULARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

    _klasspriv->pspec[PROP_VOCABULARYWORDSTORE] = g_param_spec_object (
        "vocabulary-word-store",
        "prop",
        "object",
        LGW_TYPE_VOCABULARYWORDSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYWORDSTORE, _klasspriv->pspec[PROP_VOCABULARYWORDSTORE]);

    _klasspriv->pspec[PROP_KANJI] = g_param_spec_string (
        "kanji",
        "prop",
        "object",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_KANJI, _klasspriv->pspec[PROP_KANJI]);

    _klasspriv->pspec[PROP_READING] = g_param_spec_string (
        "reading",
        "prop",
        "object",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_READING, _klasspriv->pspec[PROP_READING]);

    _klasspriv->pspec[PROP_DEFINITION] = g_param_spec_string (
        "definition",
        "prop",
        "object",
        "",
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DEFINITION, _klasspriv->pspec[PROP_DEFINITION]);

    _klasspriv->pspec[PROP_SAVE_ON_ADD] = g_param_spec_boolean (
        "save-on-add",
        "prop",
        "object",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_SAVE_ON_ADD, _klasspriv->pspec[PROP_SAVE_ON_ADD]);
}

/*

static void
lgw_addvocabularydialog_init_accelerators (LgwAddVocabularyDialog *self)
{
    GtkWidget *widget;
    GtkAccelGroup *accelgroup;

    accelgroup = gw_window_get_accel_group (LGW_WINDOW (self));

    //Set menu accelerators
    widget = GTK_WIDGET (gw_window_get_object (LGW_WINDOW (self), "cancel_button"));
    gtk_widget_add_accelerator (GTK_WIDGET (widget), "activate", 
      accelgroup, (GDK_KEY_Escape), 0, GTK_ACCEL_VISIBLE);

    widget = GTK_WIDGET (gw_window_get_object (LGW_WINDOW (self), "add_button"));
    gtk_widget_add_accelerator (GTK_WIDGET (widget), "activate", 
      accelgroup, (GDK_KEY_ISO_Enter), 0, GTK_ACCEL_VISIBLE);
}


static void
lgw_addvocabularydialog_init_combobox (LgwAddVocabularyDialog *self)
{
    LgwAddVocabularyDialogPrivate *priv;
    LgwAddVocabularyDialogClass *klass;
    GwApplication *application;
    GtkListStore *store;
    GtkTreeModel *model;

    priv = self->priv;
    klass = LGW_ADDVOCABULARYDIALOG_CLASS (G_OBJECT_GET_CLASS (self));
    application = gw_window_get_application (LGW_WINDOW (self));
    store = gw_application_get_vocabularyliststore (application);
    model = GTK_TREE_MODEL (store);

    //Initialize the combobox
    gtk_combo_box_set_model (priv->vocabulary_list_combobox, model); 

    //Remove the default entry since it doesn't seem to be editable for some reason
    priv->list_entry = GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->vocabulary_list_combobox)));
    gtk_widget_destroy (GTK_WIDGET (priv->list_entry));

    //Add our entry
    priv->list_entry = GTK_ENTRY (gtk_entry_new ());
    gtk_entry_set_activates_default (priv->list_entry, TRUE);
    g_signal_connect (G_OBJECT (priv->list_entry), "changed", G_CALLBACK (gw_addvocabularydialog_list_changed_cb), self);
    gtk_widget_show (GTK_WIDGET (priv->list_entry));
    gtk_combo_box_set_entry_text_column (priv->vocabulary_list_combobox, LGW_VOCABULARYLISTSTORE_COLUMN_NAME);
    gtk_container_add (GTK_CONTAINER (priv->vocabulary_list_combobox), GTK_WIDGET (priv->list_entry));

    //Set the correct initial selection
    if (klass->last_selected_list_name != NULL)
    {
      gtk_entry_set_text (priv->list_entry, klass->last_selected_list_name);
    }
    else
    {
      gtk_combo_box_set_active (priv->vocabulary_list_combobox, 0);
    }
    gtk_editable_select_region (GTK_EDITABLE (priv->list_entry), 0, 0);
}
*/

const gchar*
lgw_addvocabularydialog_get_kanji (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self), NULL);

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return lw_word_get_kanji (priv->data.word);
}


void
lgw_addvocabularydialog_set_kanji (LgwAddVocabularyDialog *self,
                                   const gchar            *KANJI)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    if (KANJI == NULL) KANJI = "";

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    gchar *kanji = NULL;
    const gchar *ENTRY_TEXT = NULL;
    const gchar *INTERNAL_TEXT = NULL;

    //Initializations
    priv = self->priv;
    kanji = g_strdup (KANJI);
    if (kanji == NULL) goto errored;
    kanji = g_strstrip (kanji);
    ENTRY_TEXT = gtk_entry_get_text (priv->ui.kanji_entry);
    INTERNAL_TEXT = lw_word_get_kanji (priv->data.word);

    if (strcmp(ENTRY_TEXT, KANJI) != 0)
    {
      g_signal_handler_block (priv->ui.kanji_entry, priv->data.signalid[SIGNALID_KANJI_CHANGED]);
      gtk_entry_set_text (priv->ui.kanji_entry, KANJI);
      g_signal_handler_unblock (priv->ui.kanji_entry, priv->data.signalid[SIGNALID_KANJI_CHANGED]);
    }

    if (INTERNAL_TEXT == NULL || strcmp(INTERNAL_TEXT, kanji) != 0)
    {
      lw_word_set_kanji (priv->data.word, kanji);
      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_KANJI]);
    }

errored:

    if (kanji != NULL) g_free (kanji); kanji = NULL;
}


const gchar*
lgw_addvocabularydialog_get_reading (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self), NULL);

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return lw_word_get_reading (priv->data.word);
}


void
lgw_addvocabularydialog_set_reading (LgwAddVocabularyDialog *self,
                                     const gchar            *READING)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    if (READING == NULL) READING = "";


    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    gchar *reading = NULL;
    const gchar *ENTRY_TEXT = NULL;
    const gchar *INTERNAL_TEXT = NULL;

    //Initializations
    priv = self->priv;
    reading = g_strdup (READING);
    if (reading == NULL) goto errored;
    reading = g_strstrip (reading);
    ENTRY_TEXT = gtk_entry_get_text (priv->ui.reading_entry);
    INTERNAL_TEXT = lw_word_get_reading (priv->data.word);


    if (strcmp(ENTRY_TEXT, READING) != 0)
    {
      g_signal_handler_block (priv->ui.reading_entry, priv->data.signalid[SIGNALID_READING_CHANGED]);
      gtk_entry_set_text (priv->ui.reading_entry, READING);
      g_signal_handler_unblock (priv->ui.reading_entry, priv->data.signalid[SIGNALID_READING_CHANGED]);
    }

    if (INTERNAL_TEXT == NULL || strcmp(INTERNAL_TEXT, reading) != 0)
    {
      lw_word_set_reading (priv->data.word, reading);
      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_READING]);
    }

errored:

    if (reading != NULL) g_free (reading); reading = NULL;
}


const gchar*
lgw_addvocabularydialog_get_definition (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self), NULL);

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return lw_word_get_definition (priv->data.word);
}


void
lgw_addvocabularydialog_set_definition (LgwAddVocabularyDialog *self,
                                        const gchar            *DEFINITION)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));
    if (DEFINITION == NULL) DEFINITION = "";

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    GtkTextIter start;
    GtkTextIter end;
    gchar *text_buffer_text = NULL;
    gchar *definition = NULL;
    const gchar *INTERNAL_TEXT = NULL;

    //Initializations
    priv = self->priv;
    gtk_text_buffer_get_start_iter (priv->data.definition.text_buffer, &start);
    gtk_text_buffer_get_end_iter (priv->data.definition.text_buffer, &end);
    text_buffer_text = gtk_text_buffer_get_text (priv->data.definition.text_buffer, &start, &end, FALSE);
    if (text_buffer_text == NULL) goto errored;
    definition = g_strdup (DEFINITION);
    if (definition == NULL) goto errored;
    definition = g_strstrip (definition);
    INTERNAL_TEXT = lw_word_get_definition (priv->data.word);

    if (strcmp (text_buffer_text, DEFINITION) != 0)
    {
      g_signal_handler_block (priv->data.definition.text_buffer, priv->data.signalid[SIGNALID_DEFINITION_CHANGED]);
      gtk_text_buffer_set_text (priv->data.definition.text_buffer, DEFINITION, -1);
      g_signal_handler_unblock (priv->data.definition.text_buffer, priv->data.signalid[SIGNALID_DEFINITION_CHANGED]);
    }

    if (INTERNAL_TEXT == NULL || strcmp(INTERNAL_TEXT, definition) != 0)
    { 
      lw_word_set_definition (priv->data.word, definition);
      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DEFINITION]);
    }

errored:

    if (text_buffer_text != NULL) g_free (text_buffer_text); text_buffer_text = NULL;
    if (definition != NULL) g_free (definition); definition = NULL;
}


LgwVocabularyListStore*
lgw_addvocabularydialog_get_liststore (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self), NULL);

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.store.vocabulary_list;
}


void
lgw_addvocabularydialog_set_liststore (LgwAddVocabularyDialog *self,
                                       LgwVocabularyListStore *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (vocabulary_list_store != priv->data.store.vocabulary_list);
    if (!changed) goto errored;

    if (vocabulary_list_store != NULL)
    {
      g_object_ref (vocabulary_list_store);
    }

    if (priv->data.store.vocabulary_list != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.store.vocabulary_list),
        (gpointer*) &(priv->data.store.vocabulary_list)
      );
      g_object_unref (priv->data.store.vocabulary_list);
      priv->data.store.vocabulary_list = NULL;
    }

    if (vocabulary_list_store != NULL)
    {
      priv->data.store.vocabulary_list = vocabulary_list_store;
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.store.vocabulary_list),
        (gpointer*) &(priv->data.store.vocabulary_list)
      );

      gtk_combo_box_set_model (priv->ui.combo_box, GTK_TREE_MODEL (priv->data.store.vocabulary_list));
      gtk_widget_show (GTK_WIDGET (priv->ui.combo_box));
      gtk_widget_show (GTK_WIDGET (priv->ui.list_label));
    }
    else
    {
      gtk_combo_box_set_model (priv->ui.combo_box, NULL);
      gtk_widget_hide (GTK_WIDGET (priv->ui.combo_box));
      gtk_widget_hide (GTK_WIDGET (priv->ui.list_label));
    }

    lgw_addvocabularydialog_sync_list_store (self);
    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

errored:
  
    return;
}


LgwVocabularyWordStore*
lgw_addvocabularydialog_get_wordstore (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self), NULL);

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.store.vocabulary_word;
}


void
lgw_addvocabularydialog_set_wordstore (LgwAddVocabularyDialog *self,
                                       LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (vocabulary_word_store != priv->data.store.vocabulary_word);
    if (!changed) goto errored;

    if (vocabulary_word_store != NULL)
    {
      g_object_ref (vocabulary_word_store);
    }

    if (priv->data.store.vocabulary_word != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.store.vocabulary_word), (gpointer*) &(priv->data.store.vocabulary_word));
      g_object_unref (priv->data.store.vocabulary_word);
      priv->data.store.vocabulary_word = NULL;
    }

    if (vocabulary_word_store != NULL)
    {
      priv->data.store.vocabulary_word = vocabulary_word_store;
      g_object_add_weak_pointer (G_OBJECT (priv->data.store.vocabulary_word), (gpointer*) &(priv->data.store.vocabulary_word));

    }

    lgw_addvocabularydialog_sync_list_store (self);
    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYWORDSTORE]);

errored:
  
    return;
}


void
lgw_addvocabularydialog_select_wordstore (LgwAddVocabularyDialog *self, 
                                          LgwVocabularyWordStore *vocabulary_word_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    GtkTreePath *tree_path = NULL;
    gint index = -1;

    //Initializations
    priv = self->priv;

    if (vocabulary_word_store != NULL && priv->data.store.vocabulary_list != NULL)
    {
      tree_path = lgw_vocabularyliststore_find_by_wordstore (priv->data.store.vocabulary_list, vocabulary_word_store);
      if (tree_path != NULL)
      {
        index = gtk_tree_path_get_indices (tree_path)[0];
      }
    }

    gtk_combo_box_set_active (priv->ui.combo_box, index);

errored:

    if (tree_path != NULL) gtk_tree_path_free (tree_path); tree_path = NULL;
}


gboolean
lgw_addvocabularydialog_get_save_on_add (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.save_on_add;
}


void
lgw_addvocabularydialog_set_save_on_add (LgwAddVocabularyDialog *self,
                                         gboolean                save_on_add)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (save_on_add != priv->config.save_on_add);
    if (!changed) goto errored;

    priv->config.save_on_add = save_on_add;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_SAVE_ON_ADD]);

errored:

    return;
}


void
lgw_addvocabularydialog_sync_definition_text_buffer (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;
    gboolean has_text = FALSE;

    //Initializations
    priv = self->priv;
    widget = GTK_WIDGET (priv->ui.definition_text_view);
    has_focus = gtk_widget_is_focus (widget);
    has_text = (gtk_text_buffer_get_char_count (priv->data.definition.text_buffer) > 0);

    gtk_text_view_set_buffer (priv->ui.definition_text_view, priv->data.definition.text_buffer);
/*
    if (has_focus || has_text)
    {
      printf("BREAK setting primary buffer %d %d\n", has_focus, has_text);
      gtk_text_view_set_buffer (priv->ui.definition_text_view, priv->data.definition.text_buffer);
      gtk_text_view_set_editable (priv->ui.definition_text_view, TRUE);
      gtk_text_view_set_cursor_visible (priv->ui.definition_text_view, FALSE);
    }
    else
    {
      printf("BREAK setting placeholder buffer %d %d\n", has_focus, has_text);
      gtk_text_view_set_buffer (priv->ui.definition_text_view, priv->data.definition.placeholder_text_buffer);
      gtk_text_view_set_editable (priv->ui.definition_text_view, FALSE);
      gtk_text_view_set_cursor_visible (priv->ui.definition_text_view, TRUE);
    }
*/
}


void
lgw_addvocabularydialog_sync_list_store (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    lgw_addvocabularydialog_select_wordstore (self, priv->data.store.vocabulary_word);
}


LwWord*
lgw_addvocabularydialog_steal_word (LgwAddVocabularyDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_ADDVOCABULARYDIALOG (self));

    //Declarations
    LgwAddVocabularyDialogPrivate *priv = NULL;
    LwWord *word = NULL;

    //Initializations
    priv = self->priv;
    word = priv->data.word;
    priv->data.word = lw_word_new ();

    return word;
}
                             


/*
gboolean
lgw_addvocabularydialog_validate (LgwAddVocabularyDialog *self)
{
    LgwAddVocabularyDialogPrivate *priv;
    const gchar *kanji, *furigana, *definitions, *list;
    gboolean has_kanji, has_furigana, has_definitions, has_list;
    gboolean valid;

    priv = self->priv;
    kanji = gw_addvocabularydialog_get_kanji (self);
    furigana = gw_addvocabularydialog_get_furigana (self);
    definitions = gw_addvocabularydialog_get_definitions (self);
    list = gw_addvocabularydialog_get_list (self);

    has_kanji = (strlen (kanji) > 0);
    has_furigana = (strlen (furigana) > 0);
    has_definitions = (strlen (definitions) > 0);
    has_list = (strlen (list) > 0);
    valid = ((has_kanji || has_furigana || has_definitions) && has_list);

    gtk_widget_set_sensitive (GTK_WIDGET (priv->add_button), valid);

    return valid;
}


void
lgw_addvocabularydialog_focus_add_button (LgwAddVocabularyDialog *self)
{
    if (gw_addvocabularydialog_validate (self))
      gtk_widget_grab_focus (GTK_WIDGET (self->priv->add_button));
}


gboolean
lgw_addvocabularydialog_get_iter (LgwAddVocabularyDialog *self, GtkTreeIter *iter)
{
    g_assert (iter != NULL);

    *iter = self->priv->iter;

    return self->priv->valid;
}


void
lgw_addvocabularydialog_save (LgwAddVocabularyDialog *self)
{
   if (self->priv->wordstore != NULL)
   {
     gw_vocabularywordstore_save (self->priv->wordstore, NULL);
   }
}


void
lgw_addvocabularydialog_set_focus (LgwAddVocabularyDialog *self, LgwAddVocabularyDialogFocus focus)
{
    LgwAddVocabularyDialogPrivate *priv;

    priv = self->priv;

    switch (focus)
    {
      case LGW_ADDVOCABULARYDIALOG_FOCUS_LIST:
        gtk_widget_grab_focus (GTK_WIDGET (priv->list_entry));
        break;
      case LGW_ADDVOCABULARYDIALOG_FOCUS_KANJI:
        gtk_widget_grab_focus (GTK_WIDGET (priv->kanji_entry));
        break;
      case LGW_ADDVOCABULARYDIALOG_FOCUS_FURIGANA:
        gtk_widget_grab_focus (GTK_WIDGET (priv->furigana_entry));
        break;
      case LGW_ADDVOCABULARYDIALOG_FOCUS_DEFINITIONS:
        gtk_widget_grab_focus (GTK_WIDGET (priv->definitions_textview));
        break;
      case LGW_ADDVOCABULARYDIALOG_FOCUS_ADD_BUTTON:
        gtk_widget_grab_focus (GTK_WIDGET (priv->add_button));
        break;
      default:
        g_assert_not_reached ();
        break;
    }
}
*/
