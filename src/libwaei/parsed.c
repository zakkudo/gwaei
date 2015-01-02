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
//! @file parsed.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include <libwaei/gettext.h>
#include <libwaei/parsed.h>

#include <libwaei/parsed-private.h>

G_DEFINE_TYPE (LwParsed, lw_parsed, LW_TYPE_PARSED)

struct _SerializeData {
  gsize *buffer;
  gsize *write_pointer;
  gsize bytes_written;
  GError *error;
};

struct _DeserializeData {
  gchar const *serialized_data;
  gchar const *read_pointer;
  gsize bytes_read;
  GError *error;
};


LwParsed*
lw_parsed_new (LwMappedFile *contents_mappedfile)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPEDFILE (contents_mappedfile), NULL);

    //Declarations
    LwParsed *self = NULL;

    //Initializations
    self = LW_PARSED (g_object_new (LW_TYPE_PARSED, "contents-mappedfile", contents_mappedfile, NULL));

    return self;
}


static void 
lw_parsed_init (LwParsed *self)
{
    self->priv = LW_PARSED_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwParsedPrivate));
}


static void 
lw_parsed_finalize (GObject *object)
{
    //Declarations
    LwParsed *self = NULL;
    LwParsedPrivate *priv = NULL;

    //Initializations
    self = LW_PARSED (object);
    priv = self->priv;

    lw_parsed_set_lines (self, NULL, 0);
    lw_parsed_set_contents_mappedfile (self, NULL);

    G_OBJECT_CLASS (lw_parsed_parent_class)->finalize (object);
}


static void 
lw_parsed_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    //Declarations
    LwParsed *self = NULL;
    LwParsedPrivate *priv = NULL;

    //Initializations
    self = LW_PARSED (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CONTENTS_MAPPEDFILE:
        lw_parsed_set_contents_mappedfile (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_parsed_get_property (GObject      *object,
                        guint         property_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
    //Declarations
    LwParsed *self = NULL;
    LwParsedPrivate *priv = NULL;

    //Initializations
    self = LW_PARSED (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CONTENTS_MAPPEDFILE:
        g_value_set_object (value, lw_parsed_get_contents_mappedfile (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_parsed_class_init (LwParsedClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_parsed_finalize;

    klass->priv->pspec[PROP_CONTENTS_MAPPEDFILE] = g_param_spec_object (
      "contents-mappedfile",
      gettext("Contents Mapped File"),
      "Contents of the parsed object. This data is not necessarily human readable.",
      LW_TYPE_MAPPEDFILE,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS_MAPPEDFILE, klass->priv->pspec[PROP_CONTENTS_MAPPEDFILE]);

    klass->priv->pspec[PROP_CONTENTS] = g_param_spec_string (
      "contents",
      gettext("Contents"),
      "Contents of the parsed object. This data is not necessarily human readable.",
      "",
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENTS, klass->priv->pspec[PROP_CONTENTS]);

    klass->priv->pspec[PROP_CONTENT_LENGTH] = g_param_spec_ulong (
      "content-length",
      gettext("Content Length"),
      "Contents of the parsed object. This data is not necessarily human readable.",
      0,
      G_MAXULONG,
      0,
      G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_CONTENT_LENGTH, klass->priv->pspec[PROP_CONTENT_LENGTH]);
}


void
lw_parsed_foreach (LwParsed            *self,
                   LwParsedForeachFunc  func,
                   gpointer             data)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (self));
    g_return_if_fail (func != NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;
    gint i = 0;
    gboolean has_error = FALSE;

    //Initializations
    priv = self->priv;

    while (i < priv->num_lines && !has_error)
    {
      has_error = func(self, priv->lines + i, data);
      i++;
    }
}


LwParsedLine*
lw_parsed_get_line (LwParsed *self,
                    gsize     line_number)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), NULL);
    g_return_val_if_fail (line_number > self->priv->num_lines, NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;
    LwParsedLine *line = NULL;

    //Inititalizations
    priv = self->priv;
    if (line_number < 0 || line_number >= priv->num_lines) goto errored;
    line = priv->lines + line_number;

errored:

    return line;
}


void
lw_parsed_set_lines (LwParsed         *self,
                     LwParsedLine *lines,
                     gsize             num_lines)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (lines == priv->lines) goto errored;

    {
      gint i = 0;
      for (i = 0; i < priv->num_lines; i++)
      {
        lw_parsedline_clear (priv->lines + i);
      }
    } 
    g_free (priv->lines); priv->lines = 0;
    priv->num_lines = 0;

    priv->lines = lines;
    priv->num_lines = num_lines;

errored:

    return;
}


gsize
lw_parsed_num_lines (LwParsed *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), 0);

    //Declarations
    LwParsedPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->num_lines;
}


static gboolean
_serialize (LwParsed              *self,
            LwParsedLine      *parsed_line,
            struct _SerializeData *data)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), TRUE);
    g_return_val_if_fail (parsed_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);

    //Declarations
    LwParsedPrivate *priv = NULL;
    gsize bytes_written = 0;
    gchar *contents = NULL;
    gchar *write_pointer = NULL;

    //Initializations
    priv = self->priv;
    write_pointer = (gchar*) data->write_pointer;
    contents = priv->contents;
    bytes_written = lw_parsedline_serialize (parsed_line, contents, write_pointer, &data->error);
    if (data->error != NULL)
    {
      goto errored;
    }
    data->bytes_written += bytes_written;
    write_pointer += bytes_written;
    data->write_pointer = (gsize*) write_pointer;

errored:

    return (data->error != NULL);
}


static gsize
lw_parsed_serialize (LwParsed   *self,
                     gchar      *preallocated_buffer,
                     LwProgress *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (self));
    g_return_if_fail (preallocated_buffer != NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;
    struct _SerializeData data = {
      .buffer = (gsize*) preallocated_buffer,
      .write_pointer = (gsize*) preallocated_buffer,
      .bytes_written = 0
    };

    //Copy the number of LwParsedLines
    priv = self->priv;
    *(data.write_pointer++) = priv->num_lines;
    data.bytes_written += sizeof(gsize);

    //Copy the individual seriallized LwParsedLine contents
    lw_parsed_foreach (self, (LwParsedForeachFunc) _serialize, &data);

errored:

    return data.bytes_written;
}


static gboolean
_deserialize (LwParsed                *self,
              LwParsedLine        *parsed_line,
              struct _DeserializeData *data)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), TRUE);
    g_return_val_if_fail (parsed_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);

    //Declarations
    LwParsedPrivate *priv = NULL;
    gsize bytes_read = 0;
    gchar const *contents = NULL;
    gchar const *read_pointer = NULL;

    //Initializations
    priv = self->priv;
    contents = priv->contents;
    read_pointer = data->read_pointer;
    bytes_read = lw_parsedline_deserialize_into (parsed_line, read_pointer, contents, &data->error);
    if (data->error != NULL)
    {
      goto errored;
    }

errored:

    data->bytes_read += bytes_read;
    data->read_pointer += bytes_read;

    return (data->error != NULL);
}


static gsize
lw_parsed_deserialize_into (LwParsed    *self,
                            const gchar *serialized_data,
                            gsize        serialized_length,
                            LwProgress  *progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), 0);
    g_return_val_if_fail (serialized_data != NULL, 0);

    /*TODO
    [num_lines (lwoffset)] [line]...
    type checking here
    */

    //Declarations
    LwParsedPrivate *priv = NULL;
    LwParsedLine *lines = NULL;
    gsize num_lines = 0;
    gsize bytes_read = 0;
    struct _DeserializeData data = {
      .serialized_data = serialized_data,
      .read_pointer = serialized_data,
      .bytes_read = 0,
      .error = NULL
    };

    //Initializations
    priv = self->priv;
    num_lines = *((gsize*) data.read_pointer);
    data.read_pointer += sizeof(gsize);
    lines = g_new0(LwParsedLine, num_lines);

    lw_parsed_foreach (self, (LwParsedForeachFunc) _deserialize, &data);
    if (data.error != NULL)
    {
      if (progress != NULL)
      {
        lw_progress_take_error (progress, data.error);
        data.error = NULL;
      }
      g_clear_error (&data.error);
      goto errored;
    }

    lw_parsed_set_lines (self, lines, num_lines);
    lines = NULL;

errored:

    if (lines != NULL)
    {
      gint i = 0;
      for (i = 0; i < num_lines; i++)
      {
        lw_parsedline_clear (lines + i);
      }
      g_free (lines); lines = NULL;
    }

    return bytes_read;
}


static void
lw_parsed_set_contents_mappedfile (LwParsed     * self,
                                   LwMappedFile * contents_mappedfile)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (self));
    g_return_if_fail (LW_IS_MAPPEDFILE (contents_mappedfile));

    //Declarations
    LwParsedPrivate *priv = NULL;
    LwParsedClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_PARSED_GET_CLASS (self);
    if (contents_mappedfile == priv->contents_mappedfile) goto errored;

    if (contents_mappedfile != NULL)
    {
      g_object_ref (contents_mappedfile);
    }

    if (priv->contents_mappedfile != NULL)
    {
      g_object_unref (priv->contents_mappedfile);
      priv->contents = NULL;
      priv->content_length = 0;
    }

    priv->contents_mappedfile = priv->contents_mappedfile;

    if (priv->contents_mappedfile != NULL)
    {
      priv->contents = lw_mappedfile_get_contents (priv->contents_mappedfile);
      priv->content_length = lw_mappedfile_length (priv->contents_mappedfile);
    }

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CONTENTS_MAPPEDFILE]);

errored:

    return;
}


LwMappedFile*
lw_parsed_get_contents_mappedfile (LwParsed * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), TRUE);

    //Declarations
    LwParsedPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->contents_mappedfile;
}

