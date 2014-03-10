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
#define SINGLE_COPY_PATTERN "%s Copy"
#define PLURAL_COPY_PATTERN "%s Copy %d"

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

    _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED] = g_signal_new (
        "internal-row-changed",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED] = g_signal_new (
        "internal-row-inserted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED] = g_signal_new (
        "internal-row-deleted",
        G_OBJECT_CLASS_TYPE (object_class),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwVocabularyClass, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    _klasspriv->signalid[CLASS_SIGNALID_ROWS_REORDERED] = g_signal_new (
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
    GList *removed = NULL;
    gint length = 0;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);

    removed = lw_vocabulary_remove_all (self, NULL);

errored:

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

    lw_vocabulary_load_from_string (self, contents, cb);

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

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwWord *w = LW_WORD (link->data);
        if (w != NULL)
        {
          w->row.saved_index = i;
          i++;
        }
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
    gchar *uri = NULL;
    gchar *text = NULL;

    //Initializations
    priv = self->priv;
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;
    text = lw_vocabulary_to_string (self, cb);
    if (text == NULL) goto errored;

    g_file_set_contents (uri, text, -1, NULL);

    {
      GList *link = NULL;
      gint i = 0;
      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwWord *w = LW_WORD (link->data);
        if (w != NULL)
        {
          w->row.saved_index = i;
          i++;
        }
      }
    }

    lw_vocabulary_set_changed (self, FALSE);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;
    if (text != NULL) g_free (text); text = NULL;
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

    if (changed || changed_) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_CHANGED]);
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
    file_exists = lw_vocabulary_has_file (self);
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


static void
_add_to_index (LwVocabulary *self,
               LwWord       *word)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (word == NULL) return;
    
    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
   
    //CURRENTLY UNUSED

errored:

    return;
}


static void
_remove_from_index (LwVocabulary *self,
                    LwWord       *word)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));
    if (word == NULL) return;
    
    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    //CURRENTLY UNUSED

errored:

    return;
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

    lw_vocabulary_invalidate_length (self);
}


static gint
_insert_all (LwVocabulary *self,
             gint         *position,
             GList        *wordlist)
{
printf("BREAK0 _insert_all\n");
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

      {
        GList *link = NULL;
        for (link = copy; link != NULL; link = link->next)
        {
          LwWord *word = LW_WORD (link->data);
          if (word != NULL)
          {
            _add_to_index (self, word);
          }
        }
      }
    }
    else
    {
printf("BREAK1 _insert_all\n");
      GList *insert_link = g_list_nth (priv->data.list, *position);
      if (insert_link == NULL) goto errored;
      GList *link = NULL;
      for (link = g_list_last (wordlist); link != NULL; link = link->prev)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
printf("BREAK2 _insert_all %s \n", lw_word_get_kanji (word));
          priv->data.list = g_list_insert_before (priv->data.list, insert_link, word);
          insert_link = insert_link->prev;
          _add_to_index (self, word);
        }
        else
        {
          number_inserted--;
        }
      }
printf("BREAK3 _insert_all %d \n", number_inserted);
    }

errored:

    lw_vocabulary_invalidate_length (self);

    return number_inserted;
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
printf("BREAK0 _insert_all_propogate_changed\n");

    //Rows that were inserted
    for (i = position; i < position + number_inserted; i ++)
    {
printf("BREAK1 _insert_all_propogate_changed insert %d\n", i);
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_INSERTED], 0, i);
    }

    //Rows with modified indexes
    for (i = position + number_inserted; i < length; i++)
    {
printf("BREAK2 _insert_all_propogate_changed update %d\n", i);
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, i);
    }
}


static gint*
_find_duplicates (LwVocabulary *self, 
                  gint          position, 
                  GList        *wordlist)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);
    if (wordlist == NULL) return NULL;

    //Declarations
    gint length = -1;
    gint *indices = NULL;

    //Initializations
    length = g_list_length (wordlist);
    if (position < 0) position = lw_vocabulary_length (self);
    indices = g_new0 (gint, length + 1);
    if (indices == NULL) goto errored;
printf("BREAK1 _find_duplicates\n");

    {
      GList *link = NULL;
      gint i = 0;
      for (link = wordlist; link != NULL; link = link->next)
      {
printf("BREAK2 _find_duplicates\n");
        LwWord *a = LW_WORD (link->data);
        if (a == NULL) continue;
        LwWord *b = LW_WORD (lw_vocabulary_nth (self, a->row.current_index));
        if (b == NULL) continue;

        if (a == b)
        {
          gint index = a->row.current_index;
printf("BREAK3 _find_duplicates %d\n", index);
          if (index >= position)
          {
            index += length;
          }
          indices[i] = index;
          printf("BREAK _find_duplicates indices[%d] = %d\n", i, indices[i]);
          i++;
        }
      }
      indices[i] = -1;
      printf("BREAK _find_duplicates indices[%d] = %d\n", i, indices[i]);
    }

errored:

    return indices;
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
    gint *duplicates = NULL;
    GList *removed = NULL;
    gboolean changed = FALSE;

    //Initializations
    duplicates = _find_duplicates (self, position, wordlist);
    number_inserted = _insert_all (self, &position, wordlist);

    _rebuild_array (self);
    _insert_all_propogate_changes (self, position, number_inserted);

    changed = TRUE;

    if (duplicates == NULL) goto errored;

    removed = lw_vocabulary_remove_all (self, duplicates);

errored:

    if (duplicates != NULL) g_free (duplicates); duplicates = NULL;
    if (removed != NULL) g_list_free (removed); removed;

    if (changed) lw_vocabulary_set_changed (self, TRUE);
    
    return;
}


static gint
_remove_sort (gconstpointer a, gconstpointer b)
{
    gint ia = *((gint*) a);
    gint ib = *((gint*) b);

    if (ia > ib) return -1;
    if (ia < ib) return 1;

    return 0;
}


static gint* 
_sanitize_indices (LwVocabulary *self,
                   gint         *indices)
{
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);

    //Declarations
    gint size = 0;
    gint length = 0;

    //Initializations
    length = lw_vocabulary_length (self);

    if (indices == NULL) //New
    {
      indices = g_new0 (gint, length + 1);
      {
        gint i = 0;
        for (i = 0; i < length; i++)
        {
          indices[i] = i;
        }
        indices[i] = -1;
      }
    }
    else { //Copy
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
    qsort(indices, size, sizeof(gint), _remove_sort);
  
    //Remove duplicates
    {
      gint i = 0, j = 1;
      while (indices[i] != -1 && indices[j] != -1)
      {
        if (indices[i] != indices[j])
        {
          i++;
          indices[i] = indices[j];
        }
        j++;
      }
      i++;
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
      printf("_remove_all length: %d\n", g_list_length (list));
      gint i = 0;
      for (i = 0; indices[i] != -1 && i < length; i++)
      {
        gint index = indices[i];
        printf("_remove_all removing index %d\n", index);
        GList *link = priv->data.array[index];
        if (link != NULL)
        {
          LwWord *word = LW_WORD (link->data);
          priv->data.list = g_list_remove_link (priv->data.list, link);
          list = g_list_concat (link, list);
          priv->data.array[index] = NULL;

          if (word != NULL && word->row.current_index == index)
          {
            _remove_from_index (self, word);
          }
        }
      }
      list = g_list_reverse (list);
      printf("_remove_all length: %d\n", g_list_length (list));
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
      printf("_remove_all_propogate_changes removed %d\n", indices[i]);
      g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_DELETED], 0, indices[i]);
    }
    i--;

    //Rows with modified indexes
    {
      gint index = 0;
      for (index = indices[i]; index < length; index++)
      {
        printf("_remove_all_propogate_changes updating %d\n", index);
        g_signal_emit (G_OBJECT (self), _klasspriv->signalid[CLASS_SIGNALID_ROW_CHANGED], 0, index);
      }
    }
}


GList*
lw_vocabulary_remove_all (LwVocabulary *self,
                          gint         *indices)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (self));

    //Declarations
    GList *removed = NULL;
    gboolean changed = FALSE;

    //Initializations
    indices = _sanitize_indices (self, indices);
    if (indices == NULL) goto errored;

    removed = _remove_all (self, indices);
    _rebuild_array (self);
    _remove_all_propogate_changes (self, indices);

    changed = TRUE;

errored:

    if (indices != NULL) g_free (indices); indices = NULL;

    if (changed) lw_vocabulary_set_changed (self, TRUE);

    return removed;
}


static gint
_get_new_filename_number (const gchar *PATTERN,
                          const gchar *FILENAME)
{
    //Declarations
    gint *numbers = NULL;
    gint number = -1;

    //Initializations
    numbers = lw_util_get_numbers (FILENAME);

    {
      gint i = 0;
      gchar *prototype = NULL;
      for (i = 0; numbers[i] != 0 && number < 0; i++)
      {
        gint n = numbers[i];
        prototype = g_strdup_printf (PATTERN, n);
        if (prototype != NULL)
        {
          if (strcmp(prototype, FILENAME) == 0 && n > 0 && n > number)
          {
            number = n;
          }
          g_free (prototype); prototype = NULL;
        }
      }
    }

errored:

    if (numbers != NULL) g_free (numbers); numbers = NULL;

    return number;
}


static gint
_initialize_new_filename_suffix_number (const gchar *PATTERN)
{
    //Sanity checks
    g_return_if_fail (PATTERN != NULL);

    //Declarations
    gchar **filenames = NULL;
    gint number = 0;

    //Initializations
    number = 0;
    filenames = lw_vocabulary_get_filenames ();
    if (filenames == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; filenames[i] != NULL; i++)
      {
        gchar* f = filenames[i];
        gint n = _get_new_filename_number (PATTERN, f);
        if (n > 0 && n > number)
        {
          number = n;
        }
      }
    }

errored:

    if (filenames != NULL) g_strfreev (filenames); filenames = NULL;

    number++;

    return number;
}


gchar*
lw_vocabulary_generate_new_filename ()
{
    //Declarations
    gchar *filename = NULL;
    const gchar *PATTERN = NULL;
    gint number = -1;

    //Initializations
    PATTERN = gettext("New List %d");
    number = _initialize_new_filename_suffix_number (PATTERN);

    do {
      if (filename != NULL) g_free (filename); filename = NULL;
      filename = g_strdup_printf (PATTERN, number);
      number++;
    } while (lw_vocabulary_filename_exists (filename));

    return filename;
}


static gchar*
_get_copied_filename_base (const gchar *FILENAME,
                           const gchar *PATTERN)
{
    //Sanity checks
    if (FILENAME == NULL) return NULL;
    if (PATTERN == NULL) return NULL;

    //Declarations
    gchar *pattern = NULL;
    GRegex *regex = NULL;
    gchar *base = NULL;
    GMatchInfo *match_info = NULL;

    //Initializations
    pattern = lw_util_convert_printf_pattern_to_regex_pattern (PATTERN);
    printf("BREAK _get_copied_filename_base %s\n", pattern);
    if (pattern == NULL) goto errored;
    regex = g_regex_new (pattern, 0, 0, NULL);
    if (regex == NULL) goto errored;
    base = g_strdup (FILENAME);
    if (base == NULL) goto errored;

    g_regex_match (regex, FILENAME, 0, &match_info);

    while (g_match_info_matches (match_info))
    {
      gchar *b = g_match_info_fetch_named (match_info, "text");
      if (b != NULL)
      {
        printf("BREAK _get_copied_filename_base match: %s\n", b);
        if (base != NULL) g_free (base);
        base = b;
        b = NULL;
      }
      g_match_info_next (match_info, NULL);
    }

errored:

    if (pattern != NULL) g_free (pattern); pattern = NULL;
    if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;
    if (regex != NULL) g_regex_unref (regex); regex = NULL;

    return base;
}


static gint
_get_copied_filename_number (const gchar  *FILENAME,
                             gchar       **filename_base_out)
{
    //Sanity checks
    if (FILENAME == NULL) return 1;

    //Declarations
    gint *numbers = NULL;
    gint number = 1;

    //Initializations
    numbers = lw_util_get_numbers (FILENAME);
    if (numbers == NULL) goto errored;
    if (filename_base_out != NULL) *filename_base_out = g_strdup (FILENAME);

    //Now use the numbers to do pattern matching
    {
      gint i = 0;
      for (i = 0; numbers[i] > 0; i++)
      {
        gint n = numbers[i];
        const gchar *PATTERN = ngettext(SINGLE_COPY_PATTERN, PLURAL_COPY_PATTERN, n);
        gchar *filename_base = _get_copied_filename_base (FILENAME, PATTERN);
        if (filename_base != NULL)
        {
          gchar *prototype = g_strdup_printf (PATTERN, filename_base, n);
          if (prototype != NULL)
          {
            if (strcmp(prototype, FILENAME) == 0 && n > 0 && n > number)
            {
              number = n;
              if (filename_base_out != NULL)
              {
        printf("BREAK _get_copied_filename_number %s %d %s %s\n", filename_base, n, prototype, FILENAME);
                g_free (*filename_base_out);
                *filename_base_out = filename_base;
                filename_base = NULL;
              }
            }
            g_free (prototype); prototype = NULL;
          }
          g_free (filename_base); filename_base = NULL;
        }
      }
    }

errored:

    if (numbers != NULL) g_free (numbers); numbers = NULL;

    return number;
}


static gint
_initialize_copied_filename_suffix (const gchar  *FILENAME,
                                    gchar       **filename_base_out)
{
    //Sanity checks
    if (FILENAME == NULL) return 1;

    //Declarations
    gchar **filenames = NULL;
    gint length = 0;
    gint number = 0;
    gchar *filename_base = NULL;

    //Initializations
    filenames = lw_vocabulary_get_filenames ();
    if (filenames == NULL) goto errored;
    number = _get_copied_filename_number (FILENAME, &filename_base);
    if (filename_base_out != NULL) *filename_base_out = g_strdup (filename_base);

    {
      gint i = 0;
      for (i = 0; filenames[i] != NULL; i++)
      {
        gchar *f = filenames[i];
        gchar *other_filename_base = NULL;
        gint n = _get_copied_filename_number (f, &other_filename_base);

        if (n > 0 && n > number && strcmp(filename_base, other_filename_base) == 0)
        {
          number = n;
        }

        g_free (other_filename_base); other_filename_base = NULL;
      }
    }

    number++;

errored:

    if (filenames != NULL) g_strfreev (filenames); filenames = NULL;


    return number;
}


gchar*
lw_vocabulary_generate_copied_filename (const gchar *FILENAME)
{
    //Sanity checks
    if (FILENAME == NULL) return NULL;

    //Declarations
    gchar *filename = NULL;
    gint number = -1;
    gchar *filename_base = NULL;

    //Initializations
    number = _initialize_copied_filename_suffix (FILENAME, &filename_base);

    do {
      if (filename != NULL) g_free (filename); filename = NULL;
      const gchar *TEXT = ngettext(SINGLE_COPY_PATTERN, PLURAL_COPY_PATTERN, number); 
      filename = g_strdup_printf (TEXT, filename_base, number);
      number++;
    } while (lw_vocabulary_filename_exists (filename));

errored:

    if (filename_base != NULL) g_free (filename_base); filename_base = NULL;

    return filename;
}


gboolean
lw_vocabulary_filename_exists (const gchar *FILENAME)
{
    //Sanity checks
    if (FILENAME == NULL) return FALSE;

    //Declarations
    gchar *uri = NULL;
    gboolean exists = FALSE;

    //Initializations
    uri = lw_vocabulary_build_uri (FILENAME);
    if (uri == NULL) goto errored;
    exists = g_file_test (uri, G_FILE_TEST_IS_REGULAR);

errored:

    if (uri != NULL) g_free (uri); uri = NULL;

    return exists;
}


gboolean
lw_vocabulary_has_file (LwVocabulary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), FALSE);

    //Declarations
    const gchar *FILENAME = NULL;
    gboolean exists = FALSE;

    //Initializations
    FILENAME = lw_vocabulary_get_filename (self);
    if (FILENAME == NULL) goto errored;
    exists = lw_vocabulary_filename_exists (FILENAME);

errored:

    return exists;
}


LwWord*
lw_vocabulary_nth (LwVocabulary *self,
                   gint          index)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);
    if (index < 0) return NULL;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;
    LwWord *word = NULL;
    GList *link = NULL;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);
    if (index >= length) goto errored;

    link = priv->data.array[index];
    if (link == NULL) goto errored;
    word = LW_WORD (link->data);

errored:

    return word;
}


gchar* 
lw_vocabulary_load_from_string (LwVocabulary       *self,
                                const gchar        *TEXT,
                                LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (self));
    if (TEXT == NULL) return NULL;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gchar *contents = NULL;
    gsize length = 0;
    GList *words = NULL;
    gchar *filename_hint = NULL;
    gchar *end = NULL;

    //Initializations
    priv = self->priv;
    if (*TEXT == '#')
      end = strchr(TEXT + 1, '#');
    else
      end = strchr(TEXT, '#');
    if (end == NULL)
      contents = g_strdup (TEXT);
    else
      contents = g_strndup (TEXT, end - TEXT);
    if (contents == NULL || *contents == '\0') goto errored;

    length = lw_util_replace_linebreaks_with_nullcharacter (contents);
    if (length == 0) goto errored;

    {
      gchar *c = NULL;
      for (c = contents; c != NULL && c < contents + length; c += strlen(c) + 1) 
      {
        gboolean is_filename_hint = (*c == '#' && *(c + 1) != '\0');
        gboolean can_set_filename_hint = (filename_hint == NULL && words == NULL);

        if (is_filename_hint && !can_set_filename_hint) break;

        if (is_filename_hint)
        {
printf("lw_vocabulary_load_from_string creating filename from %s\n", c);
          filename_hint = g_strdup (c + 1);
        }
        else
        {
printf("lw_vocabulary_load_from_string creating word from %s\n", c);
          LwWord *word = lw_word_new_from_string (c);
          if (word != NULL)
          {
            words = g_list_prepend (words, word);
          }
        }
      }
      words = g_list_reverse (words);
    }

    if (words == NULL) goto errored;


    if (filename_hint != NULL && priv->config.filename == NULL)
    {
      if (lw_vocabulary_filename_exists (filename_hint))
      {
        gchar *filename = lw_vocabulary_generate_copied_filename (filename_hint);
        g_free (filename_hint); filename_hint = filename;
      }
      lw_vocabulary_set_filename (self, filename_hint);
      lw_vocabulary_insert_all (self, -1, words);
      lw_vocabulary_save (self, NULL);
    }
    else if (filename_hint == NULL && priv->config.filename == NULL)
    {
      gchar *filename = lw_vocabulary_generate_new_filename ();
      g_free (filename_hint); filename_hint = filename;
      lw_vocabulary_set_filename (self, filename_hint);
      lw_vocabulary_insert_all (self, -1, words);
      lw_vocabulary_save (self, NULL);
    }
    else
    {
      lw_vocabulary_insert_all (self, -1, words);
    }


errored:

    if (filename_hint != NULL) g_free (filename_hint); filename_hint = NULL;
    if (words != NULL) g_list_free (words); words = NULL;
    if (contents != NULL) g_free (contents); contents = NULL;

    if (end == NULL)
    {
      TEXT = NULL;
    }
    else
    {
      TEXT += length;
    }

    return (gchar*) TEXT;
}


gchar*
lw_vocabulary_to_string (LwVocabulary       *self,
                         LwProgressCallback  cb)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (self), NULL);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = -1;
    gchar **line = NULL;
    gchar *text = NULL;

    //Initializations
    priv = self->priv;
    length = lw_vocabulary_length (self);
    line = g_new0 (gchar*, length + 2);

    {
      GList *link = NULL;
      gint i = 0;

      if (priv->config.filename != NULL)
      {
        line[i++] = g_strdup_printf ("#%s", priv->config.filename);
      }

      for (link = priv->data.list; link != NULL; link = link->next)
      {
        LwWord *word = LW_WORD (link->data);
        if (word != NULL)
        {
          gchar *text = lw_word_to_string (word);
          if (text != NULL)
          {
            line[i] = text;
            text = NULL;
            i++;
          }
        }
      }
      line[i++] = NULL;
    }
    
    text = g_strjoinv ("\n", line);

errored:

    if (line != NULL) g_strfreev (line); line = NULL;

    return text;
}

