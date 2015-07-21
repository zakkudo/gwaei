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

/**
 * SECTION:dictionarymodule
 * @short_description: Methods for dynamically loading libwaei dictionaries
 * @title: LwDictionaryModule
 *
 * #LwDictionaryModule allows dynamic loading and unloading of #LwDictionary classes
 * from the type system separate from the libwaei core.  This also brings support for
 * third party dictionaries and allows for smarter memory management, only loading
 * the memory for the dictionaries that are actually used.  Below is a very basic
 * example of loading modules:
 *
 * |[<!-- langauge="c" -->
 * LwDictionaryModuleReader * reader = lw_dictionarymodulereader_open (NULL);
 * if (reader != NULL)
 * {
 *   gchar const * path = NULL;
 *   GList * modules = NULL;
 *   while ((path = lw_dictionarymodulereader_read_path (reader)) != NULL)
 *   {
 *     modules = g_list_prepend (modules, lw_dictionarymodule_new (path));
 *   }
 *   lw_dictionarymodulereader_close (reader);
 * }
 * ]|
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include <libwaei/gettext.h>
#include <libwaei/dictionarymodule.h>

#include <libwaei/dictionarymodule-private.h>

G_DEFINE_TYPE (LwDictionaryModule, lw_dictionarymodule, G_TYPE_TYPE_MODULE)


/**
 * lw_dictionarymodule_new:
 * @PATH: (transfer none): A path to load the module from
 * 
 * This method was made to be used with the paths generated by #LwDictionaryModuleReader.
 * Loading a dictionary with this method registers the type, making it available to make
 * instances.  The module will stay loaded as long as there are any dictionaries remaining
 * with refcounts.
 *
 * Returns: (transfer full): A new #GTypeModule that can be free with g_object_unref()
 */
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
    /**
     * LwDictionaryModule:name:
     *
     * The name of the dictionary module
     */
    g_object_class_install_property (object_class, PROP_NAME, klass->priv->pspec[PROP_NAME]);

    klass->priv->pspec[PROP_PATH] = g_param_spec_string (
        "path",
        gettext("Path"),
        gettext("File path of the module"),
        NULL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    /**
     * LwDictionaryModule:path:
     *
     * The file path of the dictionary module
     */
    g_object_class_install_property (object_class, PROP_PATH, klass->priv->pspec[PROP_PATH]);
}


/**
 * lw_dictionarymodule_get_dictionarylib_searchpath:
 * @SEARCHPATH_OVERRIDE: (transfer none): Search path to loadable dictionary modules or %NULL to use the defaults

 * Returns the searchpath as an array of paths.  If the @SEARCHPATH_OVERRIDE
 * is %NULL, the program checks if %DICTIONARYLIB_SEARCHPATH has been
 * set in the environment.  If it hasn't been set, the compiled in search
 * paths are used. Paths should be separated with semicolons on windows
 * and colons on unixes.
 *
 * Returns: (transfer full): A %NULL terminated array of strings that should be freed with g_strfreev()
 */
gchar **
lw_dictionarymodule_get_dictionarylib_searchpath (gchar const * SEARCHPATH_OVERRIDE)
{
    //Declarations
    gchar const * SEARCHPATH = SEARCHPATH_OVERRIDE;

    if (SEARCHPATH == NULL)
    {
      SEARCHPATH = g_getenv ("DICTIONARYLIB_SEARCHPATH");
    }
    if (SEARCHPATH == NULL)
    {
      SEARCHPATH = DICTIONARYLIBDIR;
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
    if (module == NULL)
    {
      g_error (g_module_error());
    }

    if (priv->module != NULL) g_object_unref (priv->module);
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
    LwDictionaryModuleRegisterTypeFunc register_type = NULL;
    gboolean success = FALSE;

    //Initializations
    priv = self->priv;
    if (priv->name == NULL) goto errored;
    if (priv->module == NULL) goto errored;
    if (!g_module_symbol (priv->module, "register_dictionary_module_type", (gpointer*)(&register_type)))
    {
      g_error (g_module_error());
    }
    if (register_type == NULL) goto errored;

    register_type (G_TYPE_MODULE (self));
    success = TRUE;

errored:

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

    g_assert_true (g_module_supported ());

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


/**
 * lw_dictionarymodule_get_name:
 * @self: a #LwDictionaryModule
 * Returns: (transfer none): The name of the module.  Do not modify or free as it is owned by the instance.  The name is generated automatically by the path of the module.
 */
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
    LwDictionaryModuleClass * klass = NULL;
    gchar * modulename = NULL;
    gchar * name = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_DICTIONARYMODULE_GET_CLASS (self);
    modulename = g_path_get_basename (priv->path);
    if (modulename == NULL) goto errored;

    if (g_str_has_suffix (modulename, G_MODULE_SUFFIX))
    {
      modulename[strlen(modulename) - strlen("." G_MODULE_SUFFIX)] = '\0';
    }

    if (g_str_has_prefix (modulename, "lib"))
    {
      name = g_strdup (modulename + strlen("lib"));
    }
    if (name == NULL) goto errored;

    g_type_module_set_name (G_TYPE_MODULE (self), modulename);

    priv->name = name;
    name = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_NAME]);

errored:

    g_free (modulename);
    modulename = NULL;

    g_free (name);
    name = NULL;
}


/**
 * lw_dictionarymodule_set_path:
 * @self: A #LwDictionaryModule
 * @PATH: (transfer none): The path of the module to load.  This can only be set once on the object and thus is usually set in the constructor for the object.
 */
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


/**
 * lw_dictionarymodule_get_path:
 * @self: A #LwDictionaryModule
 *
 * Returns: (transfer none): The set path of the module.  This doesn't necessarily mean it is loaded. This string is owned by the instance and should not be freed or modified.
 */
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


/**
 * lw_dictionarymodule_symbol:
 * @self: A #LwDictionaryModule
 * @symbol_name: (transfer none): The name of the symbol to lookup
 * @symbol: A pointer to set the symbol
 * Returns: %TRUE if the symbol was fetched
 *
 * Gets a symbol from the dictionary module
 */
gboolean
lw_dictionarymodule_symbol (LwDictionaryModule * self,
                            gchar const        * SYMBOL_NAME,
                            gpointer           * symbol)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYMODULE (self), FALSE);

    //Declarations
    LwDictionaryModulePrivate * priv = NULL;

    //Initializations
    priv = self->priv;

    return g_module_symbol (priv->module, SYMBOL_NAME, symbol);
}


static gboolean 
_is_valid_modulename (gchar const * MODULENAME)
{
    return (g_str_has_prefix (MODULENAME, "lib") && g_str_has_suffix (MODULENAME, G_MODULE_SUFFIX));
}


/**
 * lw_dictionarymodulereader_open:
 * @SEARCHPATH_OVERRIDE: (transfer none): A path override or %NULL to use the environment or builtins
 *
 * Opens a reader that will iterate the search paths given in @SEARCHPATH_OVERRIDE.  If @SEARCHPATH_OVERRIDE
 * is %NULL, the reader then thecks if the DICTIONARYLIB_SEARCHPATH environmental variable is set.  If that
 * is not set, it falls back to the paths built in on compile time.
 * 
 * Returns: (transfer full): A new #LwDictionaryModuleReader that should be closed with lw_dictionarymodulereader_close()
 */
LwDictionaryModuleReader *
lw_dictionarymodulereader_open (gchar const * SEARCHPATH_OVERRIDE)
{
    LwDictionaryModuleReader * self = NULL;
    gchar ** paths = NULL;

    paths = lw_dictionarymodule_get_dictionarylib_searchpath (SEARCHPATH_OVERRIDE);
    if (paths == NULL) goto errored;

    self = g_new0 (LwDictionaryModuleReader, 1);
    if (self == NULL) goto errored;

    self->paths = paths;
    paths = NULL;

errored:

    g_free (paths);
    paths = NULL;

    return self;
}

/**
 * lw_dictionarymodulereader_close:
 * @self: A #LwDictionaryModuleReader
 *
 * Closes an open dictionary module reader and frees any related memory.
 */
void
lw_dictionarymodulereader_close (LwDictionaryModuleReader * self)
{
    if (self == NULL) return;

    g_strfreev (self->paths);

    if (self->dir != NULL) g_dir_close (self->dir);

    memset(self, 0, sizeof(LwDictionaryModuleReader));

    g_free (self);
}


/**
 * lw_dictionarymodulereader_read_path:
 * @self: A #LwDictionaryModuleReader
 *
 * Reads a path iteratively from the dictionary search paths on valid-looking
 * dictionary modules.  Once the reader has read all paths, it will start
 * returning %NULL.  When finished with the reader, it should be closed with
 * lw_dictionarymodulereader_close().
 *
 * Returns: The next path from the iterater until it reaches the end. Once all paths have been read this method will return %NULL.
 */
gchar *
lw_dictionarymodulereader_read_path (LwDictionaryModuleReader * self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    gchar const * NAME = NULL;
    gchar * path = NULL;

    while (path == NULL && (self->paths[self->i] != NULL || self->dir != NULL))
    {
      if (self->dir == NULL) self->dir = g_dir_open (self->paths[self->i], 0, NULL);

      if (self->dir != NULL)
      {
        if ((NAME = g_dir_read_name (self->dir)) == NULL)
        {
          self->i++;
          g_dir_close (self->dir);
          self->dir = NULL;
        }
        else if (_is_valid_modulename (NAME))
        {
          path = g_build_filename (self->paths[self->i], NAME, NULL);
          if (path != NULL && !g_file_test (path, G_FILE_TEST_IS_REGULAR))
          {
            g_free (path);
            path = NULL;
          }
        }
      }
      else
      {
        self->i++;
      }
    }

    return path;
}
