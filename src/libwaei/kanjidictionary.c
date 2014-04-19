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
//!  @file kanjidictionary.c
//!
//!  @brief LwKanjiDictionary objects represent a loaded dictionary that the program
//!         can use to carry out searches.  You can uninstall dictionaries
//!         by using the object, but you cannot install them. LwDictInst
//!         objects exist for that purpose.
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>
#include <libwaei/dictionary-private.h>

G_DEFINE_TYPE (LwKanjiDictionary, lw_kanjidictionary, LW_TYPE_DICTIONARY)

static LwResult* lw_kanjidictionary_parse (LwDictionary*, const gchar*);
static gboolean lw_kanjidictionary_installer_postprocess (LwDictionary*, gchar**, gchar**, LwProgress*);


LwDictionary* lw_kanjidictionary_new (const gchar        *FILENAME, 
                                      LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_KANJIDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_kanjidictionary_init (LwKanjiDictionary *dictionary)
{
}


static void
lw_kanjidictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->filename, "Kanji") == 0)
    {
      if (priv->name != NULL) g_free (priv->name); priv->name = NULL;
      priv->name = g_strdup (gettext("Kanji"));
    }

}


static void 
lw_kanjidictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_kanjidictionary_parent_class)->finalize (object);
}


static void
lw_kanjidictionary_class_init (LwKanjiDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_kanjidictionary_finalize;
    object_class->constructed = lw_kanjidictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->priv->parse = lw_kanjidictionary_parse;
    dictionary_class->priv->installer_postprocess = lw_kanjidictionary_installer_postprocess;
}


//!
//! @brief, Retrieve a line from FILE, parse it according to the LwKanjiDictionary rules and put the results into the LwResult
//!
static LwResult*
lw_kanjidictionary_parse (LwDictionary       *dictionary, 
                          const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;

/*TODO
    //Declarations
    GMatchInfo* match_info = NULL;
    gint start[LW_RE_TOTAL];
    gint end[LW_RE_TOTAL];
    GUnicodeScript script;
    if (result->text != NULL) g_free (result->text);
    gchar *ptr = result->text = g_strdup (TEXT);

    //First generate the grade, stroke, frequency, and jlpt fields

    //Get strokes
    result->strokes = NULL;
    g_regex_match (lw_re[LW_RE_STROKES], ptr, 0, &match_info);
    if (g_match_info_matches (match_info))
    {
      g_match_info_fetch_pos (match_info, 0, &start[LW_RE_STROKES], &end[LW_RE_STROKES]);
      result->strokes = ptr + start[LW_RE_STROKES] + 1;
    }
    g_match_info_free (match_info);

    //Get frequency
    result->frequency = NULL;
    g_regex_match (lw_re[LW_RE_FREQUENCY], ptr, 0, &match_info);
    if (g_match_info_matches (match_info))
    {
      g_match_info_fetch_pos (match_info, 0, &start[LW_RE_FREQUENCY], &end[LW_RE_FREQUENCY]);
      result->frequency = ptr + start[LW_RE_FREQUENCY] + 1;
    }
    g_match_info_free (match_info);


    //Get grade level
    result->grade = NULL;
    g_regex_match (lw_re[LW_RE_GRADE], ptr, 0, &match_info);
    if (g_match_info_matches (match_info))
    {
      g_match_info_fetch_pos (match_info, 0, &start[LW_RE_GRADE], &end[LW_RE_GRADE]);
      result->grade = ptr + start[LW_RE_GRADE] + 1;
    }
    g_match_info_free (match_info);

    //Get JLPT level
    result->jlpt = NULL;
    g_regex_match (lw_re[LW_RE_JLPT], ptr, 0, &match_info);
    if (g_match_info_matches (match_info))
    {
      g_match_info_fetch_pos (match_info, 0, &start[LW_RE_JLPT], &end[LW_RE_JLPT]);
      result->jlpt = ptr + start[LW_RE_JLPT] + 1;
    }
    g_match_info_free (match_info);


    //Get the kanji character
    result->kanji = ptr;
    ptr = g_utf8_strchr (ptr, -1, g_utf8_get_char (" "));
    if (ptr == NULL)
    {
      fprintf(stderr, "This dictionary is incorrectly formatted\n");
      exit (1);
    }
    *ptr = '\0';
    ptr++;

    //Test if the radicals information is present
    result->radicals = NULL;
    script = g_unichar_get_script (g_utf8_get_char (ptr));
    if (script != G_UNICODE_SCRIPT_LATIN)
    {
      result->radicals = ptr;
      ptr = g_utf8_next_char (ptr);
      script = g_unichar_get_script (g_utf8_get_char (ptr));
      while (*ptr == ' ' || (script != G_UNICODE_SCRIPT_LATIN && script != G_UNICODE_SCRIPT_COMMON))
      {
        ptr = g_utf8_next_char(ptr);
        script = g_unichar_get_script (g_utf8_get_char (ptr));
      }
      *(ptr - 1) = '\0';
    }

    //Go to the readings section
    script = g_unichar_get_script (g_utf8_get_char(ptr));
    while (script != G_UNICODE_SCRIPT_KATAKANA && script != G_UNICODE_SCRIPT_HIRAGANA && *ptr != '\0')
    {
      ptr = g_utf8_next_char (ptr);
      script = g_unichar_get_script (g_utf8_get_char(ptr));
    }
    result->readings[0] = ptr;

    //Copy the rest of the data
    while (*ptr != '\0' && *ptr != '{')
    {
      //The strange T1 character between kana readings
      if (g_utf8_get_char (ptr) == g_utf8_get_char ("T")) {
        ptr = g_utf8_next_char (ptr);
        if (g_utf8_get_char (ptr) == g_utf8_get_char ("1"))
        {
          *(ptr - 1) = '\0';
          ptr = g_utf8_next_char (ptr);
          ptr = g_utf8_next_char (ptr);
          result->readings[1] = ptr;
        }
        else if (g_utf8_get_char (ptr) == g_utf8_get_char ("2"))
        {
          *(ptr - 1) = '\0';
          ptr = g_utf8_next_char (ptr);
          ptr = g_utf8_next_char (ptr);
          result->readings[2] = ptr;
        }
      }
      else
      {
        ptr = g_utf8_next_char (ptr);
      }
    }
    *(ptr - 1) = '\0';

    result->meanings = ptr;

    if ((ptr = g_utf8_strrchr (ptr, -1, g_utf8_get_char ("\n"))) != NULL)
      *ptr = '\0';

    if (result->strokes)   *(result->text + end[LW_RE_STROKES]) = '\0';
    if (result->frequency) *(result->text + end[LW_RE_FREQUENCY]) = '\0';
    if (result->grade)     *(result->text + end[LW_RE_GRADE]) = '\0';
    if (result->jlpt)      *(result->text + end[LW_RE_JLPT]) = '\0';

    return 1;
*/

    return result;
}


static gboolean
lw_kanjidictionary_installer_postprocess (LwDictionary  *dictionary, 
                                          gchar        **sourcelist, 
                                          gchar        **targetlist, 
                                          LwProgress    *progress)
{
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (progress != NULL, FALSE);
    g_return_val_if_fail (g_strv_length (sourcelist) > 1, FALSE);
    g_return_val_if_fail (g_strv_length (targetlist) > 0, FALSE);

    return lw_io_create_mix_dictionary (targetlist[0], sourcelist[0], sourcelist[1], progress);
}


