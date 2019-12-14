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
//!  @file subcommand.c
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
#include <libwaei/subcommand.h>

#include <libwaei/subcommand-private.h>


G_DEFINE_ABSTRACT_TYPE (LwSubCommand, lw_subcommand, G_TYPE_OBJECT);


static void 
lw_subcommand_init (LwSubCommand *self)
{
    self->priv = LW_SUBCOMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwSubCommandPrivate));
}


static void 
lw_subcommand_finalize (GObject *object)
{
    //Declarations
    LwSubCommand *self = NULL;
    LwSubCommandPrivate *priv = NULL;

    //Initalizations
    self = LW_SUBCOMMAND (object);
    priv = self->priv;

    lw_subcommand_set_name (self, NULL);

    G_OBJECT_CLASS (lw_subcommand_parent_class)->finalize (object);
}


static void 
lw_subcommand_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwSubCommand *self = NULL;
    LwSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_SUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NAME:
        lw_subcommand_set_name (self, g_value_get_string (value));
        break;
      case PROP_OPTION_ENTRIES:
        lw_subcommand_set_option_entries (self, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_subcommand_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwSubCommand *self = NULL;
    LwSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_SUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NAME:
        g_value_set_string (value, lw_subcommand_get_name (self));
        break;
      case PROP_OPTION_ENTRIES:
        g_value_set_pointer (value, (gpointer) lw_subcommand_get_option_entries (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_subcommand_class_init (LwSubCommandClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwSubCommandClassPrivate, 1);
    object_class->set_property = lw_subcommand_set_property;
    object_class->get_property = lw_subcommand_get_property;
    object_class->finalize = lw_subcommand_finalize;

    g_type_class_add_private (object_class, sizeof (LwSubCommandPrivate));

    klass->priv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        "Name",
        "The name of the subcommand as will appear in the command-line",
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (
        object_class,
        PROP_NAME,
        klass->priv->pspec[PROP_NAME]
    );

    klass->priv->pspec[PROP_OPTION_ENTRIES] = g_param_spec_pointer (
        "option-entries",
        "Option Entries",
        "Entries that will by parsed by GOptionContext for this subcommand",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_property (
        object_class,
        PROP_OPTION_ENTRIES,
        klass->priv->pspec[PROP_OPTION_ENTRIES]
    );
}



void
lw_subcommand_run (LwSubCommand *self, gchar *** argv, gint * argc)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SUBCOMMAND (self));
    g_return_if_fail (argv != NULL);
    g_return_if_fail (argc != NULL);

    //Declarations
    LwSubCommandClass *klass = NULL;

    //Initializations
    klass = LW_SUBCOMMAND_CLASS (self);

    klass->run(self, argv, argc);
}


GOptionGroup*
lw_subcommand_build_option_group (LwSubCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SUBCOMMAND (self), NULL);

    //Declarations
    LwSubCommandPrivate *priv = NULL;
    GOptionGroup *option_group = NULL;

    //Initializations
    priv = self->priv;

errored:

    return option_group;
}


GOptionEntry const *
lw_subcommand_get_option_entries (LwSubCommand *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SUBCOMMAND (self), NULL);

    //Declarations
    LwSubCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->option_entries;
}


void
lw_subcommand_set_option_entries (LwSubCommand       * self,
                                  GOptionEntry const * option_entries)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SUBCOMMAND (self));

    //Declarations
    LwSubCommandPrivate *priv = NULL;
    LwSubCommandClass *klass = NULL;
    gint length = 0;
    GOptionEntry* copy = NULL;

    //Initializations
    priv = self->priv;
    if (priv->option_entries == option_entries) goto errored;
    klass = LW_SUBCOMMAND_CLASS (self);

    length = 1;
    while (option_entries[length].long_name != NULL) length++;
    copy = g_new (GOptionEntry, length);
    if (copy == NULL) goto errored;
    memcpy(copy, option_entries, sizeof(GOptionEntry) * length);

    g_free (priv->option_entries);
    priv->option_entries = copy;
    copy = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_OPTION_ENTRIES]);

errored:

    g_free (copy); copy = NULL;

    return;
}


gchar const *
lw_subcommand_get_name (LwSubCommand * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SUBCOMMAND (self), NULL);

    //Declarations
    LwSubCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->name;
}


static void
lw_subcommand_set_name (LwSubCommand * self,
                        gchar const  * NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SUBCOMMAND (self));

    //Declarations
    LwSubCommandPrivate *priv = NULL;
    LwSubCommandClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SUBCOMMAND_GET_CLASS (self);
    
    if (g_strcmp0 (priv->name, NAME) == 0) goto errored;

    g_free (priv->name);
    priv->name = g_strdup (NAME);
    
    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_NAME]);

errored:

    return;
}

