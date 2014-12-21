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
//!  @file installsubcommand.c
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
#include <libwaei/installsubcommand.h>

#include <libwaei/installsubcommand-private.h>


G_DEFINE_TYPE (LwInstallSubCommand, lw_installsubcommand, LW_TYPE_SUBCOMMAND);


LwSubCommand*
lw_installsubcommand_new ()
{
    //Declarations
    LwSubCommand* self = NULL;

    //Initializations
    self =  LW_SUBCOMMAND (g_object_new (LW_TYPE_INSTALLSUBCOMMAND, NULL));

    return self;
}


static void 
lw_installsubcommand_init (LwInstallSubCommand *self)
{
    self->priv = LW_INSTALLSUBCOMMAND_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwInstallSubCommandPrivate));

    //Declarations
    LwInstallSubCommandPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    
    {
      GOptionEntry entries[] = {
        { "list", 'l', 0, G_OPTION_ARG_NONE, &priv->list_switch, gettext("List installable dictionaries"), NULL },
        { NULL }
      };
      lw_subcommand_set_option_entries (LW_SUBCOMMAND (self), entries);
    }
}


static void 
lw_installsubcommand_finalize (GObject *object)
{
    //Declarations
    LwInstallSubCommand *self = NULL;
    LwInstallSubCommandPrivate *priv = NULL;

    //Initalizations
    self = LW_INSTALLSUBCOMMAND (object);
    priv = self->priv;

    memset(self->priv, 0, sizeof(LwInstallSubCommandPrivate));

    G_OBJECT_CLASS (lw_installsubcommand_parent_class)->finalize (object);
}


static void 
lw_installsubcommand_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwInstallSubCommand *self;
    LwInstallSubCommandPrivate *priv;

    //Initializations
    self = LW_INSTALLSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_installsubcommand_get_property (GObject      *object,
                                  guint         property_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwInstallSubCommand *self = NULL;
    LwInstallSubCommandPrivate *priv = NULL;

    //Initializations
    self = LW_INSTALLSUBCOMMAND (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_installsubcommand_class_init (LwInstallSubCommandClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwSubCommandClass *subcommand_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lw_installsubcommand_set_property;
    object_class->get_property = lw_installsubcommand_get_property;
    object_class->finalize = lw_installsubcommand_finalize;

    klass->priv = g_new0 (LwInstallSubCommandClassPrivate, 1);

    g_type_class_add_private (object_class, sizeof (LwInstallSubCommandPrivate));

    subcommand_class = LW_SUBCOMMAND_CLASS (klass);
    subcommand_class->run = lw_installsubcommand_run;
}


static void
lw_installsubcommand_run (LwSubCommand   * subcommand,
                          gchar        *** argv,
                          gint           * argc)
{
}


