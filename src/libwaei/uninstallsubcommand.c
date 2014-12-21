
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
//!  @file uninstallsubcommand.c
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
#include <libwaei/uninstallsubcommand.h>

#include <libwaei/uninstallsubcommand-private.h>


G_DEFINE_TYPE (LwUninstallSubCommand, lw_uninstallsubcommand, LW_TYPE_SUBCOMMAND);


LwSubCommand*
lw_uninstallsubcommand_new ()
{
    //Declarations
    LwSubCommand* self = NULL;

    //Initializations
    self =  LW_SUBCOMMAND (g_object_new (LW_TYPE_UNINSTALLSUBCOMMAND, NULL));

    return self;
}


static void 
lw_uninstallsubcommand_init (LwUninstallSubCommand *self)
{
    self->priv = LW_UNINSTALLSUBCOMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwUninstallSubCommandPrivate));

    //Declarations
    LwUninstallSubCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    
    {
      GOptionEntry entries[] = {
        { "list", 'l', 0, G_OPTION_ARG_NONE, &priv->list_switch, gettext("List uninstallable dictionaries"), NULL },
        { NULL }
      };
      lw_subcommand_set_option_entries (LW_SUBCOMMAND (self), entries);
    }
}


static void 
lw_uninstallsubcommand_finalize (GObject *object)
{
    //Declarations
    LwUninstallSubCommand *self = NULL;
    LwUninstallSubCommandPrivate *priv = NULL;

    //Initalizations
    self = LW_UNINSTALLSUBCOMMAND (object);
    priv = self->priv;

    memset(self->priv, 0, sizeof(LwUninstallSubCommandPrivate));

    G_OBJECT_CLASS (lw_uninstallsubcommand_parent_class)->finalize (object);
}


static void 
lw_uninstallsubcommand_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwUninstallSubCommand *self;
    LwUninstallSubCommandPrivate *priv;

    //Initializations
    self = LW_UNINSTALLSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_uninstallsubcommand_get_property (GObject      *object,
                                  guint         property_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwUninstallSubCommand *self = NULL;
    LwUninstallSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_UNINSTALLSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_uninstallsubcommand_class_init (LwUninstallSubCommandClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwSubCommandClass *subcommand_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwUninstallSubCommandClassPrivate, 1);
    object_class->set_property = lw_uninstallsubcommand_set_property;
    object_class->get_property = lw_uninstallsubcommand_get_property;
    object_class->finalize = lw_uninstallsubcommand_finalize;

    g_type_class_add_private (object_class, sizeof (LwUninstallSubCommandPrivate));

    subcommand_class = LW_SUBCOMMAND_CLASS (klass);
    subcommand_class->run = lw_uninstallsubcommand_run;
}


static void
lw_uninstallsubcommand_run (LwSubCommand   * subcommand,
                            gchar        *** argv,
                            gint           * argc)
{
}
