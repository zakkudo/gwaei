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
//! @file dictionarymodule.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/dictionarymodule.h>

#include <libwaei/dictionarymodule-private.h>

G_DEFINE_TYPE (LwDictionaryModule, lw_dictionarymodule, G_TYPE_TYPE_MODULE)

GTypeModule *
lw_dictionarymodule_new (gchar const * PATH)
{
    //Declarations
    GTypeModule *self = NULL;

    //Initializations
    self = G_TYPE_MODULE (g_object_new (LW_TYPE_DICTIONARYMODULE, "path", PATH, NULL));

    g_type_module_use (self);
    g_type_module_unuse (self);

    return self;
}


static void
lw_dictionarymodule_init (LwDictionaryModule * self)
{
    self->priv = LW_DICTIONARYMODULE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryModulePrivate));

    LwDictionaryModulePrivate *priv = NULL;

    priv = self->priv;
}


static void 
lw_dictionarymodule_set_property (GObject      * object,
                                  guint          property_id,
                                  const GValue * value,
                                  GParamSpec   * pspec)
{
    //Declarations
    LwDictionaryModule *self = NULL;
    LwDictionaryModulePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYMODULE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PATH:
        lw_dictionarymodule_set_path (self, g_value_get_string (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarymodule_get_property (GObject    * object,
                                  guint        property_id,
                                  GValue     * value,
                                  GParamSpec * pspec)
{
    //Declarations
    LwDictionaryModule *self = NULL;
    LwDictionaryModulePrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYMODULE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PATH:
        g_value_set_string (value, lw_dictionarymodule_get_path (self));
        break;
      case PROP_NAME:
        g_value_set_string (value, lw_dictionarymodule_get_name (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarymodule_finalize (GObject * object)
{
    //Declarations
    LwDictionaryModule *self = NULL;
    LwDictionaryModulePrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYMODULE (object);
    priv = self->priv;


    G_OBJECT_CLASS (lw_dictionarymodule_parent_class)->finalize (object);
}


static void
lw_dictionarymodule_dispose (GObject * object)
{
    //Declarations
    LwDictionaryModule *self = NULL;

    //Initializations
    self = LW_DICTIONARYMODULE (object);

    G_OBJECT_CLASS (lw_dictionarymodule_parent_class)->dispose (object);
}


static void
lw_dictionarymodule_class_init (LwDictionaryModuleClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    GTypeModuleClass *module_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryModuleClassPrivate, 1);
    object_class->set_property = lw_dictionarymodule_set_property;
    object_class->get_property = lw_dictionarymodule_get_property;
    object_class->dispose = lw_dictionarymodule_dispose;
    object_class->finalize = lw_dictionarymodule_finalize;

    module_class = G_TYPE_MODULE_CLASS (klass);
    module_class->load = lw_dictionarymodule_load;
    module_class->unload = lw_dictionarymodule_unload;

    g_type_class_add_private (object_class, sizeof (LwDictionaryModulePrivate));

    klass->priv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        gettext("Name"),
        gettext("Type name for the module"),
        NULL,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_NAME, klass->priv->pspec[PROP_NAME]);

    klass->priv->pspec[PROP_PATH] = g_param_spec_string (
        "path",
        gettext("Path"),
        gettext("File path of the module"),
        NULL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PATH, klass->priv->pspec[PROP_PATH]);
}


gchar **
lw_dictionarymodule_get_dictionarylib_searchpath (gchar const * SEARCHPATH)
{
    if (SEARCHPATH == NULL)
    {
      SEARCHPATH = g_getenv ("DICTIONARYLIB_SEARCHPATH");
    }

    return g_strsplit (SEARCHPATH, G_SEARCHPATH_SEPARATOR_S, 0);
}


static void
lw_dictionarymodule_open (LwDictionaryModule *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (self));
    if (self->priv->path == NULL) return;

    //Declarations
    LwDictionaryModulePrivate *priv = NULL;
    GModule * module = NULL;

    //Initializations
    priv = self->priv;
    module = g_module_open (priv->path, G_MODULE_BIND_LAZY);

    priv->module = module;
    module = NULL;

errored:

    return;
}


static void
lw_dictionarymodule_close (LwDictionaryModule *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (self));

    //Declarations
    LwDictionaryModulePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->module != NULL)
    {
      g_module_close (priv->module);
      priv->module = NULL;
    }
}


static gboolean 
lw_dictionarymodule_register_type (LwDictionaryModule *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYMODULE (self), FALSE);

    //Declarations
    LwDictionaryModulePrivate *priv = NULL;
    gchar * symbol_name = NULL;
    LwDictionaryModuleRegisterTypeFunc register_type = NULL;
    gboolean success = FALSE;

    //Initializations
    priv = self->priv;
    if (priv->name == NULL) goto errored;
    symbol_name = g_strdup_printf ("lw_%s_register_type", priv->name);
    if (priv->module == NULL) goto errored;
    g_module_symbol (priv->module, symbol_name, (gpointer*)(&register_type));
    if (register_type == NULL) goto errored;

    register_type (G_TYPE_MODULE (self));
    success = TRUE;

errored:

    g_free (symbol_name); symbol_name = NULL;

    return success;
}


static gboolean
lw_dictionarymodule_load (GTypeModule *module)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYMODULE (module), NULL);

    //Declarations
    LwDictionaryModule *self = NULL;

    //Initializations
    self = LW_DICTIONARYMODULE (module);

    lw_dictionarymodule_open (self);

errored:

    return lw_dictionarymodule_register_type (self);
}


static void
lw_dictionarymodule_unload (GTypeModule *module)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (module));

    //Declarations
    LwDictionaryModule * self = NULL;

    //Initializations
    self = LW_DICTIONARYMODULE (module);

    lw_dictionarymodule_close (self);
}


gchar const *
lw_dictionarymodule_get_name (LwDictionaryModule * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (self));

    //Declarations
    LwDictionaryModulePrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->name;
}


static void
lw_dictionarymodule_sync_name (LwDictionaryModule * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (self));

    //Declarations
    LwDictionaryModulePrivate * priv = NULL;
    gchar * modulename = NULL;
    gchar * name = NULL;

    //Initializations
    modulename = g_path_get_basename (priv->path);
    if (modulename == NULL) goto errored;

    if (g_str_has_suffix (modulename, ".la"))
    {
      modulename[strlen(modulename) - strlen(".la")] = '\0';
    }

    if (g_str_has_prefix (modulename, "lib"))
    {
      name = g_strdup (modulename + strlen("lib"));
    }
    if (name == NULL) goto errored;

    g_type_module_set_name (G_TYPE_MODULE (self), modulename);

    priv->name = name;
    name = NULL;

errored:

    g_free (modulename);
    modulename = NULL;

    g_free (name);
    name = NULL;
}


void
lw_dictionarymodule_set_path (LwDictionaryModule * self,
                              gchar const        * PATH)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYMODULE (self));
    if (self->priv->path != NULL) return;
    
    //Declarations
    LwDictionaryModulePrivate * priv = NULL;
    LwDictionaryModuleClass * klass = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYMODULE_GET_CLASS (self);
    
    if (g_strcmp0 (priv->path, PATH) == 0) goto errored;

    g_free (priv->path);
    priv->path = g_strdup (PATH);

    lw_dictionarymodule_sync_name (self);
    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_PATH]);

errored:

    return;
}


gchar const *
lw_dictionarymodule_get_path (LwDictionaryModule * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYMODULE (self), NULL);

    //Declarations
    LwDictionaryModulePrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->path;
}


GList *
lw_dictionarymodule_get_available (gchar const * SEARCHPATH)
{
    //Sanity checks
    g_return_val_if_fail (SEARCHPATH != NULL, NULL);

    //Declarations
    GList *names = NULL;
    gchar const * NAME = NULL;
    GDir *directory = NULL;
    GError *error = NULL;
    gint length = 0;
    gchar ** paths = NULL;
    gint i = 0;

    //Initializations
    paths = lw_dictionarymodule_get_dictionarylib_searchpath (SEARCHPATH);
    if (paths == NULL) goto errored;

    for (i = 0; paths[i] != NULL; i++)
    {
      directory = g_dir_open (paths[i], 0, &error);
      if (directory == NULL) continue;

      while ((NAME = g_dir_read_name (directory)) != NULL)
      {
        length = strlen(NAME);

        if (!g_str_has_prefix (NAME, "lib")) continue;
        length -= strlen("lib");

        if (!g_str_has_suffix (NAME, G_MODULE_SUFFIX)) continue;
        length -= strlen(".") + strlen(G_MODULE_SUFFIX);

        if (length < 1) continue;

        gchar * n = g_strndup (NAME + strlen("lib"), length);
        if (n != NULL)
        {
          gchar * path = g_build_filename (paths[i], n, NULL);
          if (path != NULL && g_file_test (path, G_FILE_TEST_IS_REGULAR))
          {
            names = g_list_prepend (names, path);
            path = NULL;
          }

          g_free (path);
          path = NULL;

          g_free (n);
          n = NULL;
        }
      }

      g_dir_close (directory);
      directory = NULL;
    }

errored:

    if (directory != NULL) g_dir_close (directory);
    directory = NULL;

    if (error != NULL)
    {
      printf ("%s\n", error->message);
      g_clear_error (&error);
      error = NULL;
    }

    return g_list_reverse (names);
}
