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
lgw_addvocabularydialog_new (LgwVocabularyListStore *vocabulary_list_store)
{
    g_return_val_if_fail (vocabulary_list_store != NULL, NULL);

    //Declarations
    LgwAddVocabularyDialog *dialog = NULL;

    //Initializations
    dialog = LGW_ADDVOCABULARYDIALOG (g_object_new (
      LGW_TYPE_ADDVOCABULARYDIALOG,
      "vocabulary-list-store", vocabulary_list_store,
      NULL
    ));

    return GTK_WIDGET (dialog);
}


static void 
lgw_addvocabularydialog_init (LgwAddVocabularyDialog *self)
{
    self->priv = LGW_ADDVOCABULARYDIALOG_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwAddVocabularyDialogPrivate));
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
    if (priv->data.vocabulary_list_store != NULL) g_object_unref (priv->data.vocabulary_list_store); priv->data.vocabulary_list_store = NULL;

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
      case PROP_KANJI:
        lgw_addvocabularydialog_set_kanji (self, g_value_get_string (value));
        break;
      case PROP_READING:
        lgw_addvocabularydialog_set_reading (self, g_value_get_string (value));
        break;
      case PROP_DEFINITION:
        lgw_addvocabularydialog_set_definition (self, g_value_get_string (value));
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
      case PROP_KANJI:
        g_value_set_string (value, lgw_addvocabularydialog_get_kanji (self));
        break;
      case PROP_READING:
        g_value_set_string (value, lgw_addvocabularydialog_get_reading (self));
        break;
      case PROP_DEFINITION:
        g_value_set_string (value, lgw_addvocabularydialog_get_definition (self));
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

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_addvocabularydialog_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_ADDVOCABULARYDIALOG (object);
    priv = self->priv;

/*
    //Set up the gtkbuilder links
    priv->kanji_entry = GTK_ENTRY (lgw_window_get_object (LGW_WINDOW (self), "kanji_entry"));
    priv->furigana_entry = GTK_ENTRY (lgw_window_get_object (LGW_WINDOW (self), "furigana_entry"));
    priv->definitions_textview = GTK_TEXT_VIEW (gw_window_get_object (LGW_WINDOW (self), "definitions_textview"));
    priv->cancel_button = GTK_BUTTON (gw_window_get_object (LGW_WINDOW (self), "cancel_button"));
    priv->add_button = GTK_BUTTON (gw_window_get_object (LGW_WINDOW (self), "add_button"));
    priv->vocabulary_list_combobox = GTK_COMBO_BOX (gw_window_get_object (LGW_WINDOW (self), "vocabulary_list_combobox"));
*/

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

    _klasspriv->signalid[CLASS_SIGNALID_WORD_ADDED] = g_signal_new (
        "word-added",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LgwAddVocabularyDialogClass, word_added),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE, 0
    );

    _klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
        "vocabulary-list-store",
        "prop",
        "object",
        LGW_TYPE_VOCABULARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);


    _klasspriv->pspec[PROP_KANJI] = g_param_spec_string (
        "word",
        "prop",
        "object",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_KANJI, _klasspriv->pspec[PROP_KANJI]);

    _klasspriv->pspec[PROP_READING] = g_param_spec_string (
        "reading",
        "prop",
        "object",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_READING, _klasspriv->pspec[PROP_READING]);

    _klasspriv->pspec[PROP_DEFINITION] = g_param_spec_string (
        "definition",
        "prop",
        "object",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DEFINITION, _klasspriv->pspec[PROP_DEFINITION]);
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (
      KANJI != lw_word_get_reading (priv->data.word) &&
      strcmp(KANJI, lw_word_get_reading (priv->data.word)) != 0
    );

    if (changed)
    {
      lw_word_set_kanji (priv->data.word, KANJI);
      gtk_entry_set_text (priv->ui.kanji_entry, KANJI);

      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_KANJI]);
    }
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (
      READING != lw_word_get_reading (priv->data.word) &&
      strcmp(READING, lw_word_get_reading (priv->data.word)) != 0
    );

    if (changed)
    {
      lw_word_set_reading (priv->data.word, READING);
      gtk_entry_set_text (priv->ui.reading_entry, READING);

      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_READING]);
    }
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (
      DEFINITION != lw_word_get_reading (priv->data.word) &&
      strcmp(DEFINITION, lw_word_get_reading (priv->data.word)) != 0
    );

    if (changed)
    {
      lw_word_set_definition (priv->data.word, DEFINITION);
      gtk_entry_set_text (priv->ui.definition_entry, DEFINITION);

      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DEFINITION]);
    }
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

    return priv->data.vocabulary_list_store;
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
    changed = (vocabulary_list_store != priv->data.vocabulary_list_store);
    if (!changed) goto errored;

    if (vocabulary_list_store != NULL)
    {
      g_object_ref (vocabulary_list_store);
    }

    if (priv->data.vocabulary_list_store != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.vocabulary_list_store),
        (gpointer*) &(priv->data.vocabulary_list_store)
      );
      g_object_unref (priv->data.vocabulary_list_store);
      priv->data.vocabulary_list_store = NULL;
    }

    if (vocabulary_list_store != NULL)
    {
      priv->data.vocabulary_list_store = vocabulary_list_store;
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.vocabulary_list_store),
        (gpointer*) &(priv->data.vocabulary_list_store)
      );

      gtk_combo_box_set_model (priv->ui.combobox, GTK_TREE_MODEL (priv->data.vocabulary_list_store));
      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
    }
    else
    {
      gtk_combo_box_set_model (priv->ui.combobox, NULL);
      g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
    }

errored:
  
    return;
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
