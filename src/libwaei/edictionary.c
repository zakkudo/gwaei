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
//!  @file edictionary.c
//!
//!  @brief LwEDictionary objects represent a loaded dictionary that the program
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

G_DEFINE_TYPE (LwEDictionary, lw_edictionary, LW_TYPE_DICTIONARY)

static LwResult* lw_edictionary_parse (LwDictionary*, const gchar*);


LwDictionary* lw_edictionary_new (const gchar        *FILENAME, 
                                  LwMorphologyEngine *morphologyengine)
{
    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (LW_TYPE_EDICTIONARY,
                                "filename", FILENAME,
                                "morphologyengine", morphologyengine,
                                NULL));

    return dictionary;
}


static void 
lw_edictionary_init (LwEDictionary *dictionary)
{
}


static void
lw_edictionary_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_edictionary_parent_class)->constructed (object);
    }

    LwDictionary *dictionary;
    LwDictionaryPrivate *priv;

    dictionary = LW_DICTIONARY (object);
    priv = dictionary->priv;

    if (strcmp(priv->config.filename, "English") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("English"));
    }
    else if (strcmp(priv->config.filename, "Names") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Names"));
    }
    else if (strcmp(priv->config.filename, "Places") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Places"));
    }
    else if (strcmp(priv->config.filename, "Names and Places") == 0)
    {
      if (priv->data.name != NULL) g_free (priv->data.name); priv->data.name = NULL;
      priv->data.name = g_strdup (gettext("Names and Places"));
    }
}


static void 
lw_edictionary_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_edictionary_parent_class)->finalize (object);
}


static void
lw_edictionary_class_init (LwEDictionaryClass *klass)
{
    //Declarations
    GObjectClass *object_class;
    LwDictionaryClass *dictionary_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = lw_edictionary_finalize;
    object_class->constructed = lw_edictionary_constructed;

    dictionary_class = LW_DICTIONARY_CLASS (klass);
    dictionary_class->priv->parse = lw_edictionary_parse;
}


static gchar*
_load_word (gchar                 *beginning,
            LwResultElementBuffer *buffer)
{
    //An example line
    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //＾
    //HERE

    //Declarations
    gchar *c = NULL;

    //Initializations
    c = beginning;
    if (c == NULL || c == '\0') return NULL;

    lw_resultelementbuffer_add (buffer, c);
    while (*c != ' ' && *c != '\0') c = g_utf8_next_char (c);
    if (*c == '\0') return NULL;

    *c = '\0';
    c++;

    return c;
}


static gchar*
_load_readings (gchar                 *beginning,
                LwResultElementBuffer *buffer)
{
    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //     ^
    //     HERE

    //Declarations
    gchar *c = NULL;

    //Initializations
    c = beginning;
    if (c == NULL || c == '\0') return NULL;

    while (*c != '\0' && *c != '/' && *c != '[') c = g_utf8_next_char (c);
    if (*c == '/') return beginning; //There was no reading section
    if (*c == '\0') return NULL;

    c = g_utf8_next_char (c);
    if (*c == '\0') return NULL;

    {
      gchar *e = c;
      while (*e != '\0' && *e != ']') e = g_utf8_next_char (e);
      if (*e == '\0') return NULL;
      if (e - c > 1) lw_resultelementbuffer_add (buffer, c);
      c = e;
    }

    if (*c == ']')
    {
      *c = '\0';
      c++;
    }

    if (*c == '\0') c = NULL;
    
    return c;
}


static gchar*
_load_classifications (gchar                 *beginning,
                       LwResultElementBuffer *buffer)
{
    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //                ^
    //                HERE

    //Declarations
    gchar *c = NULL;

    //Initializations
    c = beginning;
    if (c == NULL || c == '\0') return NULL;

    while (*c != '\0' && *c != '(' && *c != '/') c = g_utf8_next_char (c);
    if (*c == '/') while (*c != '\0' && *c != '(' && *c != '/') c = g_utf8_next_char (c);

    if (*c == '/') return beginning;
    else if (*c == '\0') return NULL;

    if (*c == '(') c = g_utf8_next_char (c);
    if (*c == '\0') return NULL;
    if (*c == ')') return beginning;

    {
      gchar *e = c;
      while (*e != ')' && *e != '(' && *e != '/' && *e != '\0' && !g_ascii_isdigit (*c)) e = g_utf8_next_char (c);
      if (*e == '/' || *e == '(' || g_ascii_isdigit (*e)) return beginning;
      if (*e == '\0') return NULL;
      if (*e == ')' && e - c > 1) lw_resultelementbuffer_add (buffer, c);
      c = e;
    }

    if (*c == ')') *c = '\0';
    c = g_utf8_next_char (c);

    if (c == '\0') return NULL;

    return c;
}


static gchar*
_load_definitions (gchar                 *beginning,
                   LwResultElementBuffer *buffer)
{
    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //                     ^
    //                     HERE

    //Declarations
    gchar *c = NULL;

    //Initializations
    c = beginning;
    if (c == NULL || c == '\0') return NULL;

    while (*c != '\0')
    {
      while (*c != '\0' && (*c == '/' || (*c == '(' && g_ascii_isdigit (*(c + 1))) || *c == ')' || g_ascii_isdigit (*c))) c = g_utf8_next_char (c);

      {
        gchar *e = c;
        if (*c == '\0') return NULL;
        while (*e != '\0' && *e != '/') e = g_utf8_next_char (e);
        if (*e == '\0') return NULL;
        if (e - c == 3 && strncmp("(P)", c, 3) == 0) return c; 
        if (e - c > 1) lw_resultelementbuffer_add (buffer, c);
        c = e;

        *c = '\0';
        *c++;
        if (*c == '\0') return NULL;
      }
    }

    return c;

errored:

    return NULL;
}


static gchar*
_load_popularity (gchar                 *beginning,
                  LwResultElementBuffer *buffer)
{
    //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
    //                                                                                             ^
    //                                                                                             HERE

    //Declarations
    gchar *c = NULL;

    //Initializations
    c = beginning;
    if (c == NULL || c == '\0') return NULL;

    while (*c != '\0' && *c == '/') c = g_utf8_next_char (c);
    if (*c == '\0') return NULL;

    if (strncpy("(P)", c, 3) == 0) lw_resultelementbuffer_add (buffer, gettext("Popular"));

    c += 3;
    if (*c == '\0') return NULL;

    *c = '\0';
    c++;
    if (*c == '\0') return NULL;

    return c;
}


//!
//! @brief, Retrieve a line from FILE, parse it according to the LwEDictionary rules and put the results into the LwResult
//!
static LwResult*
lw_edictionary_parse (LwDictionary       *dictionary,
                      const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwResult *result = NULL;
    gchar *buffer = NULL;
    LwResultElementBuffer words = {0};
    LwResultElementBuffer readings = {0};
    LwResultElementBuffer definitions = {0};
    LwResultElementBuffer classifications = {0};
    LwResultElementBuffer popular = {0};

    //Initializations
    result = lw_result_new (TEXT);
    if (result == NULL) goto errored;
    buffer = lw_result_get_buffer (result);
    if (buffer == NULL) goto errored;

    lw_result_init_elementbuffer (result, &words);
    lw_result_init_elementbuffer (result, &readings);
    lw_result_init_elementbuffer (result, &definitions);
    lw_result_init_elementbuffer (result, &classifications);
    lw_result_init_elementbuffer (result, &popular);

    {
      gchar *ptr = buffer;
      if (ptr == NULL || *ptr == '\0') goto errored;

      //An example line
      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //＾
      //HERE

      ptr = _load_word (ptr, &words);
      if (ptr == NULL) goto errored;

      //An example line
      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //     ^
      //     HERE

      ptr = _load_readings (ptr, &readings);
      if (ptr == NULL) goto errored;

      //An example line
      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                ^
      //                HERE

      ptr = _load_classifications (ptr, &readings);
      if (ptr == NULL) goto errored;

      //An example line
      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                     ^
      //                     HERE

      ptr = _load_definitions (ptr, &definitions);
      if (ptr == NULL) goto errored;

      //An example line
      //１日 [ついたち] /(n) (1) first day of the month/(2) (arch) first ten days of the lunar month/(P)/
      //                                                                                             ^
      //                                                                                             HERE

      ptr = _load_popularity (ptr, &popular);
      if (ptr == NULL) goto errored;

    }

    lw_result_take_elementbuffer (result, LW_EDICTIONARY_KEY_WORD, &words);
    lw_result_take_elementbuffer (result, LW_EDICTIONARY_KEY_READING, &readings);
    lw_result_take_elementbuffer (result, LW_EDICTIONARY_KEY_DEFINITION, &definitions);
    lw_result_take_elementbuffer (result, LW_EDICTIONARY_KEY_CLASSIFICATION, &classifications);
    lw_result_take_elementbuffer (result, LW_EDICTIONARY_KEY_POPULAR, &popular);

errored:

    lw_resultelementbuffer_clear (&words);
    lw_resultelementbuffer_clear (&readings);
    lw_resultelementbuffer_clear (&definitions);
    lw_resultelementbuffer_clear (&classifications);
    lw_resultelementbuffer_clear (&popular);

    return result;
}

