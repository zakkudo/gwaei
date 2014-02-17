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
//!  @file vocabulary.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/vocabulary-private.h>

static LwVocabularyClass *_klass = NULL;
static LwVocabularyClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE (LwVocabulary, lw_vocabulary, G_TYPE_OBJECT)

gchar**
lw_vocabulary_get_filenames ()
{
    //Declarations
    GDir *dir = NULL;
    gchar **filenames = NULL;
    const gchar *FILENAME = NULL;
    gint length = 0;
    gchar *uri = NULL;

    //Initializations
    uri = lw_util_build_filename (LW_PATH_VOCABULARY, NULL);
    if (uri == NULL) goto errored;
    dir = g_dir_open (uri, 0, NULL);
    if (dir == NULL) goto errored;

    //Get the size needed for the buffer
    while (g_dir_read_name (dir) != NULL) length++;
    g_dir_rewind (dir);
    if (length == 0) goto errored;

    filenames = g_new0(gchar*, length + 1);

    {
      gint i = 0;
      while ((FILENAME = g_dir_read_name (dir)) != NULL && i < length)
      {
        filenames[i] = g_strdup (FILENAME);
      }
    }

errored:

    if (dir != NULL) g_dir_close (dir); dir = NULL;
    if (uri != NULL) g_free (uri); uri = NULL;

    return filenames;
}



//!
//! @brief Creates a new LwDictionaryList object
//! @param MAX The maximum items you want in the dictionary before old ones are deleted
//! @return An allocated LwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LwVocabulary* 
lw_vocabulary_new (const gchar *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (FILENAME != NULL, NULL);
    
    LwVocabulary *vocabulary = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (g_object_new (LW_TYPE_VOCABULARY, "filename", FILENAME, NULL));

    return vocabulary;
}


static void 
lw_vocabulary_init (LwVocabulary *self)
{
    self->priv = LW_VOCABULARY_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwVocabularyPrivate));
}


static void 
lw_vocabulary_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwVocabulary *self = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    self = LW_VOCABULARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        lw_vocabulary_set_filename (self, g_value_get_string (value));
        break;
      case PROP_CHANGED:
        lw_vocabulary_set_changed (self, g_value_get_boolean (value));
        break;
      case PROP_LOADED:
        lw_vocabulary_set_loaded (self, g_value_get_boolean (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_vocabulary_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    //Declarations
    LwVocabulary *self = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    self = LW_VOCABULARY (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        g_value_set_string (value, lw_vocabulary_get_filename (self));
        break;
      case PROP_CHANGED:
        g_value_set_boolean (value, lw_vocabulary_has_changes (self));
        break;
      case PROP_LOADED:
        g_value_set_boolean (value, lw_vocabulary_is_loaded (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_vocabulary_finalize (GObject *object)
{
    //Declarations
    LwVocabulary *self = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initalizations
    self = LW_VOCABULARY (object);
    priv = self->priv;

    if (priv->config.filename != NULL) g_free (priv->config.filename); priv->config.filename = NULL;

    if (priv->data.list != NULL)
    {
      g_list_free_full (priv->data.list, (GDestroyNotify) lw_word_free);
      priv->data.list = NULL;
    }

    G_OBJECT_CLASS (lw_vocabulary_parent_class)->finalize (object);
}


static void
lw_vocabulary_dispose (GObject *object)
{
    //Declarations
    LwVocabulary *self = NULL;

    //Initializations
    self = LW_VOCABULARY (object);

    G_OBJECT_CLASS (lw_vocabulary_parent_class)->dispose (object);
}


static void
lw_vocabulary_initialize_filename_suffix ()
{
    //Sanity checks
    g_return_if_fail (_klasspriv != NULL);

    //Declarations
    gchar **filenames = NULL;
    gint length = 0;
    gint i = 0;

    //Initializations
    _klasspriv->new_filename_index = 0;
    _klasspriv->BASE_FILENAME = "New List ";

    length = strlen(_klasspriv->BASE_FILENAME);
    filenames = lw_vocabulary_get_filenames ();
    if (filenames == NULL) goto errored;


    for (i = 0; filenames[i] != NULL; i++)
    {
      if (strncmp(filenames[i], _klasspriv->BASE_FILENAME, length) == 0)
      {
        const gchar* FILENAME = filenames[i];
        const gchar* SUFFIX = FILENAME + length;
        gchar* endptr = NULL;
        gint index = (gint) strtol(SUFFIX, &endptr, 10);
        if (endptr != NULL && *endptr == '\0' && index > _klasspriv->new_filename_index) {
          _klasspriv->new_filename_index = index;
        }
      }
    }

errored:

    if (filenames != NULL) g_strfreev (filenames); filenames = NULL;

    _klasspriv->new_filename_index++;
}


static void
lw_vocabulary_class_init (LwVocabularyClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwVocabularyClassPrivate, 1);
    klasspriv = klass->priv;
    object_class->set_property = lw_vocabulary_set_property;
    object_class->get_property = lw_vocabulary_get_property;
    object_class->dispose = lw_vocabulary_dispose;
    object_class->finalize = lw_vocabulary_finalize;

    g_type_class_add_private (object_class, sizeof (LwVocabularyPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    lw_vocabulary_initialize_filename_suffix ();

    klasspriv->signalid[CLASS_SIGNALID_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    klasspriv->signalid[CLASS_SIGNALID_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    klasspriv->pspec[PROP_FILENAME] = g_param_spec_string (
        "filename",
        "FIlename construct prop",
        "Set the filename",
        gettext("New Vocabulary List"),
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, klasspriv->pspec[PROP_FILENAME]);

    klasspriv->pspec[PROP_CHANGED] = g_param_spec_boolean (
        "changed",
        "changed construct prop",
        "Set the changed",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CHANGED, klasspriv->pspec[PROP_CHANGED]);

    klasspriv->pspec[PROP_LOADED] = g_param_spec_boolean (
        "loaded",
        "loaded construct prop",
        "Set the loaded",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_LOADED, klasspriv->pspec[PROP_LOADED]);
}


static void
lw_vocabulary_load_from_file (LwVocabulary       *self, 
                              LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (self));
    if (lw_vocabulary_is_loaded (self)) return;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gchar const *FILENAME = NULL;
    gchar *uri = NULL;
    gchar *contents = NULL;
    gsize length = 0;

    //Initializations
    priv = self->priv;
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    uri = lw_util_build_filename (LW_PATH_VOCABULARY, FILENAME);
    if (uri == NULL) goto errored;

    if (!g_file_get_contents (uri, &contents, &length, NULL) || length == 0) goto errored;

    if (length > 0)
    {
      lw_util_replace_linebreaks_with_nullcharacter (contents);

      gchar *c = contents;
      while (c != NULL && c < contents + length) 
      {
        LwWord *word = lw_word_new_from_string (c);
        if (word != NULL)
        {
          priv->data.list = g_list_prepend (priv->data.list, word);
        }
        c += strlen(c) + 1;
      }
    }

errored:

    if (contents != NULL) g_free (contents); contents = NULL;
    if (uri != NULL) g_free (uri); uri = NULL;
}


void
lw_vocabulary_load (LwVocabulary       *self, 
                    LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (self));
    if (lw_vocabulary_is_loaded (self)) return;

    //Declarations
    LwVocabularyPrivate *priv = self->priv;
    LwVocabularyClass *klass = LW_VOCABULARY_GET_CLASS (self);
    LwVocabularyClassPrivate *klasspriv = klass->priv;

    lw_vocabulary_load_from_file (self, cb);

    //Update the list information
    priv->data.length = -1;
    priv->data.list = g_list_reverse (priv->data.list);

    {
      gint i = 0;
      gint length = lw_vocabulary_length (self);
      for (i = 0; i < length; i++)
      {
        g_signal_emit (self, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, i);
      }
    }

    lw_vocabulary_set_changed (self, FALSE);
    lw_vocabulary_set_loaded (self, TRUE);
}

void
lw_vocabulary_save (LwVocabulary       *self, 
                    LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    const gchar *FILENAME = NULL;
    gint i = 0;
    gchar *uri = NULL;
    FILE *stream = NULL;

    //Initializations
    priv = self->priv;
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    uri = lw_util_build_filename (LW_PATH_VOCABULARY, FILENAME);
    if (uri == NULL) goto errored;
    stream = g_fopen (uri, "w");
    if (stream = NULL) goto errored;

    {
      GList *link = priv->data.list;
      while (link != NULL)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
          for (i = 0; i < TOTAL_LW_WORD_FIELDS - 1 && feof(stream) == 0; i++)
          {
            if (word->fields[i] != NULL)
            {
              fputs(word->fields[i], stream);
              fputc(';', stream);
            }
          }
          if (word->fields[i] != NULL) fputs(word->fields[i], stream);
          fputc('\n', stream);
        }

        link = link->next;
      }
    }

    lw_vocabulary_set_changed (self, FALSE);

errored:

    if (stream != NULL) fclose(stream); stream = NULL;
    if (uri != NULL) g_free (uri); uri = NULL;
}


void
lw_vocabulary_set_changed (LwVocabulary *self, 
                           gboolean      changed_)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_VOCABULARY_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = (changed_ != priv->data.changed);

    priv->data.changed = changed_;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_CHANGED]);
}


gboolean
lw_vocabulary_has_changes (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(self), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.changed;
}


void
lw_vocabulary_set_loaded (LwVocabulary *self, 
                          gboolean      loaded)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_VOCABULARY_GET_CLASS (self);
    klasspriv = klass->priv;
    changed = (loaded != priv->data.loaded);

    priv->data.loaded = loaded;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_LOADED]);
}


gboolean
lw_vocabulary_is_loaded (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(self), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.loaded;
}


static gboolean
lw_vocabulary_rename_filename (LwVocabulary *self,
                               const gchar  *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    g_return_if_fail (FILENAME != NULL);

    //Declarations
    gchar *new_uri = NULL;
    gchar *old_uri = NULL;
    const gchar *OLD_FILENAME = lw_vocabulary_get_filename (self);
    gint status = FALSE;

    g_return_if_fail (OLD_FILENAME != NULL);

    old_uri = lw_util_build_filename (LW_PATH_VOCABULARY, OLD_FILENAME);
    if (old_uri == NULL) goto errored;
    new_uri = lw_util_build_filename (LW_PATH_VOCABULARY, FILENAME);
    if (new_uri == NULL) goto errored;

    status = g_rename (old_uri, new_uri);
    printf("%d\n', status\n");

errored:

    if (old_uri != NULL) g_free (old_uri); old_uri = NULL;
    if (new_uri != NULL) g_free (new_uri); new_uri = NULL;

    return (status == 0);
}


void
lw_vocabulary_set_filename (LwVocabulary *self,
                            const gchar  *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY(self));
    g_return_if_fail (FILENAME != NULL);

    //Declarations
    gchar *filename = NULL;
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;
    gboolean requires_rename = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_VOCABULARY_GET_CLASS (self);
    klasspriv = klass->priv;
    filename = g_uri_escape_string (FILENAME, " ", TRUE);
    if (filename == NULL) goto errored;
    requires_rename = (
      filename != NULL &&
      priv->config.filename != NULL &&
      lw_vocabulary_filename_exists (priv->config.filename) &&
      strcmp(filename, priv->config.filename) != 0
    );

    if (requires_rename)
    {
      if (!lw_vocabulary_rename_filename (self, filename)) goto errored; 
    }
    else
    {
      gchar *uri = lw_util_build_filename (LW_PATH_VOCABULARY, filename);
      if (!g_file_set_contents (uri, "", -1, NULL)) goto errored;
      if (uri != NULL) g_free (uri); uri = NULL;
    }

    if (priv->config.filename != NULL)
    {
      g_free (priv->config.filename);
      priv->config.filename = NULL;
    }

    if (filename != NULL)
    {
      priv->config.filename = g_strdup (filename);
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_FILENAME]);

errored:

if (filename != NULL) g_free (filename); filename = NULL;

    return;
}


const gchar*
lw_vocabulary_get_filename (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(self), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.filename;
}


LwWord*
lw_vocabulary_get_word_by_index (LwVocabulary *self, gint index)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY(self));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwWord *word = NULL;

    //Initializations
    priv = self->priv;
    word = g_list_nth_data (priv->data.list, index);

    return word;
}


GList*
lw_vocabulary_remove_all (LwVocabulary *self, gint *positions)
{
}


GList*
lw_vocabulary_insert_all (LwVocabulary *vocabualry, GList *wordlist, gint index)
{
}


void
lw_vocabulary_reorder (LwVocabulary *self, gint *new_positions)
{
}


gint
lw_vocabulary_length (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), 0);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = priv->data.length;

    if (length < 0)
    {
      length = g_list_length (priv->data.list);
    }

    return length;
}


gchar*
lw_vocabulary_generate_filename ()
{
    //Sanity checks
    g_return_val_if_fail (_klasspriv != NULL, NULL);

    //Declarations
    gchar *filename = NULL;
    
    //Initializations
    filename = g_strdup_printf ("%s%d", _klasspriv->BASE_FILENAME, _klasspriv->new_filename_index);

    if (filename != NULL) {
      _klasspriv->new_filename_index++;
    }

    return filename;
}


gboolean
lw_vocabulary_filename_exists (const gchar *FILENAME)
{
    //Sanity checks
    g_return_if_fail (FILENAME != NULL);

    //Declarations
    gchar *uri = NULL;
    gboolean exists = FALSE;

    //Initializations
    uri = lw_util_build_filename (LW_PATH_VOCABULARY, FILENAME);
    exists = g_file_test (uri, G_FILE_TEST_IS_REGULAR);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;

    return exists;
}


