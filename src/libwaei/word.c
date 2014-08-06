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
//!  @file word.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <locale.h>
#include <stdlib.h>
#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>


LwWord*
lw_word_new ()
{
    LwWord *word = NULL;

    word = g_new0 (LwWord, 1);
    word->row.current_index = -1;
    word->row.saved_index = -1;

    return word;
}


LwWord*
lw_word_new_from_string (const gchar *TEXT)
{
    //Sanity checks
    if (TEXT == NULL) return NULL;

    //Declarations
    LwWord *word = NULL;
    gchar **fields = NULL;

    //Initializations
    word = g_new0 (LwWord, 1);
    if (word == NULL) goto errored;

    fields = g_strsplit (TEXT, ";", TOTAL_LW_WORD_FIELDS);
    if (fields == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; fields[i] != NULL; i++)
      {
        if (fields[i] != NULL)
        {
          word->fields[i] = g_strdup (g_strstrip(fields[i]));
          fields[i] = NULL;
        }
        else
        {
          word->fields[i] = g_strdup("");
        }
      }
    }

    //Set up the integers
    const gchar *CORRECT_GUESSES = word->fields[LW_WORD_FIELD_CORRECT_GUESSES];
    if (CORRECT_GUESSES != NULL)
    {
      word->correct_guesses = (gint) strtoll (CORRECT_GUESSES, NULL, 10);
    }

    const gchar *INCORRECT_GUESSES = word->fields[LW_WORD_FIELD_INCORRECT_GUESSES];
    if (INCORRECT_GUESSES != NULL)
    {
      word->incorrect_guesses =  (gint) strtoll (INCORRECT_GUESSES, NULL, 10);
    }

    const gchar *TIMESTAMP = word->fields[LW_WORD_FIELD_TIMESTAMP];
    if (TIMESTAMP != NULL)
    {
    word->timestamp =  (guint32) strtoll (TIMESTAMP, NULL, 10);
    }

    word->row.current_index = -1;
    word->row.saved_index = -1;

errored:

    if (fields != NULL) g_strfreev (fields); fields = NULL;

    return word;
}


GType
lw_word_get_type ()
{
  static GType type = 0;

  if (type == 0)
  {
    type = g_boxed_type_register_static (
      "LwWord",
      (GBoxedCopyFunc) lw_word_copy,
      (GBoxedFreeFunc) lw_word_free
    );
  }

  return type;
}


void
lw_word_free (LwWord *word)
{
  //Snaity checks
  if (word == NULL) return;

  {
    gint i = 0;
    for (i = 0; i < TOTAL_LW_WORD_FIELDS; i++)
    {
      if (word->fields[i] != NULL)
      {
        g_free (word->fields[i]);
        word->fields[i] = NULL;
      }
    }
  }

  g_free (word->score); word->score = NULL;
  g_free (word->days); word->days = NULL;

  g_free (word);
}

const gchar* 
lw_word_get_kanji (LwWord *word)
{
  return word->fields[LW_WORD_FIELD_KANJI];
}

void 
lw_word_set_kanji (LwWord *word, const gchar *text)
{
  if (word->fields[LW_WORD_FIELD_KANJI] != NULL)
    g_free (word->fields[LW_WORD_FIELD_KANJI]);
  word->fields[LW_WORD_FIELD_KANJI] = g_strdup (text);
  word->has_changes = TRUE;
}

const gchar* 
lw_word_get_reading (LwWord *word)
{
  return word->fields[LW_WORD_FIELD_READING];
}

void 
lw_word_set_reading (LwWord *word, const gchar *text)
{
  if (word->fields[LW_WORD_FIELD_READING] != NULL)
    g_free (word->fields[LW_WORD_FIELD_READING]);
  word->fields[LW_WORD_FIELD_READING] = g_strdup (text);
  word->has_changes = TRUE;
}

const gchar* 
lw_word_get_definition (LwWord *word)
{
  return word->fields[LW_WORD_FIELD_DEFINITION];
}

void 
lw_word_set_definition (LwWord *word, const gchar *text)
{
  if (word->fields[LW_WORD_FIELD_DEFINITION] != NULL)
    g_free (word->fields[LW_WORD_FIELD_DEFINITION]);
  word->fields[LW_WORD_FIELD_DEFINITION] = g_strdup (text);
  word->has_changes = TRUE;
}

gint 
lw_word_get_correct_guesses (LwWord *word)
{
  return word->correct_guesses;
}

void 
lw_word_set_correct_guesses (LwWord *word, gint number)
{
  if (word->fields[LW_WORD_FIELD_CORRECT_GUESSES] != NULL)
    g_free (word->fields[LW_WORD_FIELD_CORRECT_GUESSES]);
  word->fields[LW_WORD_FIELD_CORRECT_GUESSES] = g_strdup_printf ("%d", number);
  word->correct_guesses = number;
  if (word->score != NULL) g_free (word->score); word->score = NULL;
}

gint 
lw_word_get_incorrect_guesses (LwWord *word)
{
  return word->incorrect_guesses;
}

void 
lw_word_set_incorrect_guesses (LwWord *word, gint number)
{
  if (word->fields[LW_WORD_FIELD_INCORRECT_GUESSES] != NULL)
    g_free (word->fields[LW_WORD_FIELD_INCORRECT_GUESSES]);
  word->fields[LW_WORD_FIELD_INCORRECT_GUESSES] = g_strdup_printf ("%d", number);
  word->incorrect_guesses = number;
  if (word->score != NULL) g_free (word->score); word->score = NULL;
}


gint 
lw_word_get_score (LwWord *word)
{
    gint total = word->correct_guesses + word->incorrect_guesses;
    if (total == 0) return 0.0;
    else return (word->correct_guesses * 100 / total);
}


const gchar* 
lw_word_get_score_as_string (LwWord *word)
{
    gint total;
    
    if (word->score == NULL)
    {
      total = word->correct_guesses + word->incorrect_guesses;
      if (total == 0)
        word->score = g_strdup (gettext("Untested"));
      else
        word->score = g_strdup_printf ("%3d%%", lw_word_get_score (word));
    }

    return word->score;
}


guint32
lw_word_timestamp_to_hours (gint64 timestamp)
{
    const gint MICROSECONDS = 1000000;
    const gint SECONDS = 60;
    const gint MINUTES = 60;
    return (guint32) (timestamp / MICROSECONDS / SECONDS / MINUTES);
}


void
lw_word_set_timestamp (LwWord *word, gint64 timestamp)
{
    guint32 hours = lw_word_timestamp_to_hours (timestamp);
    lw_word_set_last_studied (word, hours);
}


void
lw_word_update_timestamp (LwWord *word)
{
    lw_word_set_timestamp (word, g_get_real_time ());
}


void
lw_word_set_last_studied (LwWord *word, guint32 hours)
{
    word->timestamp = hours;

    if (word->days != NULL) g_free (word->days); word->days = NULL;
    if (word->fields[LW_WORD_FIELD_TIMESTAMP] != NULL)
      g_free (word->fields[LW_WORD_FIELD_TIMESTAMP]);

    word->fields[LW_WORD_FIELD_TIMESTAMP] = g_strdup_printf ("%" G_GUINT32_FORMAT, word->timestamp);
}


guint32
lw_word_get_last_studied (LwWord *word)
{
    return word->timestamp;
}


const gchar*
lw_word_get_last_studied_as_string (LwWord *word)
{
    if (word->days == NULL)
    {
      guint32 days = lw_word_get_last_studied (word) / 24;
      guint32 today = lw_word_timestamp_to_hours ( g_get_real_time ()) / 24;
      guint32 difference = today - days;
      if (difference < 0) difference = 0;

      if (days == 0) word->days = g_strdup (pgettext("noun", "Never"));
      else if (difference == 0) word->days = g_strdup (gettext("Today"));
      else if (difference == 1) word->days = g_strdup (gettext("Yesterday"));
      else word->days = g_strdup_printf (ngettext("%d Day Ago", "%d Days Ago", difference), difference);
    }

    return word->days;
}


gchar* 
lw_word_to_string (LwWord *word)
{
    //Sanity checks
    if (word == NULL) return NULL;

    //Delcarations
    gchar* text = NULL;
    gchar **fields = NULL;

    //Initializations
    fields = g_new0 (gchar*, TOTAL_LW_WORD_FIELDS + 1);
    if (fields == NULL) goto errored;

    {
      gint i = 0;
      gint j = 0;
      for (i = 0; i < TOTAL_LW_WORD_FIELDS; i++)
      {
        if (word->fields[i] != NULL)
        {
          fields[j++] = g_uri_escape_string (word->fields[i], " ", TRUE);
        }
      }
      fields[j++] = NULL;
    }

    text = g_strjoinv (";", fields);

errored:

    if (fields != NULL) g_strfreev (fields); fields = NULL;

    return text;
}

gboolean
lw_word_has_changes (LwWord *word)
{
    //Sanity checks
    g_return_val_if_fail (word != NULL, FALSE);

    return word->has_changes;
}


LwWord*
lw_word_copy (LwWord *word)
{
    //Sanity checks
    if (word == NULL) return NULL;

    //Declarations
    gchar *text = NULL;
    LwWord *copy = NULL;

    //Initializations
    text = lw_word_to_string (word);
    if (text == NULL) goto errored;
    copy = lw_word_new_from_string (text);
    if (copy == NULL) goto errored;

errored:

    if (text != NULL) g_free (text); text = NULL;

    return copy;
}

