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
//! @file application.c
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
#include <unistd.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/application.h>

#include <libwaei/application-private.h>

G_DEFINE_ABSTRACT_TYPE (LwApplication, lw_application, G_TYPE_APPLICATION)


static void 
lw_application_init (LwApplication *self)
{
    self->priv = LW_APPLICATION_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwApplicationPrivate));

    LwApplicationPrivate *priv = NULL;

    priv = self->priv;

    priv->dictionarymodules = g_tree_new_full ((GCompareDataFunc) g_strcmp0, NULL, NULL, (GDestroyNotify) g_object_unref);

    lw_application_load_dictionarymodules (self);
}


static void 
lw_application_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_application_parent_class)->constructed (object);
    }

    //Declarations
    LwApplication *self = NULL;
    LwApplicationPrivate *priv = NULL;

    //Initializations
    self = LW_APPLICATION (object);
    priv = self->priv;
}


static void 
lw_application_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwApplication *self = NULL;
    LwApplicationPrivate *priv = NULL;

    //Initializations
    self = LW_APPLICATION (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_application_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    //Declarations
    LwApplication *self = NULL;
    LwApplicationPrivate *priv = NULL;

    //Initializations
    self = LW_APPLICATION (object);
    priv = self->priv;

    switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_application_dispose (GObject *object)
{
    //Declarations
    LwApplication *self = NULL;

    //Initializations
    self = LW_APPLICATION (object);

    G_OBJECT_CLASS (lw_application_parent_class)->dispose (object);
}



static void 
lw_application_finalize (GObject *object)
{
    //Declarations
    LwApplication *self = NULL;
    LwApplicationPrivate *priv = NULL;

    //Initializations
    self = LW_APPLICATION (object);
    priv = self->priv;

    g_tree_unref (priv->dictionarymodules);
    priv->dictionarymodules = NULL;

    //memset(priv, 0, sizeof(LwApplicationPrivate))

    G_OBJECT_CLASS (lw_application_parent_class)->finalize (object);
}


static void
lw_application_class_init (LwApplicationClass *klass)
{
    GObjectClass *object_class = NULL;
    GApplicationClass *application_class = NULL;

    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwApplicationClassPrivate, 1);
    application_class = G_APPLICATION_CLASS (klass);

    object_class->constructed = lw_application_constructed;
    object_class->set_property = lw_application_set_property;
    object_class->get_property = lw_application_get_property;
    object_class->dispose = lw_application_dispose;
    object_class->finalize = lw_application_finalize;

    g_type_class_add_private (object_class, sizeof (LwApplicationPrivate));
}


static void
lw_application_add_dictionarymodule (LwApplication     * self,
                                     LwDictionaryModule * dictionarymodule)
{
    //Sanity checks
    g_return_if_fail (LW_IS_APPLICATION (self));

    //Declarations
    LwApplicationPrivate *priv = NULL;
    gchar const * NAME = NULL;

    //Initializations
    priv = self->priv;
    NAME = lw_dictionarymodule_get_name (dictionarymodule);
    if (NAME == NULL) goto errored;
    if (g_tree_lookup (priv->dictionarymodules, NAME) != NULL) goto errored;

    g_object_ref (dictionarymodule);
    g_tree_insert (priv->dictionarymodules, (gchar*) NAME, dictionarymodule);

errored:

    return;
}


static void
lw_application_load_dictionarymodules (LwApplication * self)
{
    //Declarations
    GList *link = NULL;
    GList *names = NULL;

    //Initializations
    names = lw_dictionarymodule_get_available (NULL);

    for (link = names; link != NULL; link = link->next)
    {
      gchar const *NAME = link->data;
      GTypeModule *module = lw_dictionarymodule_new (NAME);
      if (module != NULL)
      {
        lw_application_add_dictionarymodule (self, LW_DICTIONARYMODULE (module));
        g_object_unref (module);
        module = NULL;
      }
    }

errored:

    g_list_free_full (names, (GDestroyNotify) g_free);
    names = NULL;
}
