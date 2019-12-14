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
//!  @file searchsubcommand.c
//!
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/io.h>
#include <libwaei/searchsubcommand.h>

#include <libwaei/searchsubcommand-private.h>


G_DEFINE_TYPE (LwSearchSubCommand, lw_searchsubcommand, LW_TYPE_SUBCOMMAND);


LwSubCommand*
lw_searchsubcommand_new ()
{
    //Declarations
    LwSubCommand* self = NULL;

    //Initializations
    self =  LW_SUBCOMMAND (g_object_new (LW_TYPE_SEARCHSUBCOMMAND, "name", "search", NULL));

    return self;
}


static void 
lw_searchsubcommand_init (LwSearchSubCommand *self)
{
    self->priv = LW_SEARCHSUBCOMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwSearchSubCommandPrivate));

    //Declarations
    LwSearchSubCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    
    {
      GOptionEntry entries[] = {
        { "exact", 'e', 0, G_OPTION_ARG_NONE, &priv->exact_switch, gettext("Do not display less relevant results"), NULL },
        { "dictionary", 'd', 0, G_OPTION_ARG_STRING, &priv->dictionary_text, gettext("Search using a chosen dictionary"), NULL },
        { NULL }
      };
      lw_subcommand_set_option_entries (LW_SUBCOMMAND (self), entries);
    }
}


static void 
lw_searchsubcommand_finalize (GObject *object)
{
    //Declarations
    LwSearchSubCommand *self = NULL;
    LwSearchSubCommandPrivate *priv = NULL;

    //Initalizations
    self = LW_SEARCHSUBCOMMAND (object);
    priv = self->priv;

    memset(self->priv, 0, sizeof(LwSearchSubCommandPrivate));

    G_OBJECT_CLASS (lw_searchsubcommand_parent_class)->finalize (object);
}


static void 
lw_searchsubcommand_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwSearchSubCommand *self;
    LwSearchSubCommandPrivate *priv;

    //Initializations
    self = LW_SEARCHSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_searchsubcommand_get_property (GObject      *object,
                                  guint         property_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwSearchSubCommand *self = NULL;
    LwSearchSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_SEARCHSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_searchsubcommand_class_init (LwSearchSubCommandClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwSubCommandClass *subcommand_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lw_searchsubcommand_set_property;
    object_class->get_property = lw_searchsubcommand_get_property;
    object_class->finalize = lw_searchsubcommand_finalize;

    klass->priv = g_new0 (LwSearchSubCommandClassPrivate, 1);

    g_type_class_add_private (object_class, sizeof (LwSearchSubCommandPrivate));

    subcommand_class = LW_SUBCOMMAND_CLASS (klass);
    subcommand_class->run = lw_searchsubcommand_run;
}


static void
lw_searchsubcommand_parse_query_from_arguments (LwSearchSubCommand *   self,
                                                gchar              *** argv,
                                                gint                 * argc)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCHSUBCOMMAND (self));
    if (*argc < 2) return;

    //Declarations
    LwSearchSubCommandPrivate *priv = NULL;
    gchar **strv = NULL;
    gchar *query_text = NULL;

    //Initializations
    strv = g_new0 (gchar*, *argc);
    if (strv == NULL) goto errored;
    
    memcpy(strv, *(argv + 1), *(argc - 1));

    query_text = g_strjoinv (" ", strv);

    g_free (priv->query_text);
    priv->query_text = lw_utf8_sanitize (query_text);

errored:

    g_free (strv); strv = NULL;
    g_free (query_text); query_text = NULL;
}


static void
lw_searchsubcommand_run (LwSubCommand   * subcommand,
                         gchar        *** argv,
                         gint           * argc)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SEARCHSUBCOMMAND (subcommand));

    //Declarations
    LwSearchSubCommand *self = NULL;
    LwSearchSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_SEARCHSUBCOMMAND (subcommand);
    priv = self->priv;

    lw_searchsubcommand_parse_query_from_arguments (self, argv, argc);
}


