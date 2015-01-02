/******************************************************************************
    AUTHOR:
    File written and Copyrighted by Pauli Virtanen. All Rights Reserved.

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
//! @file mappedfile.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/mappedfile.h>

#include <libwaei/mappedfile-private.h>

G_DEFINE_TYPE (LwMappedFile, lw_mappedfile, G_TYPE_OBJECT)


LwMappedFile*
lw_mappedfile_new (gchar const * PATH)
{
		//Declarations
		LwMappedFile *self = NULL;
		GMappedFile *mapped_file = NULL;
		gchar *path = NULL;

		//Initializations
    self = LW_MAPPEDFILE (g_object_new (LW_TYPE_MAPPEDFILE, "path", PATH, NULL));
      
		return self;
}


static void 
lw_mappedfile_init (LwMappedFile *self)
{
    self->priv = LW_MAPPEDFILE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwMappedFilePrivate));
}


static void 
lw_mappedfile_finalize (GObject *object)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initalizations
    self = LW_MAPPEDFILE (object);
    priv = self->priv;

    if (priv->delete_on_free && priv->path != NULL)
    {
      g_remove (priv->path);
    }

    lw_mappedfile_set_path (self, NULL);

    memset(self->priv, 0, sizeof(LwMappedFilePrivate));

    G_OBJECT_CLASS (lw_mappedfile_parent_class)->finalize (object);
}


static void 
lw_mappedfile_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    self = LW_MAPPEDFILE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_PATH:
        lw_mappedfile_set_path (self, g_value_get_string (value));
        break;
      case PROP_DELETE_ON_FREE:
        lw_mappedfile_set_delete_on_free (self, g_value_get_boolean (value));
        break;
      case PROP_WRITABLE:
        lw_mappedfile_set_writable (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_mappedfile_get_property (GObject      *object,
                            guint         property_id,
                            GValue       *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    self = LW_MAPPEDFILE (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CONTENTS:
        g_value_set_string (value, lw_mappedfile_get_contents (self));
        break;
      case PROP_CONTENT_LENGTH:
        g_value_set_ulong (value, lw_mappedfile_length (self));
        break;
      case PROP_PATH:
        g_value_set_string (value, lw_mappedfile_get_path (self));
        break;
      case PROP_DELETE_ON_FREE:
        g_value_set_boolean (value, lw_mappedfile_get_delete_on_free (self));
        break;
      case PROP_WRITABLE:
        g_value_set_boolean (value, lw_mappedfile_get_writable (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_mappedfile_class_finalize (LwMappedFileClass *klass)
{
    memset(klass->priv, 0, sizeof(LwMappedFileClassPrivate));
    g_free (klass->priv);
    klass->priv = NULL;
}


static void
lw_mappedfile_class_init (LwMappedFileClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwMappedFileClassPrivate, 1);
    object_class->set_property = lw_mappedfile_set_property;
    object_class->get_property = lw_mappedfile_get_property;
    object_class->finalize = lw_mappedfile_finalize;

    g_type_class_add_private (object_class, sizeof (LwMappedFilePrivate));

    klass->priv->pspec[PROP_CONTENTS] = g_param_spec_string (
      "contents",
      gettext("Contents"),
      "The content string for the mapped file",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klass->priv->pspec[PROP_CONTENTS]);

    klass->priv->pspec[PROP_CONTENT_LENGTH] = g_param_spec_ulong (
      "content-length",
      gettext("Content Length"),
      "The length of the contents",
      0,
      G_MAXULONG,
      0,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENT_LENGTH, klass->priv->pspec[PROP_CONTENT_LENGTH]);

    klass->priv->pspec[PROP_PATH] = g_param_spec_string (
      "path",
      gettext("Path"),
      "The path of the mapped file",
      "",
      G_PARAM_CONSTRUCT | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_PATH, klass->priv->pspec[PROP_PATH]);

    klass->priv->pspec[PROP_DELETE_ON_FREE] = g_param_spec_boolean (
      "delete-on-free",
      gettext("Delete on Free"),
      "Delete the mapped file when the object is freed",
      FALSE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DELETE_ON_FREE, klass->priv->pspec[PROP_DELETE_ON_FREE]);
}


static GMappedFile *
_get_mapped_file (LwMappedFile * self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    GError *error = NULL;

    //Initializations
    priv = self->priv;
    if (priv->path == NULL) goto errored;

    if (priv->mapped_file == NULL)
    {
		  priv->mapped_file = g_mapped_file_new (priv->path, priv->writable, &error);
    }
    if (error != NULL) goto errored;

    mapped_file = priv->mapped_file;

errored:
    
    if (error != NULL)
    {
      g_clear_error (&error);
      error = NULL;

      if (priv->mapped_file != NULL)
      {
        g_mapped_file_unref (priv->mapped_file);
        priv->mapped_file = NULL;
        mapped_file = NULL;
      }
    }

    return mapped_file;
}


void
_clear_mapped_file (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    GError *error = NULL;

    //Initializations
    priv = self->priv;

    if (priv->path == NULL && priv->mapped_file != NULL)
    {
      g_mapped_file_unref (priv->mapped_file);
      priv->mapped_file = NULL;
    }
}


gsize
lw_mappedfile_length (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    mapped_file = _get_mapped_file (self);
    if (mapped_file == NULL) goto errored;
		length = g_mapped_file_get_length (mapped_file);

errored:

    return length;
}


gchar *
lw_mappedfile_get_contents (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *contents = NULL;

    //Initializations
    priv = self->priv;
    mapped_file = _get_mapped_file (self);
    if (mapped_file == NULL) goto errored;
    contents = g_mapped_file_get_contents (mapped_file);

errored:

    return contents;
}


void
lw_mappedfile_set_path (LwMappedFile * self,
                        gchar const  * PATH)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_MAPPEDFILE_GET_CLASS (self);

    if (g_strcmp0 (priv->path, PATH) == 0) goto errored;

    g_free (priv->path);
    priv->path = g_strdup (PATH);

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_PATH]);

errored:

    return; 
}


gchar const *
lw_mappedfile_get_path (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

		return priv->path;
}


void
lw_mappedfile_set_delete_on_free (LwMappedFile *self,
                                  gboolean delete_on_free)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_MAPPEDFILE_GET_CLASS (self);
    if (priv->delete_on_free == delete_on_free) goto errored;

    priv->delete_on_free = delete_on_free;

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_DELETE_ON_FREE]);

errored:

    return;
}


gboolean
lw_mappedfile_get_delete_on_free (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->delete_on_free;
}


static void
lw_mappedfile_set_writable (LwMappedFile *self,
                            gboolean      writable)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_MAPPEDFILE_GET_CLASS (self);
    if (writable == priv->writable) goto errored;
    priv->writable = writable;

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_WRITABLE]);

errored:

    return;
}


gboolean
lw_mappedfile_get_writable (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPEDFILE (self));
    
    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->writable;
}
                 
