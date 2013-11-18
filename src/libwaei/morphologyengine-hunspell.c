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
//! @file morphology-hunspell.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

//!
//! Searches for where the user stores their dictionaries
//!
static gchar**
lw_morphologyengine_hunspell_get_dictionary_paths ()
{
    //Declarations
    gchar **path = NULL;
    gchar *text = NULL;
    const gchar * const * system_path = g_get_system_data_dirs ();
    gint i = 0;

    for (i = 0; system_path[i] != NULL; i++)
    {
      gchar *temp_path1 = g_build_filename (system_path[i], "myspell", NULL);
      gchar *temp_path2 = g_build_filename (system_path[i], "myspell", "dicts", NULL);
      gchar *temp_path3 = g_build_filename (system_path[i], "hunspell", NULL);

      text = g_strjoin (":", HUNSPELL_MYSPELL_DICTIONARY_PATH, temp_path1, temp_path2, temp_path3, NULL);

      if (temp_path1 != NULL) g_free (temp_path1); temp_path1 = NULL;
      if (temp_path2 != NULL) g_free (temp_path2); temp_path2 = NULL;
      if (temp_path3 != NULL) g_free (temp_path3); temp_path3 = NULL;
    }

    if (text != NULL) path = g_strsplit (text, ":", -1);

    if (text != NULL) g_free (text); text = NULL;

    return path;
}


static gchar* 
lw_morphologyengine_hunspell_build_noramalized_locale (const gchar *NAME)
{
    GDir *dir;
    gchar *locale;
    gchar *ptr;
    gchar **pathlist;
    const gchar *FILENAME;
    gint i;
    gint length;

    locale = g_strdup (NAME);

    //Truncate the UTF8 part of en_US.UTF8
    ptr = strchr(locale, '.');  
    if (ptr != NULL) *ptr = '\0'; 

    pathlist = lw_morphologyengine_hunspell_get_dictionary_paths ();
    if (pathlist != NULL)
    {
      //If the local lacks a country name, try to bulid one
      ptr = strchr(locale, '_'); 
      if (ptr == NULL)
      {
        for (i = 0; ptr == NULL && pathlist[i] != NULL; i++)
        {
          dir = g_dir_open (pathlist[i], 0, NULL);
          if (dir != NULL)
          {
            length = strlen (locale);
            while (ptr == NULL && (FILENAME = g_dir_read_name (dir)) != NULL)
            {
              if (strncmp(FILENAME, locale, length) == 0)
              {
                if (locale != NULL) g_free (locale);
                locale = g_strdup (FILENAME);
                ptr = strchr(locale, '.');  
                if (ptr != NULL) *ptr = '\0';
                ptr = strchr(locale, '_');
              }
            }
            g_dir_close (dir); dir = NULL;
          }
        }
      }
      g_strfreev (pathlist); pathlist = NULL;
    }

    //Wasn't a valid locale for a dictionary
    if (ptr == NULL && locale != NULL)
    {
      g_free (locale); locale = NULL;
    }
   
    return locale; 
}


static Hunhandle*
lw_morphologyengine_hunspell_new_by_locale (const gchar *LOCALE)
{
    gchar **pathlist;
    gchar *path, *dpath, *affpath;
    gchar *locale;
    gint i;
    Hunhandle *handle;

    handle = NULL;

    locale = lw_morphologyengine_hunspell_build_noramalized_locale (LOCALE);
    if (locale != NULL)
    {
      pathlist = lw_morphologyengine_hunspell_get_dictionary_paths ();
      if (pathlist != NULL)
      {
        for (i = 0; handle == NULL && pathlist[i] != NULL; i++)
        {
          path = g_build_filename (pathlist[i], locale, NULL);
          dpath = g_strjoin (".", path, "dic", NULL);
          affpath = g_strjoin (".", path, "aff", NULL);
          if (g_file_test (affpath, G_FILE_TEST_IS_REGULAR) && 
              g_file_test (dpath, G_FILE_TEST_IS_REGULAR))
            handle = Hunspell_create (affpath, dpath);
          if (path != NULL) g_free (path); path = NULL;
          if (dpath != NULL) g_free (dpath); dpath = NULL;
          if (affpath != NULL) g_free (affpath); affpath = NULL;
        }
        g_strfreev (pathlist); pathlist = NULL;
      }
      g_free (locale); locale = NULL;
    }
    
    return handle;
}


Hunhandle*
lw_morphologyengine_hunspell_new (const gchar *PREFERED_LOCALE)
{
    //Declarations
    const gchar *locale = setlocale(LC_ALL, NULL);
    Hunhandle *handle = NULL;

    //See if we should try setting the prefered handle
    if (handle == NULL && PREFERED_LOCALE != NULL && strncmp("auto", PREFERED_LOCALE, 4) != 0)
      handle = lw_morphologyengine_hunspell_new_by_locale (PREFERED_LOCALE);

    //Load from environment locale if it starts with en
    if (handle == NULL && strncmp("en", locale, strlen("en")) == 0)
      handle = lw_morphologyengine_hunspell_new_by_locale (locale);

    //Load from en_US
    if (handle == NULL)
      handle = lw_morphologyengine_hunspell_new_by_locale ("en_US");

    //Load from en
    if (handle == NULL)
      handle = lw_morphologyengine_hunspell_new_by_locale ("en");

    return handle;
}


static gchar*
lw_morphologyengine_hunspell_stem (LwMorphologyEngine *engine, 
                                   const gchar        *WORD)
{
    //Sanity checks
    if (engine == NULL) return NULL;
    if (engine->hunspell == NULL) return NULL;
    if (WORD == NULL) return NULL;
    
    gchar **suggestions;
    gint total;
    gchar *output;
    gint i;

    total = Hunspell_stem (engine->hunspell, &suggestions, WORD); 
    output = NULL;

    if (suggestions != NULL)
    {
      for (i = total - 1; i >= 0 && output == NULL; i--) //The results at the end tend to be the most core form
      {
        if (g_ascii_strcasecmp (WORD, suggestions[i]) != 0)  //Make sure we aren't just getting the lower case form of the word
          output = g_strdup (suggestions[i]);
      }
      Hunspell_free_list (engine->hunspell, &suggestions, total); suggestions = NULL;
    }

    return output;
}


static gchar*
lw_morphologyengine_hunspell_spellcheck (LwMorphologyEngine *engine,
                                         const gchar        *WORD)
{
    //Sanity checks
    if (engine == NULL) return NULL;
    if (engine->hunspell == NULL) return NULL;
    if (WORD == NULL) return NULL;

    if (Hunspell_spell (engine->hunspell, WORD) != 0) return NULL;

    //Delarations
    gchar **suggestions = NULL;
    GString *output = NULL;
    gint i = 0;
    gint total = 0;

    //Initializations
    total = Hunspell_suggest (engine->hunspell, &suggestions, WORD);
    output = g_string_new ("");

    if (suggestions != NULL)
    {
      for (i = 0; i < total; i++)
      {
        if (*output->str == '\0') g_string_assign (output, suggestions[i]);
        else g_string_append_printf (output, LW_MORPHOLOGY_SPELLCHECK_DELIMITOR "%s", suggestions[i]);
      }
      Hunspell_free_list (engine->hunspell, &suggestions, total); suggestions = NULL;
    }

    return g_string_free (output, total < 1);
}


//!
//! @brief Analyzes a sentence for misspellings, positions, and stem forms of words
//!
GList*
lw_morphologyengine_hunspell_analyze (LwMorphologyEngine *engine, 
                                      const gchar        *TEXT, 
                                      gboolean            include_spellcheck)
{
    //Sanity checks
    if (engine == NULL) return NULL;
    if (engine->hunspell == NULL) return NULL;
    if (TEXT == NULL) return NULL;

    //Declations
    gint start_offset = 0, end_offset = 0;
    GMatchInfo *match_info = NULL;
    GList *list = NULL;
    gchar *word = NULL;
    LwMorphology *morphology = NULL;

    //Initializations
    gchar *shortened = lw_regex_remove_parenthesis (TEXT);

    //Body
    lw_regex_get_contiguous (shortened, &match_info);

    while (g_match_info_matches (match_info))
    {
        word = g_match_info_fetch (match_info, 0);
        if (word != NULL && !g_unichar_ispunct (g_utf8_get_char (word)) && !lw_util_string_has_japanese (word))
        {
          g_match_info_fetch_pos (match_info, 0, &start_offset, &end_offset);

          gchar *normalized = NULL, *stem = NULL, *canonical = NULL, *spellcheck = NULL;

          //Generate the forms
          normalized = lw_util_normalize_string (word, TRUE, FALSE);    
          stem = lw_morphologyengine_hunspell_stem (engine, word);
          if (stem != NULL) canonical = lw_util_normalize_string (stem, TRUE, FALSE); //You don't want to case fold before hunspell works
          if (include_spellcheck) spellcheck = lw_morphologyengine_hunspell_spellcheck (engine, word);

          //Cleanup identicals
          if (normalized != NULL)
          {
            if (strcmp(normalized, word) == 0) { g_free (normalized); normalized = NULL; }
          }
          if (stem != NULL)
          {
            if (strcmp(stem, word) == 0) { g_free (stem); stem = NULL; }
            if (canonical != NULL && strcmp(stem, canonical) == 0) { g_free (canonical); canonical = NULL; } //Canonical is built on stem
          }

          morphology = lw_morphology_new (
            word,
            normalized,
            stem,
            canonical,
            spellcheck,
            NULL,
            start_offset,
            end_offset
          );
          if (morphology != NULL)
          {
            list = g_list_append (list, morphology);
            morphology = NULL;
          }
          word = NULL;
        }
        else if (word != NULL)
        {
          g_free (word); word = NULL;
        }
        g_match_info_next (match_info, NULL);
    }

errored:

    if (match_info != NULL) g_match_info_free (match_info); match_info = NULL;
    if (shortened != NULL) g_free(shortened); shortened = NULL;
    if (morphology != NULL) lw_morphology_free (morphology); morphology = NULL;
    if (word != NULL) g_free (word); word = NULL;

    return list;
}

