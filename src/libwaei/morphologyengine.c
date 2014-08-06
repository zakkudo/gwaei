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
//! @file morphologyengine.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>

#ifdef HAVE_MECAB
#include <libwaei/morphologyengine-mecab.h>
#endif
#ifdef HAVE_HUNSPELL
#include <libwaei/morphologyengine-hunspell.h>
#endif

#include <libwaei/morphologyengine-private.h>

G_DEFINE_TYPE (LwMorphologyEngine, lw_morphologyengine, G_TYPE_OBJECT)

LwMorphologyEngine* lw_morphologyengine_new (const gchar *HUNSPELL_PREFERED_LOCALE)
{
    //Declarations
    LwMorphologyEngine *self;

    //Initializations
    self = LW_MORPHOLOGYENGINE (g_object_new (LW_TYPE_MORPHOLOGYENGINE, "locale", HUNSPELL_PREFERED_LOCALE, NULL));

    return self;
}


static void 
lw_morphologyengine_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwMorphologyEngine *self = NULL;

    //Initializations
    self = LW_MORPHOLOGYENGINE (object);

    switch (property_id)
    {
      case PROP_LOCALE:
        lw_morphologyengine_set_locale (self, g_value_get_string (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_morphologyengine_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    //Declarations
    LwMorphologyEngine *self = NULL;

    //Initializations
    self = LW_MORPHOLOGYENGINE (object);

    switch (property_id)
    {
      case PROP_LOCALE:
        g_value_set_string (value, lw_morphologyengine_get_locale (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_morphologyengine_init (LwMorphologyEngine *self)
{
    self->priv = LW_MORPHOLOGYENGINE_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwMorphologyEnginePrivate));
}


static void
lw_morphologyengine_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_morphologyengine_parent_class)->constructed (object);
    }

    LwMorphologyEngine *self = NULL;
    LwMorphologyEnginePrivate *priv = NULL;

    self = LW_MORPHOLOGYENGINE (object);
    priv = self->priv;

/*TODO
#ifdef HAVE_MECAB
    priv->mecab = lw_morphologyengine_mecab_new ();
#endif
#ifdef HAVE_HUNSPELL
    priv->hunspell = lw_morphologyengine_hunspell_new (priv->locale);
#endif
    */

#ifdef HAVE_MECAB
    if (priv->mecab == NULL) 
        g_message (gettext("Mecab had errors loading.  You may need to install mecab-ipadic. "
                           "Until then, libwaei will not try to conjugate words to their root form for Japanese."));
#endif
#ifdef HAVE_HUNSPELL
    if (priv->hunspell == NULL)
        g_message (gettext("Hunspell had errors loading.  You may need to install some hunspell/myspell dictionaries. "
                           "Until then, libwaei will not try to conjugate words to their root form for english or spellcheck."));
#endif
}


static void 
lw_morphologyengine_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_morphologyengine_parent_class)->finalize (object);

    LwMorphologyEngine *self = NULL;
    LwMorphologyEnginePrivate *priv = NULL;

    self = LW_MORPHOLOGYENGINE (object);
    priv = self->priv;

#ifdef HAVE_MECAB
    if (priv->mecab != NULL) mecab_destroy (priv->mecab); 
#endif
#ifdef HAVE_HUNSPELL
    if (priv->hunspell != NULL) Hunspell_destroy (priv->hunspell); 
#endif
    if (priv->locale != NULL) g_free (priv->locale); 

    memset(object + sizeof(GObject), 0, sizeof(LwMorphologyEngine) - sizeof(GObject));
}


static void
lw_morphologyengine_class_init (LwMorphologyEngineClass *klass)
{
    //Declarations
    GParamSpec *pspec;
    GObjectClass *object_class;
    //LwMorphologyEngineClass *morphologyengine_class;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwMorphologyEngineClassPrivate, 1);
    object_class->set_property = lw_morphologyengine_set_property;
    object_class->get_property = lw_morphologyengine_get_property;
    object_class->finalize = lw_morphologyengine_finalize;
    object_class->constructed = lw_morphologyengine_constructed;

    g_type_class_add_private (object_class, sizeof (LwMorphologyEnginePrivate));

    LwMorphologyEngineClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_LOCALE] = g_param_spec_string (
      "locale",
      "Locale for european morphology",
      "Set the european morphology locale",
      "",
      G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_LOCALE, klasspriv->pspec[PROP_LOCALE]);
}


void
lw_morphologyengine_set_locale (LwMorphologyEngine *self,
                                const gchar        *LOCALE)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MORPHOLOGYENGINE (self));

    //Declarations
    LwMorphologyEnginePrivate *priv = NULL;
    LwMorphologyEngineClass *klass = NULL;
    LwMorphologyEngineClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = LW_MORPHOLOGYENGINE_CLASS (self);
    klasspriv = klass->priv;
    changed = g_strcmp0 (LOCALE, priv->locale);

    if (priv->locale != NULL) g_free (priv->locale);
    priv->locale = g_strdup (LOCALE);

errored:

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_LOCALE]);
}


const gchar*
lw_morphologyengine_get_locale (LwMorphologyEngine *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MORPHOLOGYENGINE (self), NULL);

    //Declarations
    LwMorphologyEnginePrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->locale;
}


GList*
lw_morphologyengine_tokenize (LwMorphologyEngine *self,
                             const gchar        *TEXT)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    gchar **tokens = NULL;
    GList *list = NULL;
    gchar *copy = NULL;

    //Initializations
    copy = g_strdup (TEXT);
    
    //Generate copy
    {
      const gchar *p = NULL;
      gchar *output = copy;
      GUnicodeScript prev_script = G_UNICODE_SCRIPT_INVALID_CODE;
      for (p = TEXT; *p != '\0'; p = g_utf8_next_char (p))
      {
        gunichar c = g_utf8_get_char (p);
        GUnicodeScript script = g_unichar_get_script (c);

        if (g_unichar_ispunct (c) || g_unichar_isspace (c))
        {
          *output = '_';
          output++;
          script = G_UNICODE_SCRIPT_INVALID_CODE;
        }
        else
        {
          if (script != prev_script && prev_script != G_UNICODE_SCRIPT_INVALID_CODE)
          {
            *output = '_';
            output++;
          }

          gint bytes = g_unichar_to_utf8 (c, output);
          output += bytes;
        }

        prev_script = script;
      }
      *output = '\0';
    }

    //Initializations
    tokens = g_strsplit (copy, "_", -1);
    if (tokens == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; tokens[i] != NULL; i++)
      {
          if (strlen(tokens[i]) > 0)
          {
            list = g_list_prepend (list, tokens[i]);
          }
      }
      list = g_list_reverse (list);
    }

    g_free (tokens); tokens = NULL;

errored:

    if (copy != NULL) g_free (copy); copy = NULL;
    if (tokens != NULL) g_strfreev (tokens); tokens = NULL;

    return list;
}


//!
//! @brief Will analyze the sentence and return an array of non-trivial root form words
//!
LwMorphologyString*
lw_morphologyengine_parse (LwMorphologyEngine   *self, 
                           const gchar          *TEXT,
                           LwNormalizationFlags  flags)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations
    LwMorphologyEnginePrivate *priv = NULL;
    LwMorphologyString *morphologystring = NULL;

    //Initializations
    priv = self->priv;
    morphologystring = lw_morphologystring_new (TEXT, flags);
    if (morphologystring == NULL) goto errored;

    if (morphologystring->tokens != NULL) g_list_free_full (morphologystring->tokens, g_free);
    morphologystring->tokens = lw_morphologyengine_tokenize (self, TEXT);

/*
#ifdef HAVE_MECAB
    if (priv->mecab != NULL) lw_morphologyengine_mecab_parse (self, morphologystring);
#endif

#ifdef HAVE_HUNSPELL
    if (priv->hunspell != NULL) lw_morphologyengine_hunspell_parse (self, morphologystring, spellcheck);
#endif
*/

errored:

    return morphologystring;
}



