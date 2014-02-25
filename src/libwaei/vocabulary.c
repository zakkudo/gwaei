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
        filenames[i] = g_uri_unescape_string (FILENAME, NULL);
        i++;
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

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwVocabularyClassPrivate, 1);
    object_class->set_property = lw_vocabulary_set_property;
    object_class->get_property = lw_vocabulary_get_property;
    object_class->dispose = lw_vocabulary_dispose;
    object_class->finalize = lw_vocabulary_finalize;

    g_type_class_add_private (object_class, sizeof (LwVocabularyPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    lw_vocabulary_initialize_filename_suffix ();

    _klasspriv->signalid[CLASS_SIGNALID_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_REORDERED] = g_signal_new (
        "internal-rows-reordered",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );

    _klasspriv->signalid[CLASS_SIGNALID_FILENAME_CHANGED] = g_signal_new (
        "filename-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, filename_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__STRING,
        G_TYPE_NONE, 1,
        G_TYPE_STRING
    );

    _klasspriv->pspec[PROP_FILENAME] = g_param_spec_string (
        "filename",
        "FIlename construct prop",
        "Set the filename",
        gettext("New Vocabulary List"),
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_FILENAME, _klasspriv->pspec[PROP_FILENAME]);

    _klasspriv->pspec[PROP_CHANGED] = g_param_spec_boolean (
        "changed",
        "changed construct prop",
        "Set the changed",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_CHANGED, _klasspriv->pspec[PROP_CHANGED]);

    _klasspriv->pspec[PROP_LOADED] = g_param_spec_boolean (
        "loaded",
        "loaded construct prop",
        "Set the loaded",
        FALSE,
        G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_LOADED, _klasspriv->pspec[PROP_LOADED]);
}

void
lw_vocabulary_clear (LwVocabulary *self)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (self));
    if (lw_vocabulary_is_loaded (self)) return;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint *indices = NULL;
    GList *removed = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);
    indices = g_new0 (gint, length + 1);

    {
      gint i = 0;
      for (i = 0; i < length; i++)
      {
        indices[i] = i;
      }
      indices[i] = -1;
    }

    removed = lw_vocabulary_remove_all (self, indices);

errored:

    if (indices != NULL) g_free (indices); indices = NULL;
    if (removed != NULL) g_list_free_full (removed, (GDestroyNotify) lw_word_free);
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

    lw_vocabulary_clear (self);

    if (!g_file_get_contents (uri, &contents, &length, NULL) || length == 0) goto errored;

    if (length > 0)
    {
      lw_util_replace_linebreaks_with_nullcharacter (contents);

      gchar *c = contents;
      GList *list = NULL;
      while (c != NULL && c < contents + length) 
      {
        LwWord *word = lw_word_new_from_string (c);
        if (word != NULL)
        {
          list = g_list_prepend (list, word);
        }
        c += strlen(c) + 1;
      }
      lw_vocabulary_insert_all (self, -1, list);

      if (list != NULL) g_list_free (list); list = NULL;
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

    lw_vocabulary_load_from_file (self, cb);

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
    gchar *uri = NULL;
    FILE *fd = NULL;

    //Initializations
    priv = self->priv;
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;
    fd = g_fopen (uri, "w");
    if (fd = NULL) goto errored;

    {
      GList *link = priv->data.list;
      while (link != NULL)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
          gchar *text = lw_word_to_string (word);
          if (text != NULL)
          {
            fputs(text, fd);
            fputc('\n', fd);
          }
          if (text != NULL) g_free (text); text = NULL;
        }

        link = link->next;
      }
    }

    lw_vocabulary_set_changed (self, FALSE);

errored:

    if (fd != NULL) fclose(fd); fd = NULL;
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (changed_ != priv->data.changed);

    priv->data.changed = changed_;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_CHANGED]);
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (loaded != priv->data.loaded);

    priv->data.loaded = loaded;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_LOADED]);
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


gchar*
lw_vocabulary_build_uri (const gchar *FILENAME)
{
    //Sanity checks
    if (FILENAME == NULL) return NULL;

    //Declarations
    gchar *filename = NULL;
    gchar *uri = NULL;

    //Initializations
    filename = g_strstrip (g_uri_escape_string (FILENAME, " ", TRUE));
    if (filename == NULL || strlen(filename) == 0) goto errored;

    uri = lw_util_build_filename (LW_PATH_VOCABULARY, filename);

errored:

    if (filename != NULL) g_free (filename); filename = NULL;

    return uri;
}


static gboolean
lw_vocabulary_delete_file (LwVocabulary *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    const gchar *FILENAME = NULL;
    gchar *uri = NULL;
    gint status = -1;

    //Initializations
    FILENAME = lw_vocabulary_get_filename (self);
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;
    if (g_file_test (uri, G_FILE_TEST_IS_REGULAR) == FALSE) goto errored;

    status = g_unlink (uri);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;

    return (status == 0);
}


static gboolean
lw_vocabulary_create_file (LwVocabulary *self,
                           const gchar *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), FALSE);
    if (FILENAME == NULL) return FALSE;

    //Declarations
    gchar *uri = NULL;
    gboolean success = FALSE;

    //Initializations
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;

    success = g_file_set_contents (uri, "", -1, NULL);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;

    return success;
}


static gboolean
lw_vocabulary_rename_file (LwVocabulary *self,
                           const gchar  *FILENAME)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), FALSE);
    if (FILENAME == NULL) return FALSE;

    //Declarations
    gchar *new_uri = NULL;
    gchar *old_uri = NULL;
    const gchar *OLD_FILENAME = NULL;
    gint status = -1;

    //Initializations
    OLD_FILENAME = lw_vocabulary_get_filename (self);
    if (OLD_FILENAME == NULL) goto errored;
    old_uri = lw_vocabulary_build_uri (OLD_FILENAME);
    if (old_uri == NULL) goto errored;
    new_uri = lw_vocabulary_build_uri (FILENAME);
    if (new_uri == NULL) goto errored;
    if (g_file_test (new_uri, G_FILE_TEST_IS_REGULAR)) goto errored;

    status = g_rename (old_uri, new_uri);

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

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gboolean changed = FALSE;
    gchar *previous_filename = NULL;
    gboolean file_exists = FALSE;
    gchar *new_uri = NULL;

    //Initializations
    priv = self->priv;
    new_uri = lw_vocabulary_build_uri (FILENAME);
    file_exists = lw_vocabulary_file_exists (self);
    if (new_uri == NULL) FILENAME = NULL;

    //Delete
    if (FILENAME == NULL && file_exists)
    {
      printf("BREAK delete file %s\n", FILENAME);
      changed = lw_vocabulary_delete_file (self);
    }
    //Rename
    else if (FILENAME != NULL && priv->config.filename != NULL && file_exists)
    {
      printf("BREAK rename file %s\n", FILENAME);
      changed = lw_vocabulary_rename_file (self, FILENAME);
    }
    //Create
    else if (FILENAME != NULL && priv->config.filename != NULL && !file_exists)
    {
      printf("BREAK create file %s\n", FILENAME);
      changed = lw_vocabulary_create_file (self, FILENAME);
    }
    //Initialize
    else if (priv->config.filename == NULL && FILENAME != NULL)
    {
      printf("BREAK initialize %s\n", FILENAME);
      changed = TRUE;
    }

    if (!changed) goto errored;

    previous_filename = priv->config.filename;
    priv->config.filename = g_strdup (FILENAME);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_FILENAME]);
    g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_FILENAME_CHANGED], 0, previous_filename);

errored:

    if (new_uri != NULL) g_free (new_uri); new_uri = NULL;
    if (previous_filename != NULL) g_free (previous_filename); previous_filename = NULL;

    return;
}


const gchar*
lw_vocabulary_get_filename (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(self), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gchar *filename = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.filename;
}


static void
_rebuild_array (LwVocabulary *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    lw_vocabulary_invalidate_length (self);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);

    g_free (priv->data.array); priv->data.array = NULL;
    priv->data.array = g_new0 (GList*, length);

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwWord *word = LW_WORD (link->data);
        priv->data.array[i] = link;
        word->row.current_index = i;
        i++;
      }
    }
}


static gint
_insert_all (LwVocabulary *self,
             gint         *position,
             GList        *wordlist)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (wordlist == NULL) return 0;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint number_inserted = 0;
    gint length = 0;
    gboolean append = FALSE;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);
    append = (*position < 0 || *position >= length || length == 0);
    number_inserted = g_list_length (wordlist);

    if (append)
    {
      GList *copy = g_list_copy (wordlist);
      *position = length;
      priv->data.list = g_list_concat (priv->data.list, copy);
    }
    else
    {
      GList *insert_link = g_list_nth (priv->data.list, *position);
      if (insert_link == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (wordlist); link != NULL; link = link->next)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
          priv->data.list = g_list_insert_before (priv->data.list, insert_link, word);
          insert_link = insert_link->prev;
        }
        else
        {
          number_inserted--;
        }
      }
    }

errored:

    lw_vocabulary_invalidate_length (self);

    return number_inserted;;
}


static void
_insert_all_propogate_changes (LwVocabulary *self,
                               gint          position,
                               gint          number_inserted)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;
    gint i = 0;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);

    //Rows that were inserted
    for (i = position; i < position + number_inserted; i ++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, i);
    }

    //Rows with modified indexes
    for (i = position + number_inserted; i < length; i++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_CHANGED], 0, i);
    }
}


void
lw_vocabulary_insert_all (LwVocabulary *self, 
                          gint          position,
                          GList        *wordlist)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (wordlist == NULL) return;

    //Declarations
    gint number_inserted = 0;

    //Initializations
    number_inserted = _insert_all (self, &position, wordlist);

    _rebuild_array (self);
    _insert_all_propogate_changes (self, position, number_inserted);
}


static gint
_remove_sort (gconstpointer a, gconstpointer b)
{
    gint ia = GPOINTER_TO_INT (a);
    gint ib = GPOINTER_TO_INT (b);

    if (ia > ib) return -1;
    if (ia < ib) return 1;

    return 0;
}



static gint* 
_sanitize_indices (LwVocabulary *self,
                   gint         *indices)
{
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);
    if (indices == NULL) return NULL;

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lw_vocabulary_length (self);

    //Copy
    {
      gint *copy = NULL;
      gint i = 0;
      while (indices[i] > -1 && indices[i] < length && i < length) i++;
      size = i;
      copy = g_new0 (gint, size + 1);
      memcpy(copy, indices, sizeof(gint) * (size));
      copy[size] = -1;
      indices = copy;
    }

    //Sort
    qsort(indices, sizeof(gint), size, _remove_sort);

    //Remove duplicates
    {
      gint i = 0, j = 0;
      while (indices[i] != -1 && indices[j] != -1)
      {
        if (indices[i] != indices[j])
        {
          indices[j] = indices[i];
          j++;
        }
        i++;
      }

      indices[i] = -1;
    }

    return indices;
}


static GList*
_remove_all (LwVocabulary *self,
             gint         *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (indices == NULL) return NULL;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;
    GList *list = NULL;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);
    if (length == 0) goto errored;

    {
      gint i = 0;
      for (i = 0; indices[i] != -1 && i < length; i++)
      {
        gint index = indices[i];
        GList *link = priv->data.array[index];
        if (link != NULL)
        {
          LwWord *word = LW_WORD (link->data);
          priv->data.list = g_list_remove_link (priv->data.list, link);
          list = g_list_concat (link, list);
          priv->data.array[index] = NULL;
        }
      }
      list = g_list_reverse (list);
    }

errored:

    lw_vocabulary_invalidate_length (self);

    return list;
}


static void
_remove_all_propogate_changes (LwVocabulary *self,
                               gint         *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (indices == NULL) return;

    //Declarations
    gint length = 0;
    gint i = 0;

    //Initializations
    length = lw_vocabulary_length (self);

    //Rows that were removed
    for (i = 0; indices[i] != -1; i++)
    {
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_DELETED], 0, indices[i]);
    }
    i--;

    //Rows with modified indexes
    {
      gint index = 0;
      for (index = indices[i]; index < length; index++)
      {
        g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_CHANGED], 0, index);
      }
    }
}


GList*
lw_vocabulary_remove_all (LwVocabulary *self,
                          gint         *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (indices == NULL) return NULL;

    //Declarations
    GList *removed = NULL;

    //Initializations
    indices = _sanitize_indices (self, indices);
    if (indices == NULL) goto errored;

    removed = _remove_all (self, indices);
    _rebuild_array (self);
    _remove_all_propogate_changes (self, indices);

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    return removed;
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


void
lw_vocabulary_invalidate_length (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), 0);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;

    priv->data.length = -1;
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
lw_vocabulary_file_exists (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), FALSE);

    //Declarations
    const gchar *FILENAME = NULL;
    gchar *uri = NULL;
    gboolean exists = FALSE;

    //Initializations
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;
    exists = g_file_test (uri, G_FILE_TEST_IS_REGULAR);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;

    return exists;
}


LwWord*
lw_vocabulary_nth (LwVocabulary *self,
                   gint          index)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;
    LwWord *word = NULL;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);

    if (index > -1 && index < length)
    {
      GList *link = priv->data.array[index];
      word = LW_WORD (link->data);
    }

    return word;
}


