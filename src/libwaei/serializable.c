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
//! @file serializable.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <glib-object.h>

#include <libwaei/gettext.h>
#include <libwaei/io.h>
#include <libwaei/serializable.h>

#include <libwaei/serializable-private.h>


G_DEFINE_ABSTRACT_TYPE (LwSerializable, lw_serializable, G_TYPE_OBJECT);


static void 
lw_serializable_init (LwSerializable *self)
{
    self->priv = LW_SERIALIZABLE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwSerializablePrivate));
}


static void 
lw_serializable_finalize (GObject *object)
{
    //Declarations
    LwSerializable *self = NULL;
    LwSerializablePrivate *priv = NULL;

    //Initalizations
    self = LW_SERIALIZABLE (object);
    priv = self->priv;

    if (priv->cachefile != NULL)
    {
      lw_cachefile_unref (priv->cachefile);
      priv->cachefile = NULL;
    }

    memset(self->priv, 0, sizeof(LwSerializablePrivate));

    G_OBJECT_CLASS (lw_serializable_parent_class)->finalize (object);
}


static void
lw_serializable_class_init (LwSerializableClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_serializable_finalize;

    g_type_class_add_private (object_class, sizeof (LwSerializablePrivate));
}


gsize
lw_serializable_get_serialized_length (LwSerializable *self,
                                       LwProgress     *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 0);
    if (lw_progress_should_abort (progress)) return 0;
 
    return lw_serializable_serialize (self, NULL, progress);
}


gsize
lw_serializable_serialize (LwSerializable *self,
                           gchar          *preallocated_buffer,
                           LwProgress     *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SERIALIZABLE (self));
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 0);
    if (lw_progress_should_abort (progress)) return 0;

    //Declarations
    LwSerializablePrivate *priv = NULL;
    LwSerializableClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SERIALIZABLE_CLASS (self);

    return klass->serialize (self, preallocated_buffer, progress);
}


gsize
lw_serializable_serialize_to_cachefile (LwSerializable *self,
                                        gchar const    *CHECKSUM,
                                        LwCacheFile    *cachefile,
                                        LwProgress     *progress)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);
    g_return_val_if_fail (CHECKSUM != NULL, 0);
    g_return_val_if_fail (cachefile != NULL, 0);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 0);
    if (lw_progress_should_abort (progress)) return 0;

    //Declarations
    LwSerializablePrivate *priv = NULL;
    gsize length = 0;
    gchar *tmp_path = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *contents = NULL;
    GError *error = NULL;
    gboolean has_error = FALSE;

    //Initializations
    priv = self->priv;
    length = lw_serializable_get_serialized_length (self, progress);
    tmp_path = lw_io_allocate_temporary_file (length, progress);
    mapped_file = g_mapped_file_new (tmp_path, TRUE, &error);
    if (error != NULL)
    {
      lw_progress_take_error (progress, error);
      error = NULL;
      has_error = TRUE;
      goto errored;
    }
    contents = g_mapped_file_get_contents (mapped_file);

    lw_serializable_serialize (self, contents, progress);
    lw_cachefile_write (cachefile, CHECKSUM, contents, length, progress);

errored:

    g_remove (tmp_path);
    g_free (tmp_path); tmp_path = NULL;
    g_mapped_file_unref (mapped_file); mapped_file = NULL;
    g_clear_error (&error);
    contents = NULL;
}


gsize
lw_serializable_deserialize_from (LwSerializable *self,
                                  gchar const    *serialized_data,
                                  gsize           serialized_length,
                                  LwProgress     *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_SERIALIZABLE (self));
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 0);
    if (lw_progress_should_abort (progress)) return 0;

    //Declarations
    LwSerializablePrivate *priv = NULL;
    LwSerializableClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_SERIALIZABLE_CLASS (self);

    klass->deserialize_into (self, serialized_data, serialized_length, progress);

    lw_cachefile_unref (priv->cachefile);
    priv->cachefile = NULL;
}


gsize
lw_serializable_deserialize_from_cachefile (LwSerializable *self,
                                            gchar const    *EXPECTED_CHECKSUM,
                                            LwCacheFile    *cachefile,
                                            LwProgress     *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_SERIALIZABLE (self), 0);
    g_return_val_if_fail (EXPECTED_CHECKSUM != NULL, 0);
    g_return_val_if_fail (cachefile != NULL, 0);
    g_return_val_if_fail (LW_IS_PROGRESS (progress), 0);
    if (lw_progress_should_abort (progress)) return 0;

    //Declarations
    LwSerializablePrivate *priv = NULL;
    gchar *content = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    content = lw_cachefile_read (cachefile, EXPECTED_CHECKSUM, progress);
    if (content == NULL) goto errored;
    length = lw_cachefile_length (cachefile);
    if (length == 0) goto errored;

    lw_serializable_deserialize_from (self, content, length, progress);

    priv->cachefile = lw_cachefile_ref (cachefile);

errored:

    return length;
}

