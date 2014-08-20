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
//! @file dictionarybuffer.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>


LwDictionaryBuffer*
lw_dictionarybuffer_new (LwUtf8NormalizeFlag flags)
{
    //Sanity checks
    g_return_val_if_fail (BUFFER != NULL, NULL);

    //Declarations
    LwDictionaryBuffer *self = NULL;

    //Initializations
    self = LW_DICTIONARYBUFFER (g_object_new (LW_TYPE_DICTIONARYBUFFER,
      "normalization-flags", flags,
      NULL
    ));

    return self;
}


static void 
lw_dictionarybuffer_init (LwDictionaryBuffer *self)
{
    self->priv = LW_DICTIONARYBUFFER_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryBufferPrivate));

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
}


static void 
lw_dictionarybuffer_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryBuffer *self = NULL;
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYBUFFER (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_NORMALIZATION_FLAGS:
        lw_dictionarybuffer_set_flags (self, g_value_get_flags (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarybuffer_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    //Declarations
    LwDictionaryBuffer *self = NULL;
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYBUFFER (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CONTENTS:
        g_value_take_string (value, lw_dictionarybuffer_get_contents (self));
        break;
      case PROP_LENGTH:
        g_value_set_int (value, lw_dictionarybuffer_length (self));
        break;
      case PROP_NORMALIZATION_FLAGS:
        g_value_set_flags (value, lw_dictionarybuffer_get_flags (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionarybuffer_finalize (GObject *object)
{
    //Declarations
    LwDictionaryBuffer *self = NULL;
    LwDictionaryBufferPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYBUFFER (object);
    priv = self->priv;

    G_OBJECT_CLASS (lw_dictionarybuffer_parent_class)->finalize (object);
}


static void
lw_dictionarybuffer_dispose (GObject *object)
{
    //Declarations
    LwDictionaryBuffer *self = NULL;

    //Initializations
    self = LW_DICTIONARYBUFFER (object);

    G_OBJECT_CLASS (lw_dictionarybuffer_parent_class)->dispose (object);
}


static void
lw_dictionarybuffer_class_init (LwDictionaryBufferClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryBufferClassPrivate, 1);
    object_class->set_property = lw_dictionarybuffer_set_property;
    object_class->get_property = lw_dictionarybuffer_get_property;
    object_class->dispose = lw_dictionarybuffer_dispose;
    object_class->finalize = lw_dictionarybuffer_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryBufferPrivate));

    LwDictionaryBufferClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_CONTENTS] = g_param_spec_string (
        "buffer",
        "Buffer",
        "Internal String buffer",
        ""
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klasspriv->pspec[PROP_CONTENTS]);

    klasspriv->pspec[PROP_NORMALIZATION_FLAGS] = g_param_spec_flags (
        "normalization-flags",
        "Normalization Flags",
        "The flags that are set on normalizations",
        LW_TYPE_UTF8NORMALIZEFLAG, 
        0,
        G_PARAM_CONSTRUCT | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_NORMALIZATION_FLAGS, klasspriv->pspec[PROP_NORMALIZATION_FLAGS]);
}


gsize
lw_dictionarybuffer_length (LwDictionaryBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, 0);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->length;
}


void
_create_mapped_file (LwDictionaryBuffer *self,
                     const gchar        *CONTENTS,
                     gint                length)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (func != NULL);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_free (priv->filename); priv->filename = NULL;
    if (priv->fd > -1) g_close (priv->fd); priv->fd = -1;
    if (priv->mapped_file != NULL) g_mapped_file_unref (priv->mapped_file); priv->mapped_file = NULL;
    priv->contents = NULL;

    priv->filename = g_strdup ("LwDictionaryBuffer.XXXXXX");
    if (priv->filename == NULL) goto errored;

    priv->fd = g_mkstemp (priv->filename);
    if (priv->fd == -1) goto errored;
    lw_io_write_fd (priv->fd, priv->CONTENTS, length, progress);

    priv->mapped_file = g_mapped_file_new_from_fd (priv->fd, TRUE, &error)
    if (priv->mapped_file == NULL) goto errored;

    priv->contents = g_mapped_file_get_contents (priv->mapped_file);
    if (priv->contents == NULL) goto errored;

errored:

    return;
}


void
lw_dictionarybuffer_set_contents (LwDictionaryBuffer          *self,
                                  const gchar                 *CHECKSUM
                                  const gchar                 *CONTENTS
                                  gint                         length,
                                  LwDictionaryBufferParseFunc  func,
                                  gpointer                     data)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;
    const gchar *NORMALIZED_CONTENTS = NULL;

    //Initializations
    priv = self->priv;

    while (chunk)
    {
      lw_utf8_normalize(string, 4KiB, FLAGS);
    }

    lw_cachefile_new ();
    if (!lw_cachefile_read())
    {
      lw_cachefile_write();
      lw_cachefile_read();
    }



    lines = func (self, data, &num_lines);
    lw_dictionarybuffer_set_lines (self, lines, num_lines);

errored:

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CONTENTS]);

    return;
}


gchar*
lw_dictionarybuffer_get_contents (LwDictionaryBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents;
}


void
lw_dictionarybuffer_set_flags (LwDictionaryBuffer  *self,
                               LwUtf8NormalizeFlag  flags)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    flags = lw_utf8normalizeflags_clean (flags);
    if (flags == priv->flags) goto errored;
    priv->flags = flags;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_NORMALIZATION_FLAGS]);

    lw_dictionarybuffer_sync_buffer (self);

errored:

    return;
}


LwUtf8NormalizeFlag
lw_dictionarybuffer_get_flags (LwDictionaryBuffer *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryBufferPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->flags;
}

