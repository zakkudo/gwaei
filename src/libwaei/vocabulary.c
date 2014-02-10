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

#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/vocabulary-private.h>


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
lw_vocabulary_init (LwVocabulary *vocabulary)
{
    vocabulary->priv = LW_VOCABULARY_GET_PRIVATE (vocabulary);
    memset(vocabulary->priv, 0, sizeof(LwVocabularyPrivate));
}


static void 
lw_vocabulary_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    LwVocabulary *vocabulary = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (object);
    priv = vocabulary->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        lw_vocabulary_set_filename (vocabulary, g_value_get_string (value));
        break;
      case PROP_CHANGED:
        lw_vocabulary_set_changed (vocabulary, g_value_get_boolean (value));
        break;
      case PROP_LOADED:
        lw_vocabulary_set_loaded (vocabulary, g_value_get_boolean (value));
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
    LwVocabulary *vocabulary = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (object);
    priv = vocabulary->priv;

    switch (property_id)
    {
      case PROP_FILENAME:
        g_value_set_string (value, lw_vocabulary_get_filename (vocabulary));
        break;
      case PROP_CHANGED:
        g_value_set_boolean (value, lw_vocabulary_has_changes (vocabulary));
        break;
      case PROP_LOADED:
        g_value_set_boolean (value, lw_vocabulary_is_loaded (vocabulary));
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
    LwVocabulary *vocabulary = NULL;
    LwVocabularyPrivate *priv = NULL;

    //Initalizations
    vocabulary = LW_VOCABULARY (object);
    priv = vocabulary->priv;

    lw_vocabulary_set_filename (vocabulary, NULL);

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
    LwVocabulary *vocabulary = NULL;

    //Initializations
    vocabulary = LW_VOCABULARY (object);

    G_OBJECT_CLASS (lw_vocabulary_parent_class)->dispose (object);
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
lw_vocabulary_load_from_file (LwVocabulary       *vocabulary, 
                              LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (vocabulary));
    if (lw_vocabulary_is_loaded (vocabulary)) return;

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gchar const *FILENAME = NULL;
    gchar *uri = NULL;
    gchar *contents = NULL;
    gsize length = 0;

    //Initializations
    priv = vocabulary->priv;
    FILENAME = lw_vocabulary_get_filename (vocabulary);
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
lw_vocabulary_load (LwVocabulary       *vocabulary, 
                    LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_VOCABULARY (vocabulary));
    if (lw_vocabulary_is_loaded (vocabulary)) return;

    //Declarations
    LwVocabularyPrivate *priv = vocabulary->priv;
    LwVocabularyClass *klass = LW_VOCABULARY_GET_CLASS (vocabulary);
    LwVocabularyClassPrivate *klasspriv = klass->priv;

    lw_vocabulary_load_from_file (vocabulary, cb);

    //Update the list information
    priv->data.length = -1;
    priv->data.list = g_list_reverse (priv->data.list);

    {
      gint i = 0;
      gint length = lw_vocabulary_length (vocabulary);
      for (i = 0; i < length; i++)
      {
        g_signal_emit (vocabulary, klasspriv->signalid[CLASS_SIGNALID_INSERTED], 0, i);
      }
    }

    lw_vocabulary_set_changed (vocabulary, FALSE);
    lw_vocabulary_set_loaded (vocabulary, TRUE);
}

void
lw_vocabulary_save (LwVocabulary       *vocabulary, 
                    LwProgressCallback  cb)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (vocabulary));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    const gchar *FILENAME = NULL;
    gint i = 0;
    gchar *uri = NULL;
    FILE *stream = NULL;

    //Initializations
    priv = vocabulary->priv;
    FILENAME = lw_vocabulary_get_filename (vocabulary);
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

    lw_vocabulary_set_changed (vocabulary, FALSE);

errored:

    if (stream != NULL) fclose(stream); stream = NULL;
    if (uri != NULL) g_free (uri); uri = NULL;
}


void
lw_vocabulary_set_changed (LwVocabulary *vocabulary, 
                           gboolean      changed_)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (vocabulary));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = vocabulary->priv;
    klass = LW_VOCABULARY_GET_CLASS (vocabulary);
    klasspriv = klass->priv;
    changed = (changed_ != priv->data.changed);

    priv->data.changed = changed_;

    if (changed) g_object_notify_by_pspec (G_OBJECT (vocabulary), klasspriv->pspec[PROP_CHANGED]);
}


gboolean
lw_vocabulary_has_changes (LwVocabulary *vocabulary)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(vocabulary), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = vocabulary->priv;

    return priv->data.changed;
}


void
lw_vocabulary_set_loaded (LwVocabulary *vocabulary, 
                          gboolean      loaded)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY (vocabulary));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = vocabulary->priv;
    klass = LW_VOCABULARY_GET_CLASS (vocabulary);
    klasspriv = klass->priv;
    changed = (loaded != priv->data.loaded);

    priv->data.loaded = loaded;

    if (changed) g_object_notify_by_pspec (G_OBJECT (vocabulary), klasspriv->pspec[PROP_LOADED]);
}


gboolean
lw_vocabulary_is_loaded (LwVocabulary *vocabulary)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(vocabulary), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = vocabulary->priv;

    return priv->data.loaded;
}


void
lw_vocabulary_set_filename (LwVocabulary *vocabulary,
                            const gchar  *FILENAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY(vocabulary));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwVocabularyClass *klass = NULL;
    LwVocabularyClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = vocabulary->priv;
    klass = LW_VOCABULARY_GET_CLASS (vocabulary);
    klasspriv = klass->priv;
    changed = (FILENAME != priv->config.filename || (FILENAME && priv->config.filename && strcmp(FILENAME, priv->config.filename) != 0));

    if (priv->config.filename != NULL)
    {
      g_free (priv->config.filename);
      priv->config.filename = NULL;
    }

    if (FILENAME != NULL)
    {
      priv->config.filename = g_strdup (FILENAME);
    }

    if (changed) g_object_notify_by_pspec (G_OBJECT (vocabulary), klasspriv->pspec[PROP_FILENAME]);
}


const gchar*
lw_vocabulary_get_filename (LwVocabulary *vocabulary)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY(vocabulary), FALSE);

    //Declarations
    LwVocabularyPrivate *priv = NULL;

    //Initializations
    priv = vocabulary->priv;

    return priv->config.filename;
}


LwWord*
lw_vocabulary_get_word_by_index (LwVocabulary *vocabulary, gint index)
{
    //Sanity checks
    g_return_if_fail (LW_IS_VOCABULARY(vocabulary));

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    LwWord *word = NULL;

    //Initializations
    priv = vocabulary->priv;
    word = g_list_nth_data (priv->data.list, index);

    return word;
}


GList*
lw_vocabualry_remove_all (LwVocabulary *vocabulary, gint *positions)
{
}


GList*
lw_vocabulary_insert_all (LwVocabulary *vocabualry, GList *wordlist, gint index)
{
}


void
lw_vocabulary_reorder (LwVocabulary *vocabulary, gint *new_positions)
{
}


gint
lw_vocabulary_length (LwVocabulary *vocabulary)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_VOCABULARY (vocabulary), 0);

    //Declarations
    LwVocabularyPrivate *priv = NULL;
    gint length = 0;

    //Initializations
    priv = vocabulary->priv;
    length = priv->data.length;

    if (length < 0)
    {
      length = g_list_length (priv->data.list);
    }

    return length;
}

