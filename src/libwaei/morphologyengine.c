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

G_DEFINE_TYPE (LwMorphologyEngine, lw_morphologyengine, G_TYPE_OBJECT)

typedef enum
{
  PROP_0,
  PROP_LOCALE
} LwMorphologyEngineProps;


LwMorphologyEngine* lw_morphologyengine_new (const gchar *HUNSPELL_PREFERED_LOCALE)
{
    //Declarations
    LwMorphologyEngine *engine;

    //Initializations
    engine = LW_MORPHOLOGYENGINE (g_object_new (LW_TYPE_MORPHOLOGYENGINE, "locale", HUNSPELL_PREFERED_LOCALE, NULL));

    return engine;
}


static void 
lw_morphologyengine_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwMorphologyEngine *engine;

    //Initializations
    engine = LW_MORPHOLOGYENGINE (object);

    switch (property_id)
    {
      case PROP_LOCALE:
        if (engine->locale != NULL) g_free (engine->locale);
        engine->locale = g_value_dup_string (value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_morphologyengine_get_property (GObject      *object,
                                  guint         property_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
    //Declarations
    LwMorphologyEngine *engine;

    //Initializations
    engine = LW_MORPHOLOGYENGINE (object);

    switch (property_id)
    {
      case PROP_LOCALE:
        g_value_set_string (value, engine->locale);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_morphologyengine_init (LwMorphologyEngine *engine)
{
}


static void
lw_morphologyengine_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (lw_morphologyengine_parent_class)->constructed (object);
    }

    LwMorphologyEngine *engine;

    engine = LW_MORPHOLOGYENGINE (object);

#ifdef HAVE_MECAB
    engine->mecab = lw_morphologyengine_mecab_new ();
#endif
#ifdef HAVE_HUNSPELL
    engine->hunspell = lw_morphologyengine_hunspell_new (engine->locale);
#endif
    g_mutex_init (&engine->mutex);

#ifdef HAVE_MECAB
    if (engine->mecab == NULL) 
        g_message (gettext("Mecab had errors loading.  You may need to install mecab-ipadic. "
                           "Until then, libwaei will not try to conjugate words to their root form for Japanese."));
#endif
#ifdef HAVE_HUNSPELL
    if (engine->hunspell == NULL)
        g_message (gettext("Hunspell had errors loading.  You may need to install some hunspell/myspell dictionaries. "
                           "Until then, libwaei will not try to conjugate words to their root form for english or spellcheck."));
#endif
}


static void 
lw_morphologyengine_finalize (GObject *object)
{
    G_OBJECT_CLASS (lw_morphologyengine_parent_class)->finalize (object);

    LwMorphologyEngine *engine;

    engine = LW_MORPHOLOGYENGINE (object);

#ifdef HAVE_MECAB
    if (engine->mecab != NULL) mecab_destroy (engine->mecab); 
#endif
#ifdef HAVE_HUNSPELL
    if (engine->hunspell != NULL) Hunspell_destroy (engine->hunspell); 
#endif
    g_mutex_clear (&engine->mutex);
    if (engine->locale != NULL) g_free (engine->locale); 

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
    object_class->set_property = lw_morphologyengine_set_property;
    object_class->get_property = lw_morphologyengine_get_property;
    object_class->finalize = lw_morphologyengine_finalize;
    object_class->constructed = lw_morphologyengine_constructed;

    pspec = g_param_spec_string ("locale",
                                 "Locale for european morphology",
                                 "Set the european morphology locale",
                                 "",
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_LOCALE, pspec);
}


//!
//! @brief Will analyze the sentence and return an array of non-trivial root form words
//!
LwMorphologyList*
lw_morphologyengine_analyze (LwMorphologyEngine *engine, const gchar *TEXT, gboolean spellcheck)
{
    //Sanity checks
    g_return_val_if_fail (engine != NULL, NULL);
    if (TEXT == NULL) return NULL;

    //Declarations and initializations
    GList *list= NULL;

#ifdef HAVE_MECAB
    if (engine->mecab != NULL) list = g_list_concat (list, lw_morphologyengine_mecab_analyze (engine, TEXT));
#endif
#ifdef HAVE_HUNSPELL
    if (engine->hunspell != NULL) list = g_list_concat (list, lw_morphologyengine_hunspell_analyze (engine, TEXT, spellcheck));
#endif

    return lw_morphologylist_new_from_list (list);
}



