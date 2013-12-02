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
//! @file tagtable.c
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

static void lgw_texttagtable_init_base_tags (LgwTextTagTable*);
static void lgw_texttagtable_connect_signals (LgwTextTagTable*);
static void lgw_texttagtable_disconnect_signals (LgwTextTagTable*);
static void lgw_texttagtable_sync_tag_cb (GSettings*, gchar*, gpointer);

G_DEFINE_TYPE (LgwTextTagTable, lgw_texttagtable, GTK_TYPE_TEXT_TAG_TABLE)

typedef enum
{
  PROP_0,
  PROP_PREFERENCES,
} LgwTextTagTableProps;


GtkTextTagTable*
lgw_texttagtable_new ()
{
    //Declarations
    LgwTextTagTable *tagtable = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    preferences = lw_preferences_get_default ();
    tagtable = LGW_TEXTTAGTABLE (g_object_new (LGW_TYPE_TEXTTAGTABLE, "preferences", preferences, NULL));

    return GTK_TEXT_TAG_TABLE (tagtable);
}


void
lgw_texttagtable_set_preferences (LgwTextTagTable *tagtable,
                                  LwPreferences   *preferences)
{
    //Sanity checks
    g_return_if_fail (tagtable != NULL);

    g_object_set (G_OBJECT (tagtable), "preferences", preferences, NULL);
}


static void 
lgw_texttagtable_init (LgwTextTagTable *tagtable)
{
    tagtable->priv = LGW_TEXTTAGTABLE_GET_PRIVATE (tagtable);
    memset(tagtable->priv, 0, sizeof(LgwTextTagTablePrivate));
}


static void 
lgw_texttagtable_finalize (GObject *object)
{
    //Declarations
    LgwTextTagTable *tagtable = NULL;

    //Initializations
    tagtable = LGW_TEXTTAGTABLE (object);

    lgw_texttagtable_disconnect_signals (tagtable);

    G_OBJECT_CLASS (lgw_texttagtable_parent_class)->finalize (object);
}


static void 
lgw_texttagtable_constructed (GObject *object)
{
    //Declarations
    LgwTextTagTable *tagtable;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_texttagtable_parent_class)->constructed (object);
    }

    //Initializations
    tagtable = LGW_TEXTTAGTABLE (object);

    lgw_texttagtable_init_base_tags (tagtable);
    lgw_texttagtable_connect_signals (tagtable);
}


static void 
lgw_texttagtable_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    //Declarations
    LgwTextTagTable *tagtable = NULL;
    LgwTextTagTablePrivate *priv = NULL;

    //Initializations
    tagtable = LGW_TEXTTAGTABLE (object);
    priv = tagtable->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lgw_texttagtable_disconnect_signals (tagtable);
        if (priv->preferences != NULL) g_object_unref (priv->preferences);
        priv->preferences = LW_PREFERENCES (g_value_get_object (value));
        lgw_texttagtable_connect_signals (tagtable);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lgw_texttagtable_get_property (GObject      *object,
                              guint         property_id,
                              GValue       *value,
                              GParamSpec   *pspec)
{
    //Declarations
    LgwTextTagTable *tagtable = NULL;
    LgwTextTagTablePrivate *priv = NULL;

    //Initializations
    tagtable = LGW_TEXTTAGTABLE (object);
    priv = tagtable->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, priv->preferences);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lgw_texttagtable_class_init (LgwTextTagTableClass *klass)
{
    //Declarations
    GParamSpec *pspec = NULL;
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_texttagtable_set_property;
    object_class->get_property = lgw_texttagtable_get_property;
    object_class->finalize = lgw_texttagtable_finalize;
    object_class->constructed = lgw_texttagtable_constructed;

    g_type_class_add_private (object_class, sizeof (LgwTextTagTablePrivate));

    pspec = g_param_spec_object ("preferences",
                                 "Preferences construct prop",
                                 "Set the preferences object",
                                 LW_TYPE_PREFERENCES,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, pspec);
}


static void
lgw_texttagtable_init_base_tags (LgwTextTagTable *tagtable)
{
    //Declarations
    GtkTextTag *tag = NULL;

    tag = gtk_text_tag_new ("entry-grand-header");
    g_object_set (tag, "scale", 5.0, "family", "KanjiStrokeOrders", NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-header");
    g_object_set (tag, "scale", 1.3, "weight", 600, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-definition");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-lexicon");
    g_object_set (tag, "scale", 1.0, "foreground", "#888888", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-popular");
    g_object_set (tag, "scale", 1.0, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-example-definition");
    g_object_set (tag, "scale", 1.3, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-bullet");
    g_object_set (tag, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("comment");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("match");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("header");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("spacing");
    g_object_set (tag, "scale", 0.5, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tagtable), tag);
    g_object_unref (tag);
}


static void
lgw_texttagtable_connect_signals (LgwTextTagTable *tagtable)
{
    //Sanity checks
    g_return_if_fail (tagtable != NULL);

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = tagtable->priv;
    preferences = priv->preferences;
    if (preferences == NULL) goto errored;

    priv->signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_FG] = lw_preferences_add_change_listener_by_schema (
        preferences, 
        LW_SCHEMA_HIGHLIGHT, 
        LW_KEY_MATCH_FG, 
        lgw_texttagtable_sync_tag_cb, 
        tagtable
    );

    priv->signalid[LGW_TEXTTAGTABLE_SIGNALID_MATCH_BG] = lw_preferences_add_change_listener_by_schema (
        preferences, 
        LW_SCHEMA_HIGHLIGHT, 
        LW_KEY_MATCH_BG, 
        lgw_texttagtable_sync_tag_cb, 
        tagtable
    );

    priv->signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_FG] = lw_preferences_add_change_listener_by_schema (
        preferences, 
        LW_SCHEMA_HIGHLIGHT, 
        LW_KEY_HEADER_FG, 
        lgw_texttagtable_sync_tag_cb, 
        tagtable
    );

    priv->signalid[LGW_TEXTTAGTABLE_SIGNALID_HEADER_BG] = lw_preferences_add_change_listener_by_schema (
        preferences, 
        LW_SCHEMA_HIGHLIGHT, 
        LW_KEY_HEADER_BG, 
        lgw_texttagtable_sync_tag_cb, 
        tagtable
    );

    priv->signalid[LGW_TEXTTAGTABLE_SIGNALID_COMMENT_FG] = lw_preferences_add_change_listener_by_schema (
        preferences, 
        LW_SCHEMA_HIGHLIGHT, 
        LW_KEY_COMMENT_FG, 
        lgw_texttagtable_sync_tag_cb, 
        tagtable
    );

errored:

    return;
}


static void
lgw_texttagtable_disconnect_signals (LgwTextTagTable *tagtable)
{
    //Sanity checks
    g_return_if_fail (tagtable != NULL);

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = tagtable->priv;
    preferences = priv->preferences;
    if (preferences == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; i < G_N_ELEMENTS (priv->signalid); i++) {
        lw_preferences_remove_change_listener_by_schema (
          preferences, 
          LW_SCHEMA_HIGHLIGHT, 
          priv->signalid[i]
        );
        priv->signalid[i] = 0;
      }
    }

errored:

    return;
}


//!
//! @brief Resets the color tags according to the preferences
//!
static void
lgw_texttagtable_sync_tag_cb (GSettings *settings, gchar *key, gpointer data)
{
    //Declarations
    gchar *text = NULL;
    GdkRGBA color;
    gchar **pair = NULL;
    GtkTextTag *tag = NULL;
    GtkTextTagTable *tagtable = NULL;

    tagtable = GTK_TEXT_TAG_TABLE (data);

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
      tag = gtk_text_tag_table_lookup (tagtable, pair[0]);
      g_object_set (G_OBJECT (tag), pair[1], text, NULL);
    }

errored:

    if (pair != NULL) g_strfreev (pair);
    pair = NULL;
}


