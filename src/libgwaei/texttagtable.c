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

G_DEFINE_TYPE (LgwTextTagTable, lgw_texttagtable, GTK_TYPE_TEXT_TAG_TABLE)


GtkTextTagTable*
lgw_texttagtable_new (LwPreferences *preferences)
{
    //Declarations
    LgwTextTagTable *tag_table = NULL;

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (g_object_new (LGW_TYPE_TEXTTAGTABLE, "preferences", preferences, NULL));

    return GTK_TEXT_TAG_TABLE (tag_table);
}


static void 
lgw_texttagtable_init (LgwTextTagTable *tag_table)
{
    tag_table->priv = LGW_TEXTTAGTABLE_GET_PRIVATE (tag_table);
    memset(tag_table->priv, 0, sizeof(LgwTextTagTablePrivate));
}


static void
lgw_texttagtable_dispose (GObject *object)
{
    //Declarations
    LgwTextTagTable *tag_table = NULL;

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (object);

    lgw_texttagtable_disconnect_signals (tag_table);

    G_OBJECT_CLASS (lgw_texttagtable_parent_class)->dispose (object);
}


static void 
lgw_texttagtable_finalize (GObject *object)
{
    //Declarations
    LgwTextTagTable *tag_table = NULL;

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (object);

    G_OBJECT_CLASS (lgw_texttagtable_parent_class)->finalize (object);
}


static void 
lgw_texttagtable_constructed (GObject *object)
{
    //Declarations
    LgwTextTagTable *tag_table = NULL;

    //Chain the parent class
    {
      G_OBJECT_CLASS (lgw_texttagtable_parent_class)->constructed (object);
    }

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (object);

    lgw_texttagtable_init_base_tags (tag_table);
}


static void 
lgw_texttagtable_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    //Declarations
    LgwTextTagTable *tag_table = NULL;
    LgwTextTagTablePrivate *priv = NULL;

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (object);
    priv = tag_table->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        lgw_texttagtable_set_preferences (tag_table, LW_PREFERENCES (g_value_get_object (value)));
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
    LgwTextTagTable *tag_table = NULL;
    LgwTextTagTablePrivate *priv = NULL;

    //Initializations
    tag_table = LGW_TEXTTAGTABLE (object);
    priv = tag_table->priv;

    switch (property_id)
    {
      case PROP_PREFERENCES:
        g_value_set_object (value, lgw_texttagtable_get_preferences (tag_table));
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
    GObjectClass *object_class = NULL;
    LgwTextTagTableClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lgw_texttagtable_set_property;
    object_class->get_property = lgw_texttagtable_get_property;
    object_class->dispose = lgw_texttagtable_dispose;
    object_class->finalize = lgw_texttagtable_finalize;
    object_class->constructed = lgw_texttagtable_constructed;

    klass->priv = g_new0 (LgwTextTagTableClassPrivate, 1);

    klasspriv = klass->priv;

    g_type_class_add_private (object_class, sizeof (LgwTextTagTablePrivate));

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "Preferences construct prop",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);
}


static void
lgw_texttagtable_init_base_tags (LgwTextTagTable *tag_table)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_TEXTTAGTABLE (tag_table));

    //Declarations
    GtkTextTag *tag = NULL;

    tag = gtk_text_tag_new ("entry-grand-header");
    g_object_set (tag, "scale", 5.0, "family", "KanjiStrokeOrders", NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-header");
    g_object_set (tag, "scale", 1.3, "weight", 600, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-definition");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-lexicon");
    g_object_set (tag, "scale", 1.0, "foreground", "#888888", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-popular");
    g_object_set (tag, "scale", 1.0, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-example-definition");
    g_object_set (tag, "scale", 1.3, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("entry-bullet");
    g_object_set (tag, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("comment");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("match");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("header");
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);

    tag = gtk_text_tag_new ("spacing");
    g_object_set (tag, "scale", 0.5, NULL);
    gtk_text_tag_table_add (GTK_TEXT_TAG_TABLE (tag_table), tag);
    g_object_unref (tag);
}


void
lgw_texttagtable_set_preferences (LgwTextTagTable *tag_table,
                                  LwPreferences   *preferences)
{
    //Sanity checks
    g_return_if_fail (LGW_IS_TEXTTAGTABLE (tag_table));

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;
    LgwTextTagTableClass *klass = NULL;
    LgwTextTagTableClassPrivate* klasspriv = NULL;

    //Initializations
    priv = tag_table->priv;
    klass = LGW_TEXTTAGTABLE_GET_CLASS (tag_table);
    klasspriv = klass->priv;

    if (preferences != NULL)
    {
      g_object_ref (preferences);
    }

    if (priv->config.preferences != NULL)
    {
      lgw_texttagtable_disconnect_signals (tag_table);
      g_object_remove_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      g_object_unref (priv->config.preferences);
      priv->config.preferences = NULL;
    }

    priv->config.preferences = preferences;

    if (priv->config.preferences != NULL)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->config.preferences), (gpointer*) &(priv->config.preferences));
      lgw_texttagtable_connect_signals (tag_table);
    }

    g_object_notify_by_pspec (G_OBJECT (tag_table), klasspriv->pspec[PROP_PREFERENCES]);
}


LwPreferences*
lgw_texttagtable_get_preferences (LgwTextTagTable *tag_table)
{
    //Sanity checks
    g_return_val_if_fail (LGW_IS_TEXTTAGTABLE (tag_table), NULL);

    //Declarations
    LgwTextTagTablePrivate *priv = NULL;

    //Initializations
    priv = tag_table->priv;

    return priv->config.preferences;
}

