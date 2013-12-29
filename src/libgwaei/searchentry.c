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
//! @file searchentry.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchentry-private.h>

static void lgw_searchentry_init_actionable_interface (LgwActionableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LgwSearchEntry, lgw_searchentry, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_searchentry_init_actionable_interface));



//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_searchentry_new ()
{
    //Declarations
    LgwSearchEntry *entry = NULL;

    //Initializations
    entry = LGW_SEARCHENTRY (g_object_new (LGW_TYPE_SEARCHENTRY, NULL));

    return GTK_WIDGET (entry);
}


static void 
lgw_searchentry_init (LgwSearchEntry *entry)
{
    entry->priv = LGW_SEARCHENTRY_GET_PRIVATE (entry);
    memset(entry->priv, 0, sizeof(LgwSearchEntryPrivate));

    LgwSearchEntryPrivate *priv;
    priv = entry->priv;
}


static void
lgw_searchentry_init_actionable_interface (LgwActionableInterface *iface)
{
    iface->get_actions = lgw_searchentry_get_actions;
    iface->set_actiongroup = lgw_searchentry_set_actiongroup;
}


static void 
lgw_searchentry_finalize (GObject *object)
{
    //Declarations
    LgwSearchEntry *entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    entry = LGW_SEARCHENTRY (object);
    priv = entry->priv;

    G_OBJECT_CLASS (lgw_searchentry_parent_class)->finalize (object);

}


static void
lgw_searchentry_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (object);
    actionable = LGW_ACTIONABLE (object);
    priv = search_entry->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        lgw_actionable_set_actiongroup (actionable, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_searchentry_get_property (GObject      *object,
                              guint         property_id,
                              GValue       *value,
                              GParamSpec   *pspec)
{
    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (object);
    actionable = LGW_ACTIONABLE (object);
    priv = search_entry->priv;

    switch (property_id)
    {
      case PROP_ACTIONS:
        g_value_set_pointer (value, lgw_actionable_get_actions (actionable));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}



static void 
lgw_searchentry_constructed (GObject *object)
{
    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_searchentry_parent_class)->constructed (object);
    }

    //Initializations
    search_entry = LGW_SEARCHENTRY (object);
    priv = search_entry->priv;
    priv->ui.box = GTK_BOX (search_entry);
    widget = GTK_WIDGET (object);

    {
        GtkWidget *search_entry = gtk_search_entry_new ();
        priv->ui.search_entry = GTK_SEARCH_ENTRY (search_entry);
        gtk_box_pack_start (priv->ui.box, search_entry, FALSE, FALSE, 0);
        gtk_widget_set_size_request (search_entry, 300, -1);
        gtk_widget_show (search_entry);
    }

    lgw_searchentry_connect_signals (search_entry);
}


static void
lgw_searchentry_dispose (GObject *object)
{
    //Declarations
    LgwSearchEntry *entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    entry = LGW_SEARCHENTRY (object);
    priv = entry->priv;

    lgw_searchentry_disconnect_signals (entry);

    if (priv->data.action_group != NULL)
    {
      lgw_actiongroup_free (priv->data.action_group);
      priv->data.action_group = NULL;
    }

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    G_OBJECT_CLASS (lgw_searchentry_parent_class)->dispose (object);
}


static void
lgw_searchentry_class_init (LgwSearchEntryClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_searchentry_set_property;
    object_class->get_property = lgw_searchentry_get_property;
    object_class->constructed = lgw_searchentry_constructed;
    object_class->finalize = lgw_searchentry_finalize;
    object_class->dispose = lgw_searchentry_dispose;

    g_type_class_add_private (object_class, sizeof (LgwSearchEntryPrivate));

    klass->priv = g_new0 (LgwSearchEntryClassPrivate, 1);

    g_object_class_override_property (object_class, PROP_ACTIONS, "actions");
}


void
lgw_searchentry_set_search_as_you_typea (LgwSearchEntry *entry,
                                         gboolean        mode)
{
    /*
    //Sanity checks
    g_return_if_fail (entry != NULL);

    //Declarations
    LgwSearchEntryClass *klass = NULL;
    LgwSearchEntryClassPrivate *klasspriv = NULL;

    //Initializations
    klass = LGW_SEARCHENTRY_GET_CLASS (entry);
    klasspriv = klass->priv;
    */
}


void
lgw_searchentry_set_dictionarylistbox (LgwSearchEntry       *search_entry,
                                       LgwDictionaryListBox *dictionary_list_box)
{

}


void
lgw_searchentry_set_resultsview (LgwSearchEntry *search_entry,
                                 LgwResultsView *results_view)
{
}


GtkEntry*
lgw_searchentry_get_entry (LgwSearchEntry *search_entry)
{
    //Sanity checks
    g_return_val_if_fail (search_entry != NULL, NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    priv = search_entry->priv;

    return GTK_ENTRY (priv->ui.search_entry);
}


static void
lgw_searchentry_set_actiongroup (LgwActionable  *actionable,
                                 LgwActionGroup *action_group)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    LgwSearchEntryClass *klass = NULL;
    LgwSearchEntryClassPrivate *klasspriv = NULL;
    GList *list = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (actionable);
    priv = search_entry->priv;
    klass = LGW_SEARCHENTRY_GET_CLASS (search_entry);
    klasspriv = klass->priv;

    if (priv->data.action_group_list != NULL)
    {
      g_list_free (priv->data.action_group_list);
      priv->data.action_group_list = NULL;
    }

    if (priv->data.action_group != NULL)
    {
        lgw_actiongroup_free (priv->data.action_group);
        priv->data.action_group = NULL;
    }
    priv->data.action_group = action_group;

    if (action_group != NULL)
    {
      priv->data.action_group_list = g_list_prepend (priv->data.action_group_list, action_group);
    }
}


void
lgw_searchentry_sync_actions (LgwSearchEntry *search_entry)
{
    //Sanity checks
    g_return_val_if_fail (search_entry != NULL, NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;
    gboolean has_focus = FALSE;

    //Initializations
    priv = search_entry->priv;
    widget = GTK_WIDGET (search_entry);
    has_focus = gtk_widget_has_focus (GTK_WIDGET (priv->ui.search_entry)); 

    if (has_focus)
    {
      static GActionEntry entries[] = {
        { "copy", lgw_searchentry_copy_cb, NULL, NULL, NULL },
        { "cut", lgw_searchentry_cut_cb, NULL, NULL, NULL },
        { "paste", lgw_searchentry_paste_cb, NULL, NULL, NULL },
        { "insert-unknown-character", lgw_searchentry_insert_unknown_character_cb, NULL, NULL, NULL },
        { "insert-word-edge-character", lgw_searchentry_insert_word_edge_cb, NULL, NULL, NULL },
        { "insert-not-word-edge-character", lgw_searchentry_insert_not_word_edge_cb, NULL, NULL, NULL },
        { "insert-and-character", lgw_searchentry_insert_and_cb, NULL, NULL, NULL },
        { "insert-or-character", lgw_searchentry_insert_or_cb, NULL, NULL, NULL },
        { "clear", lgw_searchentry_clear_search_cb, NULL, NULL, NULL },
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_actionable_set_actiongroup (LGW_ACTIONABLE (search_entry), action_group);
    }
    else 
    {
      static GActionEntry entries[] = {
        { "insert-unknown-character", lgw_searchentry_insert_unknown_character_cb, NULL, NULL, NULL },
        { "insert-word-edge-character", lgw_searchentry_insert_word_edge_cb, NULL, NULL, NULL },
        { "insert-not-word-edge-character", lgw_searchentry_insert_not_word_edge_cb, NULL, NULL, NULL },
        { "insert-and-character", lgw_searchentry_insert_and_cb, NULL, NULL, NULL },
        { "insert-or-character", lgw_searchentry_insert_or_cb, NULL, NULL, NULL },
        { "clear", lgw_searchentry_clear_search_cb, NULL, NULL, NULL },
      };
      LgwActionGroup *action_group = lgw_actiongroup_static_new (entries, G_N_ELEMENTS (entries), widget);
      lgw_actionable_set_actiongroup (LGW_ACTIONABLE (search_entry), action_group);
    }
}


static GList*
lgw_searchentry_get_actions (LgwActionable *actionable)
{
    //Sanity checks
    g_return_val_if_fail (actionable != NULL, NULL);

    //Declarations
    LgwSearchEntry *search_entry = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    search_entry = LGW_SEARCHENTRY (actionable);
    priv = search_entry->priv;

    return priv->data.action_group_list;
}


void
lgw_searchentry_insert_text (LgwSearchEntry *search_entry,
                             const gchar    *TEXT)
{
    //Sanity checks
    g_return_if_fail (search_entry != NULL);
    if (TEXT == NULL) TEXT = "";

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    gint start = 0;
    gint end = 0;
    gint length = 0;
    GtkEditable *editable = NULL;

    //Initializations
    priv = search_entry->priv;
    editable = GTK_EDITABLE (priv->ui.search_entry);
    if (editable == NULL) goto errored;
    length = g_utf8_strlen (TEXT, -1);

    gtk_editable_get_selection_bounds (editable, &start, &end);
    gtk_editable_delete_text (editable, start, end);

    gtk_editable_insert_text (editable, TEXT, -1, &start);
    gtk_editable_set_position (editable, start);

errored:

    return;
}


void
lgw_searchentry_set_text (LgwSearchEntry *search_entry,
                          const gchar    *TEXT)
{
    //Sanity checks
    g_return_if_fail (search_entry != NULL);
    if (TEXT == NULL) TEXT = "";

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    GtkEntry *entry = NULL;
    GtkEditable *editable = NULL;

    //Initializations
    priv = search_entry->priv;
    entry = GTK_ENTRY (priv->ui.search_entry);
    editable = GTK_EDITABLE (entry);

    gtk_entry_set_text (entry, TEXT);
    gtk_editable_set_position (editable, -1);

errored:

    return;
}



