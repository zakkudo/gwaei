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
//! @file tagtable-callbacks.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include <gtk/gtk.h>

#include <libgwaei/libgwaei.h>
#include <libgwaei/texttagtable-private.h>
#include <libgwaei/gettext.h>


void
lgw_texttagtable_connect_signals (LgwTextTagTable *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_TEXTTAGTABLE (self));

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    preferences = lgw_texttagtable_get_preferences (self);
    if (preferences == NULL) goto errored;

    if (priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_FG] == 0) {
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_FG] = lw_preferences_add_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          LW_KEY_MATCH_FG, 
          lgw_texttagtable_sync_tag_cb, 
          self
      );
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_FG] = 0;
    }

    if (priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_BG] == 0) {
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_BG] = lw_preferences_add_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          LW_KEY_MATCH_BG, 
          lgw_texttagtable_sync_tag_cb, 
          self
      );
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_BG] = 0;
    }

    if (priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_FG] == 0) {
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_FG] = lw_preferences_add_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          LW_KEY_HEADER_FG, 
          lgw_texttagtable_sync_tag_cb, 
          self
      );
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_FG] = 0;
    }

    if (priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_BG] == 0) {
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_BG] = lw_preferences_add_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          LW_KEY_HEADER_BG, 
          lgw_texttagtable_sync_tag_cb, 
          self
      );
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_BG] = 0;
    }

    if (priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_COMMENT_FG] == 0) {
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_COMMENT_FG] = lw_preferences_add_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          LW_KEY_COMMENT_FG, 
          lgw_texttagtable_sync_tag_cb, 
          self
      );
      priv->data.signalid[LGW_TEXTTAGTABLE_SIGNALID_COMMENT_FG] = 0;
    }

errored:

    return;
}


void
lgw_texttagtable_disconnect_signals (LgwTextTagTable *self)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_TEXTTAGTABLE (self));

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    preferences = lgw_texttagtable_get_preferences (self);
    if (preferences == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; i < G_N_ELEMENTS (priv->data.signalid); i++) {
        lw_preferences_remove_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          priv->data.signalid[i]
        );
        priv->data.signalid[i] = 0;
      }
    }

errored:

    return;
}


//!
//! @brief Resets the color tags according to the preferences
//!
void
lgw_texttagtable_sync_tag_cb (GSettings *settings,
                              gchar     *key,
                              gpointer   data)
{
    //Sanity checks
    g_return_if_fail (settings);
    g_return_if_fail (key != NULL);

    //Declarations
    gchar *text = NULL;
    GdkRGBA color;
    gchar **pair = NULL;
    GtkTextTag *tag = NULL;
    GtkTextTagTable *self = NULL;

    self = GTK_TEXT_TAG_TABLE (data);
    if (self == NULL) goto errored;

    //Parse the color
    text = lw_preferences_get_string (settings, key);
    if (gdk_rgba_parse (&color, text) == FALSE)
    {
      fprintf(stderr, "Failed to set tag to the tag table: %s\n", text);
      lw_preferences_reset_value (settings, key);
      return;
    }

    //Update the tag 
    pair = g_strsplit (key, "-", 2);
    if (pair != NULL && pair[0] != NULL && pair[1] != NULL)
    {
      tag = gtk_text_tag_table_lookup (self, pair[0]);
      if (tag == NULL) goto errored;
      g_object_set (G_OBJECT (tag), pair[1], text, NULL);
    }

errored:

    if (pair != NULL) g_strfreev (pair);
    pair = NULL;
}

