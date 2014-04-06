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
//! @file deletevocabularylistdialog.c
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

#include <libgwaei/deletevocabularylistdialog.h>
#include <libgwaei/deletevocabularylistdialog-private.h>


static LgwDeleteVocabularyListDialogClass *_klass = NULL;
static LgwDeleteVocabularyListDialogClassPrivate *_klasspriv = NULL;


G_DEFINE_TYPE (LgwDeleteVocabularyListDialog, lgw_deletevocabularylistdialog, GTK_TYPE_DIALOG)

//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_deletevocabularylistdialog_new (GtkWindow              *window,
                                    LgwVocabularyListStore *vocabulary_list_store)
{
  g_return_val_if_fail (window != NULL, NULL);
    //Declarations
    LgwDeleteVocabularyListDialog *dialog = NULL;

    //Initializations
    dialog = LGW_DELETEVOCABULARYLISTDIALOG (g_object_new (LGW_TYPE_DELETEVOCABULARYLISTDIALOG, "transient-for", window, "vocabulary-list-store", vocabulary_list_store, NULL));

    return GTK_WIDGET (dialog);
}


static void 
lgw_deletevocabularylistdialog_init (LgwDeleteVocabularyListDialog *self)
{
    self->priv = LGW_DELETEVOCABULARYLISTDIALOG_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LgwDeleteVocabularyListDialogPrivate));

    self->priv->data.index = g_hash_table_new (g_direct_hash, g_direct_equal);
}


static void
lgw_deletevocabularylistdialog_dispose (GObject *object)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);

    lgw_deletevocabularylistdialog_disconnect_signals (self);

    G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->dispose (object);
}


static void 
lgw_deletevocabularylistdialog_finalize (GObject *object)
{
    LgwDeleteVocabularyListDialog *self;
    LgwDeleteVocabularyListDialogPrivate *priv;

    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    if (priv->data.vocabulary_list_store != NULL) g_object_unref (priv->data.vocabulary_list_store); priv->data.vocabulary_list_store = NULL;
    if (priv->data.index != NULL) g_hash_table_unref (priv->data.index); priv->data.index = NULL;

    G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->finalize (object);
}


static void
lgw_deletevocabularylistdialog_set_property (GObject      *object,
                                             guint         property_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_WORDSTORES:
        lgw_deletevocabularylistdialog_set_wordstores (self, g_value_get_pointer (value));
        break;
      case PROP_VOCABULARYLISTSTORE:
        lgw_deletevocabularylistdialog_set_liststore (self, g_value_get_object (value));
        break;
      case PROP_DELETE_ON_RESPONSE:
        lgw_deletevocabularylistdialog_set_delete_on_response (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_deletevocabularylistdialog_get_property (GObject      *object,
                                      guint         property_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self = NULL;
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_WORDSTORES:
        g_value_set_pointer (value, lgw_deletevocabularylistdialog_get_wordstores (self));
        break;
      case PROP_VOCABULARYLISTSTORE:
        g_value_set_object (value, lgw_deletevocabularylistdialog_get_liststore (self));
        break;
      case PROP_DELETE_ON_RESPONSE:
        g_value_set_boolean (value, lgw_deletevocabularylistdialog_get_delete_on_response (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_deletevocabularylistdialog_constructed (GObject *object)
{
    //Declarations
    LgwDeleteVocabularyListDialog *self;
    LgwDeleteVocabularyListDialogPrivate *priv;
    GtkDialog *dialog = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_deletevocabularylistdialog_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_DELETEVOCABULARYLISTDIALOG (object);
    priv = self->priv;
    dialog = GTK_DIALOG (self);

    {
      GtkBox *box = GTK_BOX (gtk_dialog_get_content_area (dialog));

      {
        GtkWidget *layout_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
        priv->ui.layout_box = GTK_BOX (layout_box);
        gtk_container_set_border_width (GTK_CONTAINER (layout_box), 6);
        gtk_widget_set_margin_top (layout_box, 6);
        gtk_box_pack_start (box, layout_box, TRUE, TRUE, 0);
        gtk_widget_show (layout_box);

        {
          {
            GtkWidget *image = NULL;
            GtkIconTheme *theme = gtk_icon_theme_get_default ();
            if (gtk_icon_theme_has_icon (theme, "edit-delete"))
            {
              image = gtk_image_new_from_icon_name ("edit-delete-symbolic", GTK_ICON_SIZE_DIALOG);
            }
            else
            {
              image = gtk_image_new_from_icon_name ("edit-delete-symbolic", GTK_ICON_SIZE_DIALOG);
            }
            if (image != NULL)
            {
              gtk_misc_set_alignment (GTK_MISC (image), 0.0, 0.0);
              gtk_box_pack_start (priv->ui.layout_box, image, FALSE, FALSE, 0);
              gtk_widget_show (image);
            }
          }

          {
            GtkWidget *grid = gtk_grid_new ();
            priv->ui.grid = GTK_GRID (grid);
            gtk_grid_set_row_spacing (priv->ui.grid, 8);
            gtk_grid_set_column_spacing (priv->ui.grid, 6);
            gtk_widget_set_margin_bottom (grid, 6);
            gtk_box_pack_start (priv->ui.layout_box, grid, TRUE, TRUE, 0);
            gtk_widget_show (grid);
          }

          {
            GtkWidget *label = gtk_label_new (NULL);
            priv->ui.primary_label = GTK_LABEL (label);
            gtk_label_set_text (priv->ui.primary_label, "Delete?");
            gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
            gtk_grid_attach (priv->ui.grid, label, 0, 0, 1, 1);
            gtk_widget_show (label);
          }

          {
            GtkWidget *label = gtk_label_new (NULL);
            priv->ui.secondary_label = GTK_LABEL (label);
            gtk_label_set_text (priv->ui.secondary_label, "These items will be perminently deleted.");
            gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
            gtk_grid_attach (priv->ui.grid, label, 0, 1, 1, 1);
            gtk_widget_show (label);
          }

          {
            GtkWidget *label = gtk_label_new (NULL);
            priv->ui.list_label = GTK_LABEL (label);
            gtk_label_set_text (priv->ui.list_label, "LIST");
            gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
            gtk_widget_set_margin_left (GTK_WIDGET (label), 6);
            gtk_grid_attach (priv->ui.grid, label, 0, 2, 1, 1);
            gtk_widget_show (label);
          }
        }
      }
    }

    gtk_dialog_add_buttons (
      dialog,
      gettext("_Cancel"),
      LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL,
      gettext("_Delete"),
      LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE,
      NULL
    );
    gtk_dialog_set_default_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL);
    priv->ui.delete_button = GTK_BUTTON (gtk_dialog_get_widget_for_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_DELETE));
    priv->ui.cancel_button = GTK_BUTTON (gtk_dialog_get_widget_for_response (dialog, LGW_DELETEVOCABULARYLISTDIALOG_RESPONSE_CANCEL));

    gtk_window_set_title (GTK_WINDOW (dialog), "");
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

    lgw_deletevocabularylistdialog_connect_signals (self);

    lgw_deletevocabularylistdialog_sync_labels (self);
}


static void
lgw_deletevocabularylistdialog_class_init (LgwDeleteVocabularyListDialogClass *klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = lgw_deletevocabularylistdialog_constructed;
    object_class->set_property = lgw_deletevocabularylistdialog_set_property;
    object_class->get_property = lgw_deletevocabularylistdialog_get_property;
    object_class->finalize = lgw_deletevocabularylistdialog_finalize;
    object_class->dispose = lgw_deletevocabularylistdialog_dispose;

    g_type_class_add_private (object_class, sizeof(LgwDeleteVocabularyListDialogPrivate));

    klass->priv = g_new0 (LgwDeleteVocabularyListDialogClassPrivate, 1);

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_WORDSTORES] = g_param_spec_pointer (
        "tree-paths",
        "prop",
        "object",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_WORDSTORES, _klasspriv->pspec[PROP_WORDSTORES]);

    _klasspriv->pspec[PROP_VOCABULARYLISTSTORE] = g_param_spec_object (
        "vocabulary-list-store",
        "prop",
        "object",
        LGW_TYPE_VOCABULARYLISTSTORE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_VOCABULARYLISTSTORE, _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);

    _klasspriv->pspec[PROP_DELETE_ON_RESPONSE] = g_param_spec_boolean (
        "delete-on-response",
        "prop",
        "object",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DELETE_ON_RESPONSE, _klasspriv->pspec[PROP_DELETE_ON_RESPONSE]);
}


LgwVocabularyListStore*
lgw_deletevocabularylistdialog_get_liststore (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self), NULL);

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.vocabulary_list_store;
}


void
lgw_deletevocabularylistdialog_set_liststore (LgwDeleteVocabularyListDialog *self,
                                              LgwVocabularyListStore        *vocabulary_list_store)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
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

    priv->data.vocabulary_list_store = vocabulary_list_store;

    lgw_deletevocabularylistdialog_set_wordstores (self, priv->data.wordstores);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_VOCABULARYLISTSTORE]);
    lgw_deletevocabularylistdialog_sync_labels (self);

errored:
  
    return;
}


gboolean
lgw_deletevocabularylistdialog_get_delete_on_response (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.delete_on_response;
}


void
lgw_deletevocabularylistdialog_set_delete_on_response (LgwDeleteVocabularyListDialog *self,
                                                       gboolean                       delete_on_response)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (delete_on_response != priv->config.delete_on_response);
    if (!changed) goto errored;

    priv->config.delete_on_response = delete_on_response;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DELETE_ON_RESPONSE]);

errored:

    return;
}

GList*
lgw_deletevocabularylistdialog_get_wordstores (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;

    return priv->data.wordstores;
}


gboolean
_internal_wordlist_equals (LgwDeleteVocabularyListDialog *self,
                           GList                         *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean equals = TRUE;

    //Initializations
    priv = self->priv;

    if ((priv->data.wordstores == NULL && wordstores != NULL) || (priv->data.wordstores != NULL && wordstores == NULL))
    {
      equals = FALSE;
      goto errored;
    }

    if (g_list_length (wordstores) != g_list_length (priv->data.wordstores))
    {
      equals = FALSE;
      goto errored;
    }

    {
      GList *link = NULL;
      for (link = wordstores; link != NULL; link = link->next)
      {
        if (!g_hash_table_contains (priv->data.index, link->data)) 
        {
          equals = FALSE;
          goto errored;
        }
      }
    }

errored:

    return equals;
}


void
_rebuild_index (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;

    g_hash_table_remove_all (priv->data.index);

    {
      GList *link = NULL;
      for (link = priv->data.wordstores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *w = LGW_VOCABULARYWORDSTORE (link->data);
        g_hash_table_add (priv->data.index, w);
      }
    }
}


void
lgw_deletevocabularylistdialog_set_wordstores_by_treepaths (LgwDeleteVocabularyListDialog *self,
                                                            GList                         *treepaths)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    GList *wordstores = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_list_store = priv->data.vocabulary_list_store;
    if (vocabulary_list_store == NULL) goto errored;
    wordstores = lgw_vocabularyliststore_get_wordstores (priv->data.vocabulary_list_store, treepaths);

    lgw_deletevocabularylistdialog_set_wordstores (self, wordstores);

errored:

    return;
}


void
lgw_deletevocabularylistdialog_set_wordstores (LgwDeleteVocabularyListDialog *self,
                                               GList                         *wordstores)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    LgwVocabularyListStore *vocabulary_list_store = NULL;
    gboolean changed = FALSE;
    GList *sanitized_wordstores = NULL;

    //Initializations
    priv = self->priv;
    vocabulary_list_store = priv->data.vocabulary_list_store;
    changed = !_internal_wordlist_equals (self, wordstores);
    if (!changed) goto errored;

    if (wordstores != NULL)
    {
      GList *link = NULL;
      for (link = wordstores; link != NULL; link = link->next)
      {
        LgwVocabularyWordStore *w = LGW_VOCABULARYWORDSTORE (link->data);
        if (vocabulary_list_store != NULL && lgw_vocabularyliststore_contains_wordstore (vocabulary_list_store, w)) 
        {
          g_object_ref (G_OBJECT (w));
          printf("BREAK lgw_deletevocabularylistdialog_set_wordstores refs %s %d\n", lw_vocabulary_get_filename (LW_VOCABULARY (w)), G_OBJECT (w)->ref_count);
          sanitized_wordstores = g_list_prepend (sanitized_wordstores, w);
        }
      }
      sanitized_wordstores = g_list_reverse (sanitized_wordstores);
    }

    if (priv->data.wordstores != NULL)
    {
      g_list_free_full (priv->data.wordstores, (GDestroyNotify) g_object_unref);
      priv->data.wordstores = NULL;
    }

    priv->data.wordstores = sanitized_wordstores;

    _rebuild_index (self);

    lgw_deletevocabularylistdialog_sync_labels (self);
    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_WORDSTORES]);

errored:

    return;
}


gchar*
_get_filenames (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gchar **filenames = NULL;
    gint length = -1;
    gchar *joined = NULL;

    //Initializations
    priv = self->priv;
    length = g_list_length (priv->data.wordstores);
    filenames = g_new0 (gchar*, length + 1);

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.wordstores; link != NULL; link = link->next)
      {
        LwVocabulary *v  = LW_VOCABULARY (link->data);
        filenames[i++] = (gchar*) lw_vocabulary_get_filename (v);
      }

      filenames[i++] = NULL;
    }

    joined = g_strjoinv ("\n", filenames);

errored:

    if (filenames != NULL) g_free (filenames); filenames = NULL;

    return joined;
}


void
lgw_deletevocabularylistdialog_sync_labels (LgwDeleteVocabularyListDialog *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;

    if (priv->ui.primary_label != NULL)
    {
      gchar *markup = g_markup_printf_escaped ("<b><big>%s</big></b>", "Are you sure you want to delete these vocabulary lists?");
      if (markup != NULL)
      {
        gtk_label_set_markup (priv->ui.primary_label, markup);
        g_free (markup); markup = NULL;
      }
    }

    if (priv->ui.secondary_label != NULL)
    {
      gchar *markup = g_markup_printf_escaped ("%s", "The following vocabulary lists will be perminently deleted:");
      if (markup != NULL)
      {
        gtk_label_set_markup (priv->ui.secondary_label, markup);
        g_free (markup); markup = NULL;
      }
    }

    if (priv->ui.list_label != NULL)
    {
      gchar *filenames = _get_filenames (self);
      gchar *markup = g_markup_printf_escaped ("%s", filenames);
      if (markup != NULL)
      {
        gtk_label_set_markup (priv->ui.list_label, markup);
        g_free (markup); markup = NULL;
      }
      if (filenames != NULL) g_free (filenames); filenames = NULL;
    }
}


gboolean
lgw_deletevocabularylistdialog_validate (LgwDeleteVocabularyListDialog *self)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (LGW_IS_DELETEVOCABULARYLISTDIALOG (self));

    //Declarations
    LgwDeleteVocabularyListDialogPrivate *priv = NULL;
    gboolean changed = FALSE;

    gtk_widget_set_sensitive ();

    return FALSE;
    */
}


