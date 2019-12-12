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

/**
 * SECTION:parsed
 * @short_description: A container for parsed lines
 * @title: LwParsed
 *
 * This is an object made for management of a list of
 * parsedlines representing a document.
 */

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

G_DEFINE_TYPE (LwParsed, lw_parsed, LW_TYPE_SERIALIZABLE)


struct _SerializeData {
  gchar * buffer;
  gchar * write_pointer;
  LwProgress * progress;
  GError * error;
  gsize chunk;
  gsize chunk_size;
};

struct _DeserializeData {
  gchar const * serialized_data;
  gchar const * read_pointer;
  LwProgress * progress;
  GError * error;
  gsize chunk;
  gsize chunk_size;
};


GQuark
lw_parsed_error_quark ()
{
    return g_quark_from_static_string ("lw-parsed-error");
}


/**
 * lw_parsed_new:
 * @cache_file: The base data to reference
 * Returns: (transfer full): A new #LwParsed object that can be freed with g_object_unref()
 */
LwParsed*
lw_parsed_new (LwCacheFile *cache_file)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_CACHEFILE (cache_file), NULL);

    //Declarations
    LwParsed *self = NULL;

    //Initializations
    self = LW_PARSED (g_object_new (LW_TYPE_PARSED, "cache-file", cache_file, NULL));

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
    lw_parsed_set_cachefile (self, NULL);

    G_OBJECT_CLASS (lw_parsed_parent_class)->finalize (object);
}


static void 
lw_parsed_set_property (GObject      * object,
                        guint          property_id,
                        const GValue * value,
                        GParamSpec   * pspec)
{
    //Declarations
    LwParsed *self = NULL;
    LwParsedPrivate *priv = NULL;

    //Initializations
    self = LW_PARSED (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CACHEFILE:
        lw_parsed_set_cachefile (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_parsed_get_property (GObject      * object,
                        guint          property_id,
                        GValue       * value,
                        GParamSpec   * pspec)
{
    //Declarations
    LwParsed *self = NULL;
    LwParsedPrivate *priv = NULL;

    //Initializations
    self = LW_PARSED (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_CACHEFILE:
        g_value_set_object (value, lw_parsed_get_cachefile (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_parsed_class_init (LwParsedClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwSerializableClass *serializable_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_parsed_finalize;
    object_class->set_property = lw_parsed_set_property;
    object_class->get_property = lw_parsed_get_property;

    serializable_class = LW_SERIALIZABLE_CLASS (klass);
    serializable_class->serialize = lw_parsed_serialize;
    serializable_class->deserialize_into = lw_parsed_deserialize_into;

    klass->priv = g_new0 (LwParsedClassPrivate, 1);

    klass->priv->pspec[PROP_CACHEFILE] = g_param_spec_object (
      "cache-file",
      gettext("Contents Mapped File"),
      "Contents of the parsed object. This data is not necessarily human readable.",
      LW_TYPE_CACHEFILE,
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CACHEFILE, klass->priv->pspec[PROP_CACHEFILE]);

    g_type_class_add_private (object_class, sizeof (LwParsedPrivate));
}


/**
 * lw_parsed_foreach:
 * @self: The #LwParsed
 * @func: The function to run for each #LwParsedLine
 * @data: Data to pass to the @func
 */
void
lw_parsed_foreach (LwParsed            * self,
                   LwParsedForeachFunc   func,
                   gpointer              data)
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
      has_error = func (self, priv->lines + i, data);
      i++;
    }
}


/**
 * lw_parsed_get_line:
 * @self: A #LwParsed
 * @line_number: The line number to fetch
 * Returns: (no transfer): A #LwParsedLine or %NULL if the index is invalid
 */
LwParsedLine*
lw_parsed_get_line (LwParsed * self,
                    gsize      line_number)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), NULL);
    g_return_val_if_fail (line_number < self->priv->num_lines, NULL);
    g_return_val_if_fail (line_number >= 0, NULL);

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


/**
 * lw_parsed_set_lines:
 * @self: A #LwParsed
 * @lines: (transfer full): An array of lines to set
 * @num_lines: The number of lines in @lines
 */
void
lw_parsed_set_lines (LwParsed         * self,
                     LwParsedLine     * lines,
                     gsize              num_lines)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (self));

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
    g_free (priv->lines); priv->lines = NULL;
    priv->num_lines = 0;

    priv->lines = lines;
    priv->num_lines = num_lines;

errored:

    return;
}


/**
 * lw_parsed_get_lines:
 * @self: A #LwParsed
 * @num_lines_out: (transfer out): A pointer to write the number of lines or %NULL
 * Returns: an array of #LwParsedLines or %NULL
 */
LwParsedLine *
lw_parsed_get_lines (LwParsed * self,
                     gsize    * num_lines_out)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (num_lines_out != NULL) *num_lines_out = priv->num_lines;

    return priv->lines;
}


/**
 * lw_parsed_num_lines:
 * @self: A #LwParsed
 * Returns: The number of lines stored in #LwParsed
 */
gsize
lw_parsed_num_lines (LwParsed * self)
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
_serialize (LwParsed              * self,
            LwParsedLine          * parsed_line,
            struct _SerializeData * data)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), TRUE);
    g_return_val_if_fail (parsed_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);
    if (data->progress != NULL && lw_progress_should_abort (data->progress)) return TRUE;

    //Declarations
    LwParsedPrivate *priv = NULL;
    gsize bytes_written = 0;
    gchar * contents = NULL;
    gchar * write_pointer = NULL;

    //Initializations
    priv = self->priv;
    if (data->buffer != NULL)
    {
      contents = lw_cachefile_get_contents (priv->cache_file);
      write_pointer = data->write_pointer;
    }
    bytes_written = lw_parsedline_serialize (parsed_line, contents, write_pointer, &data->error);
    data->write_pointer += bytes_written;

    if (data->progress != NULL)
    {
      data->chunk += bytes_written;
      if (G_UNLIKELY (data->chunk >= data->chunk_size))
      {
        lw_progress_set_current (data->progress, data->write_pointer - data->buffer);
        data->chunk = 0;
      }
    }

errored:

    return (data->error != NULL);
}


static gsize
lw_parsed_serialize (LwSerializable * serializable,
                     gchar          * preallocated_buffer,
                     LwProgress     * progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (serializable));

    //Declarations
    LwParsed * self = LW_PARSED (serializable);
    LwParsedPrivate *priv = NULL;
    struct _SerializeData data = {
      .buffer = preallocated_buffer,
      .write_pointer = preallocated_buffer,
      .progress = progress,
      .chunk = 0,
      .chunk_size = 0,
    };
    if (progress != NULL)
    {
      data.chunk_size = lw_progress_get_chunk_size (progress);
    }

    priv = self->priv;

    //Copy the number of LwParsedLines
    if (data.buffer != NULL) memcpy(data.write_pointer, &priv->num_lines, sizeof(gsize));
    data.write_pointer += sizeof(gsize);

    if (progress != NULL)
    {
      if (data.buffer == NULL)
      {
        lw_progress_set_secondary_message (progress, "Calculating serialized size..");
      }
      else
      {
        lw_progress_set_secondary_message (progress, "Serializing...");
      }

      gsize total = data.write_pointer - data.buffer + (priv->num_lines * sizeof(LwParsedLine));
      lw_progress_set_total (progress, total);
      lw_progress_set_current (progress, 0);
      lw_progress_set_current (progress, data.write_pointer - data.buffer);
    }

    //Copy the individual seriallized LwParsedLine contents
    lw_parsed_foreach (self, (LwParsedForeachFunc) _serialize, &data);

errored:

    return data.write_pointer - data.buffer;
}


static gboolean
_deserialize (LwParsed                * self,
              LwParsedLine            * parsed_line,
              struct _DeserializeData * data)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), TRUE);
    g_return_val_if_fail (parsed_line != NULL, TRUE);
    g_return_val_if_fail (data != NULL, TRUE);
    if (data->progress != NULL && lw_progress_should_abort (data->progress)) return TRUE;

    //Declarations
    LwParsedPrivate *priv = NULL;
    gsize bytes_read = 0;
    gchar const *contents = NULL;
    gchar const *read_pointer = NULL;

    //Initializations
    priv = self->priv;
    contents = lw_cachefile_get_contents (priv->cache_file);
    read_pointer = data->read_pointer;
    bytes_read = lw_parsedline_deserialize_into (parsed_line, read_pointer, contents, &data->error);
    if (data->error != NULL)
    {
      goto errored;
    }

    data->read_pointer += bytes_read;

    if (data->progress != NULL)
    {
      data->chunk += bytes_read;
      if (G_UNLIKELY (data->chunk >= data->chunk_size))
      {
        lw_progress_set_current (data->progress, data->read_pointer - data->serialized_data);
        data->chunk = 0;
      }
    }

errored:

    return (data->error != NULL);
}


static gsize
lw_parsed_deserialize_into (LwSerializable * serializable,
                            gchar const    * serialized_data,
                            gsize            serialized_length,
                            LwProgress     * progress)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (serializable), 0);
    g_return_val_if_fail (serialized_data != NULL, 0);

    /*TODO
    [num_lines (lwoffset)] [line]...
    type checking here
    */

    //Declarations
    LwParsed * self = LW_PARSED (serializable);
    LwParsedPrivate *priv = NULL;
    LwParsedLine *lines = NULL;
    gsize num_lines = 0;
    struct _DeserializeData data = {
      .serialized_data = serialized_data,
      .read_pointer = serialized_data,
      .progress = progress,
      .error = NULL,
      .chunk = 0,
      .chunk_size = 0,
    };
    if (progress != NULL)
    {
      data.chunk_size = lw_progress_get_chunk_size (progress);
    }

    //Initializations
    priv = self->priv;

    if (priv->lines != NULL || priv->num_lines > 0) goto errored;

    memcpy(&num_lines, data.read_pointer, sizeof(gsize));
    data.read_pointer += sizeof(gsize);

    if (progress != NULL)
    {
      lw_progress_set_total (progress, serialized_length);
      lw_progress_set_current (progress, 0);
      lw_progress_set_current (progress, data.read_pointer - data.serialized_data);
      lw_progress_set_secondary_message (progress, "Deserializing...");
    }

    lines = g_new0 (LwParsedLine, num_lines);
    lw_parsed_set_lines (self, lines, num_lines);
    lines = NULL;

    lw_parsed_foreach (self, (LwParsedForeachFunc) _deserialize, &data);
    if (data.error != NULL) goto errored;

    if (data.read_pointer - data.serialized_data != serialized_length)
    {
      g_set_error (
        &data.error,
        LW_PARSED_ERROR,
        LW_PARSED_ERRORCODE_DESERIALIZATION_ERROR,
        "Expected %d lines, but file had %d lines", 
        data.read_pointer - data.serialized_data, serialized_length
      );
      goto errored;
    }

errored:

    if (data.error != NULL)
    {
      lw_parsed_set_lines (self, NULL, 0);
      if (progress != NULL)
      {
        lw_progress_take_error (progress, data.error);
        data.error = NULL;
      }
      g_clear_error (&data.error);
    }

    return data.read_pointer - data.serialized_data;
}


static void
lw_parsed_set_cachefile (LwParsed    * self,
                         LwCacheFile * cache_file)
{
    //Sanity checks
    g_return_if_fail (LW_IS_PARSED (self));

    //Declarations
    LwParsedPrivate *priv = NULL;
    LwParsedClass *klass = NULL;

    //Initializations
    priv = self->priv;
    klass = LW_PARSED_GET_CLASS (self);
    if (cache_file == priv->cache_file) goto errored;

    if (cache_file != NULL)
    {
      g_object_ref (cache_file);
    }

    if (priv->cache_file != NULL)
    {
      g_object_unref (priv->cache_file);
    }

    priv->cache_file = cache_file;

    g_object_notify_by_pspec (G_OBJECT (self), klass->priv->pspec[PROP_CACHEFILE]);

errored:

    return;
}


/**
 * lw_parsed_get_cachefile:
 * @self: A #LwParsed
 * Returns: (transfer none): The #LwCacheFile that the #LwParsed was initialized with
 */
LwCacheFile*
lw_parsed_get_cachefile (LwParsed * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_PARSED (self), NULL);

    //Declarations
    LwParsedPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->cache_file;
}

