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

#include <gtk/gtk.h>

#include <libgwaei/gettext.h>
#include <libgwaei/libgwaei.h>

#include <libgwaei/searchentry-private.h>


G_DEFINE_TYPE_WITH_CODE (LgwSearchEntry, lgw_searchentry, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (LGW_TYPE_ACTIONABLE, lgw_searchentry_implement_actionable_interface));



//!
//! @brief Sets up the variables in main-interface.c and main-callbacks.c for use
//!
GtkWidget* 
lgw_searchentry_new ()
{
    //Declarations
    LgwSearchEntry *entry = NULL;

    //Initializations
    entry = LGW_SEARCHENTRY (g_object_new (LGW_TYPE_SEARCHENTRY, "spacing", 0, "border-width", 0, NULL));

    return GTK_WIDGET (entry);
}


static void 
lgw_searchentry_init (LgwSearchEntry *entry)
{
    entry->priv = LGW_SEARCHENTRY_GET_PRIVATE (entry);
    memset(entry->priv, 0, sizeof(LgwSearchEntryPrivate));

    LgwSearchEntryPrivate *priv;
    priv = entry->priv;

    GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (entry));
    gtk_style_context_add_class (context, GTK_STYLE_CLASS_LINKED);

    gtk_widget_set_margin_left (GTK_WIDGET (entry), 6);
    gtk_widget_set_margin_right (GTK_WIDGET (entry), 6);
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
    LgwSearchEntry *self = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHENTRY (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

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
    LgwSearchEntry *self = NULL;
    LgwActionable *actionable = NULL;
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    self = LGW_SEARCHENTRY (object);
    actionable = LGW_ACTIONABLE (object);
    priv = self->priv;

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
    LgwSearchEntry *self = NULL;
    LgwSearchEntryPrivate *priv = NULL;
    GtkWidget *widget = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_searchentry_parent_class)->constructed (object);
    }

    //Initializations
    self = LGW_SEARCHENTRY (object);
    priv = self->priv;
    priv->ui.box = GTK_BOX (self);
    widget = GTK_WIDGET (object);


    //GtkWidget *overlay = gtk_overlay_new ();
    //gtk_box_pack_start (priv->ui.box, overlay, FALSE, FALSE, 0);

    {
      //Search toggle button
      {
        GtkWidget *label = gtk_label_new (NULL);
        gtk_label_set_markup (GTK_LABEL (label), "<small><b>漢</b></small>");

        GtkWidget *toggle_button = gtk_toggle_button_new();
        gtk_box_pack_start (priv->ui.box, toggle_button, TRUE, TRUE, 0);
        //gtk_overlay_add_overlay (GTK_OVERLAY (overlay), toggle_button);

        gtk_container_add (GTK_CONTAINER (toggle_button), label);
        gtk_misc_set_padding (GTK_MISC (label), 0, 0);

        {
          GtkStyleContext *context = gtk_widget_get_style_context (toggle_button);
          gtk_style_context_set_junction_sides (context, GTK_JUNCTION_LEFT);
          gtk_widget_reset_style (toggle_button);
        }
      }
    }

    {
      GtkWidget *self = gtk_search_entry_new ();
      priv->ui.search_entry = GTK_SEARCH_ENTRY (self);
      gtk_box_pack_start (priv->ui.box, self, FALSE, FALSE, 0);
      //gtk_container_add (GTK_CONTAINER (overlay), self);
      gtk_widget_set_size_request (self, 300, -1);
      gtk_widget_show (self);

      {
        GtkStyleContext *context = gtk_widget_get_style_context (self);
        gtk_style_context_set_junction_sides (context, GTK_JUNCTION_RIGHT);
        gtk_widget_reset_style (self);
      }
    }

    lgw_searchentry_connect_signals (self);
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
lgw_searchentry_set_dictionarylistbox (LgwSearchEntry       *self,
                                       LgwDictionaryListBox *dictionary_list_box)
{

}


void
lgw_searchentry_set_resultsview (LgwSearchEntry *self,
                                 LgwResultsView *results_view)
{
}


GtkEntry*
lgw_searchentry_get_entry (LgwSearchEntry *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return GTK_ENTRY (priv->ui.search_entry);
}


void
lgw_searchentry_insert_text (LgwSearchEntry *self,
                             const gchar    *TEXT)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (TEXT == NULL) TEXT = "";

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    gint start = 0;
    gint end = 0;
    gint length = 0;
    GtkEditable *editable = NULL;

    //Initializations
    priv = self->priv;
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
lgw_searchentry_set_text (LgwSearchEntry *self,
                          const gchar    *TEXT)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    if (TEXT == NULL) TEXT = "";

    //Declarations
    LgwSearchEntryPrivate *priv = NULL;
    GtkEntry *entry = NULL;
    GtkEditable *editable = NULL;

    //Initializations
    priv = self->priv;
    entry = GTK_ENTRY (priv->ui.search_entry);
    editable = GTK_EDITABLE (entry);

    gtk_entry_set_text (entry, TEXT);
    gtk_editable_set_position (editable, -1);

errored:

    return;
}



