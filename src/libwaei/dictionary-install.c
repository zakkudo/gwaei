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
//!  @file dictionary_install.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary_install that the program
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
#include <glib/gstdio.h>

#include <libwaei/gettext.h>
#include <libwaei/libwaei.h>

#include <libwaei/dictionary_install-private.h>

typedef enum {
    PROP_0,
    PROP_ID,
    PROP_NAME,
    PROP_DESCRIPTION,
    PROP_GTYPE,
    PROP_TEXT_ENCODING,
    PROP_PREFERENCES,
    PROP_DOWNLOAD_KEY,
    PROP_DOWNLOAD_URI,
    PROP_INSTALL_PATH,
    PROP_SPLIT_PLACES_FROM_NAMES,
    PROP_MERGE_RADICALS_INTO_KANJI,
    PROP_PROGRESS,
    PROP_STATUS,
    PROP_DEPENDENCIES,
    TOTAL_PROPS
} Props;

typedef enum {
    CLASS_SIGNALID_UNUSED,
    TOTAL_CLASS_SIGNALIDS
} SignalId;

struct _Index {
  GHashTable *id;
};

struct _LwDictionaryInstallPrivate {
  struct _Index index;
  gchar *id;
  gchar *name;
  gchar *description;
  LwPreferences *preferences;
  gchar *download_key;
  gchar *download_uri;
  gchar *text_encoding;
  GList *dependancies;
  GType gtype;
  LwProgress *progress;
  LwDictionaryInstallStateHistory *history;
  gchar *install_path;

  gboolean merge_radicals_into_kanji;
  gboolean split_places_from_names;
};


struct _LwDictionaryInstallClassPrivate {
  GParamSpec *pspec[TOTAL_PROPS];
  guint signalid[TOTAL_CLASS_SIGNALIDS];
};

const gchar* lw_dictionary_install_get_name (LwDictionaryInstall *self);
void lw_dictionary_install_set_name (LwDictionaryInstall *self, const gchar *NAME);


G_DEFINE_TYPE_WITH_PRIVATE (LwDictionaryInstall, lw_dictionary_install, G_TYPE_OBJECT)

typedef struct {
  GList *targets;
  LwProgress *progress;
  const gchar *STAGE_NAME;
  LwDictionaryInstall *dictionary_install;
} InstallStateData;
  

//!
//! @brief Creates a new LwDictionaryList object
//! @param MAX The maximum items you want in the self before old ones are deleted
//! @return An allocated LwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaryInstall* 
lw_dictionary_install_new ()
{
    LwDictionaryInstall *dictionary_install = NULL;

    //Initializations
    dictionary_install = LW_DICTIONARY_INSTALL (g_object_new (LW_TYPE_DICTIONARY_INSTALL, NULL));

    return dictionary_install;
}


static void 
lw_dictionary_install_init (LwDictionaryInstall *self)
{
    self->priv = LW_DICTIONARY_INSTALL_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryInstallPrivate));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    priv->history = lw_dictionary_installstatehistory_new ();
}


static void 
lw_dictionary_install_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY_INSTALL (object);
    priv = lw_dictionary_install_get_instance_private (self);

    switch (property_id)
    {
      case PROP_ID:
        lw_dictionary_install_set_id (self, g_value_get_string (value));
        break;
      case PROP_NAME:
        lw_dictionary_install_set_name (self, g_value_get_string (value));
        break;
      case PROP_DESCRIPTION:
        lw_dictionary_install_set_description (self, g_value_get_string (value));
        break;
      case PROP_GTYPE:
        lw_dictionary_install_set_gtype (self, g_value_get_gtype (value));        
        break;
      case PROP_TEXT_ENCODING:
        lw_dictionary_install_set_text_encoding (self, g_value_get_string (value));
        break;
      case PROP_DOWNLOAD_KEY:
        lw_dictionary_install_set_download_key (self, g_value_get_string (value));
        break;
      case PROP_PREFERENCES:
        lw_dictionary_install_set_preferences (self, g_value_get_object (value));
        break;
      case PROP_DOWNLOAD_URI:
        lw_dictionary_install_set_download_uri (self, g_value_get_string (value));
        break;
      case PROP_PROGRESS:
        lw_dictionary_install_set_progress (self, g_value_get_object (value));
        break;
      case PROP_DEPENDENCIES:
        lw_dictionary_install_set_dependancies (self, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionary_install_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY_INSTALL (object);
    priv = lw_dictionary_install_get_instance_private (self);

    switch (property_id)
    {
      case PROP_ID:
        g_value_set_string (value, lw_dictionary_install_get_id (self));
        break;
      case PROP_NAME:
        g_value_set_string (value, lw_dictionary_install_get_name (self));
        break;
      case PROP_DESCRIPTION:
        g_value_set_string (value, lw_dictionary_install_get_description (self));
        break;
      case PROP_GTYPE:
        g_value_set_gtype (value, lw_dictionary_install_get_gtype (self));
        break;
      case PROP_TEXT_ENCODING:
        g_value_set_string (value, lw_dictionary_install_get_text_encoding (self));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionary_install_get_preferences (self));
        break;
      case PROP_DOWNLOAD_KEY:
        g_value_set_string (value, lw_dictionary_install_get_download_key (self));
        break;
      case PROP_DOWNLOAD_URI:
        g_value_set_string (value, lw_dictionary_install_get_download_uri (self));
        break;
      case PROP_INSTALL_PATH:
        g_value_set_string (value, lw_dictionary_install_get_install_path (self));
        break;
      case PROP_PROGRESS:
        g_value_set_object (value, lw_dictionary_install_get_progress (self));
        break;
      case PROP_DEPENDENCIES:
        g_value_set_pointer (value, lw_dictionary_install_get_dependancies (self));
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionary_install_finalize (GObject *object)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARY_INSTALL (object);
    priv = lw_dictionary_install_get_instance_private (self);

    G_OBJECT_CLASS (lw_dictionary_install_parent_class)->finalize (object);
}


static void
lw_dictionary_install_dispose (GObject *object)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARY_INSTALL (object);
    priv = lw_dictionary_install_get_instance_private (self);

/*
    if (install->files != NULL) g_free (install->files); install->files = NULL;
    if (install->downloads != NULL) g_free (install->downloads); install->downloads = NULL;

    if (install->preferences != NULL && install->listenerid != 0)
    {
      lw_preferences_remove_change_listener_by_schema (install->preferences, LW_SCHEMA_DICTIONARY, install->listenerid);
      install->listenerid = 0;
    }
*/

    G_OBJECT_CLASS (lw_dictionary_install_parent_class)->dispose (object);
}


static void
lw_dictionary_install_class_init (LwDictionaryInstallClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryInstallClassPrivate, 1);
    object_class->set_property = lw_dictionary_install_set_property;
    object_class->get_property = lw_dictionary_install_get_property;
    object_class->dispose = lw_dictionary_install_dispose;
    object_class->finalize = lw_dictionary_install_finalize;

    LwDictionaryInstallClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_ID] = g_param_spec_string (
        "id",
        "dictionary install name",
        "Set the preferences object",
        NULL,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ID, klasspriv->pspec[PROP_ID]);

    klasspriv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        "dictionary install name",
        "Set the preferences object",
        "Unnamed",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_NAME, klasspriv->pspec[PROP_NAME]);

    klasspriv->pspec[PROP_DESCRIPTION] = g_param_spec_string (
        "description",
        "dictionary install name",
        "Set the preferences object",
        "No Description",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DESCRIPTION, klasspriv->pspec[PROP_DESCRIPTION]);

    klasspriv->pspec[PROP_GTYPE] = g_param_spec_gtype (
        "dictionary-gtype",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_DICTIONARY,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_GTYPE, klasspriv->pspec[PROP_GTYPE]);

    klasspriv->pspec[PROP_TEXT_ENCODING] = g_param_spec_string (
        "text-encoding",
        "dictionary install name",
        "Set the preferences object",
        "utf-8",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_TEXT_ENCODING, klasspriv->pspec[PROP_TEXT_ENCODING]);

    klasspriv->pspec[PROP_DOWNLOAD_KEY] = g_param_spec_string (
        "download-preference-key",
        "dictionary install name",
        "Set the preferences object",
        NULL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DOWNLOAD_KEY, klasspriv->pspec[PROP_DOWNLOAD_KEY]);

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);

    klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
        "progress",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_PROGRESS,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, klasspriv->pspec[PROP_PROGRESS]);

    klasspriv->pspec[PROP_DOWNLOAD_URI] = g_param_spec_string (
        "download-uri",
        "dictionary install name",
        "Set the preferences object",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DOWNLOAD_URI, klasspriv->pspec[PROP_DOWNLOAD_URI]);

    klasspriv->pspec[PROP_INSTALL_PATH] = g_param_spec_string (
        "install-path",
        "dictionary install name",
        "Set the preferences object",
        "",
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_INSTALL_PATH, klasspriv->pspec[PROP_INSTALL_PATH]);

    klasspriv->pspec[PROP_DEPENDENCIES] = g_param_spec_pointer (
        "dependencies",
        "Required dictionaries to be install",
        "A list of the prerequisite dictionaries for this to be installed.",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DEPENDENCIES, klasspriv->pspec[PROP_DEPENDENCIES]);
}


void
lw_dictionary_install_set_name (LwDictionaryInstall *self,
                               const gchar         *NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    if (g_strcmp0 (NAME, priv->name) == 0)
    {
      goto errored;
    }

    g_free (priv->name);
    priv->name = g_strdup (NAME);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_NAME]);

    lw_dictionary_install_sync_id (self);
    lw_dictionary_install_sync_install_path (self);

errored:

    return;
}


const gchar*
lw_dictionary_install_get_name (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->name;
}
                        

void
lw_dictionary_install_set_description (LwDictionaryInstall *self,
                                      const gchar         *DESCRIPTION)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (g_strcmp0 (DESCRIPTION, priv->description) != 0);

    if (priv->description != NULL)
    {
      g_free (priv->description);
    }

    priv->description = g_strdup (DESCRIPTION);

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DESCRIPTION]);
}


const gchar*
lw_dictionary_install_get_description (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->description;
}


void
lw_dictionary_install_set_text_encoding (LwDictionaryInstall *self,
                                        const gchar         *TEXT_ENCODING)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (g_strcmp0 (TEXT_ENCODING, priv->text_encoding) != 0);

    if (priv->text_encoding != NULL)
    {
      g_free (priv->text_encoding);
    }

    priv->text_encoding = g_strdup (TEXT_ENCODING);

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_TEXT_ENCODING]);
}


const gchar*
lw_dictionary_install_get_text_encoding (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->text_encoding;
}

void
lw_dictionary_install_set_gtype (LwDictionaryInstall *self,
                                GType                gtype)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));
    g_return_if_fail (g_type_is_a (gtype, LW_TYPE_DICTIONARY));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    if (gtype == priv->gtype)
    {
      goto errored;
    }
    
    priv->gtype = gtype;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_GTYPE]);
    lw_dictionary_install_sync_id (self);
    lw_dictionary_install_sync_install_path (self);

errored:

    return;
}


GType
lw_dictionary_install_get_gtype (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), G_TYPE_INVALID);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    
    return priv->gtype;
}


void
lw_dictionary_install_set_download_key (LwDictionaryInstall *self,
                                       const gchar         *DOWNLOAD_KEY)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (g_strcmp0 (DOWNLOAD_KEY, priv->download_key) != 0);
    if (changed == FALSE) goto errored;

    g_free (priv->download_key);
    priv->download_key = g_strdup (DOWNLOAD_KEY);

    lw_dictionary_install_sync_download_uri (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DOWNLOAD_KEY]);

errored:

    return;
}


const gchar*
lw_dictionary_install_get_download_key (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->download_key;
}


void
lw_dictionary_install_set_download_uri (LwDictionaryInstall *self,
                                       const gchar         *DOWNLOAD_URI)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (g_strcmp0 (DOWNLOAD_URI, priv->download_uri) != 0);
    if (changed == FALSE) goto errored;

    g_free (priv->download_uri);
    priv->download_uri = g_strdup (DOWNLOAD_URI);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DOWNLOAD_URI]);

errored:

    return;
}


void
lw_dictionary_install_sync_download_uri (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwPreferences *preferences = NULL;
    const gchar *KEY = NULL;
    gchar *uri = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    preferences = lw_dictionary_install_get_preferences (self);
    if (preferences == NULL) goto errored;
    KEY = lw_dictionary_install_get_download_key (self);
    if (KEY == NULL) goto errored;
    uri = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY);
    changed = (g_strcmp0 (uri, priv->download_uri) != 0);
    if (changed == FALSE) goto errored;

    lw_dictionary_install_set_download_uri (self, uri);

errored:

    g_free (uri); uri = NULL;
}


const gchar*
lw_dictionary_install_get_download_uri (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->download_uri;
}


const gchar*
lw_dictionary_install_get_install_path (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->install_path;
}


void
lw_dictionary_install_sync_install_path (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    GType type = G_TYPE_INVALID;
    const gchar *FILENAME = NULL;
    gchar *path = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    type = lw_dictionary_install_get_gtype (self);
    FILENAME = lw_dictionary_install_get_name (self);
    if (g_type_is_a (type, LW_TYPE_DICTIONARY) && FILENAME != NULL)
    {
      path = lw_dictionary_build_contents_path_by_type_and_name (type, FILENAME);
    }

    if (g_strcmp0 (path, priv->install_path) == 0) goto errored;

    g_free (priv->install_path);
    priv->install_path = path;
    path = NULL;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_INSTALL_PATH]);

errored:

    g_free (path); path = NULL;
}


void
lw_dictionary_install_set_preferences (LwDictionaryInstall *self,
                                      LwPreferences       *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (preferences != priv->preferences);
    if (!changed) goto errored;

    if (preferences != NULL)
    {
      g_object_ref (preferences);
    }

    if (priv->preferences != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->preferences),
        (gpointer) &(priv->preferences)
      );
      g_object_unref (priv->preferences);
    }

    priv->preferences = preferences;

    if (priv->preferences != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->preferences),
        (gpointer*) &(priv->preferences)
      );
    }

    lw_dictionary_install_sync_download_uri (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERENCES]);

errored:

    return;
}

LwPreferences*
lw_dictionary_install_get_preferences (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->preferences;
}


void
lw_dictionary_install_set_progress (LwDictionaryInstall *self,
                                   LwProgress          *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    changed = (progress != priv->progress);
    if (!changed) goto errored;

    if (progress != NULL)
    {
      g_object_ref (progress);
    }

    if (priv->progress != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->progress),
        (gpointer*) &(priv->progress)
      );
      g_object_unref (priv->progress);
    }

    priv->progress = progress;

    if (priv->progress != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->progress),
        (gpointer*) &(priv->progress)
      );
    }

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PROGRESS]);

errored:

    return;
}


LwProgress*
lw_dictionary_install_get_progress (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    if (priv->progress == NULL)
    {
      LwProgress *progress = lw_progress_new ();
      lw_dictionary_install_set_progress (self, progress);
      progress = NULL;
    }

    return priv->progress;
}


GList*
lw_dictionary_install_get_dependancies (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->dependancies;
}


void
lw_dictionary_install_set_dependancies (LwDictionaryInstall *self,
                                       GList               *dependancies)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;
    GList *sanitized_list_copy = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    if (dependancies == priv->dependancies) goto errored;

    {
      GList *link = NULL;
      for (link = dependancies; link != NULL; link = link->next)
      {
        LwDependancy *dependancy = lw_dependancy_copy (LW_DEPENDANCY (link->data));
        if (dependancy != NULL)
        {
          sanitized_list_copy = g_list_prepend (sanitized_list_copy, dependancy);
        }
      }
    }

    g_list_free_full (priv->dependancies, (GDestroyNotify) lw_dependancy_free);
    priv->dependancies = sanitized_list_copy;

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DEPENDENCIES]);

errored:

    return;
}


void
lw_dictionary_install_set_simple_boolean_dependancy (LwDictionaryInstall *self,
                                                    const gchar         *NAME,
                                                    const gchar         *PROPERTY_NAME,
                                                    gboolean             VALUE)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));
    g_return_if_fail (NAME != NULL);
    g_return_if_fail (PROPERTY_NAME != NULL);

    //Declarations
    GList *dependancies = NULL;
    {
      LwDependancy *dependancy = lw_dependancy_new (NAME);
      if (dependancy != NULL)
      {
        GList *conditions = NULL;
        {
          LwCondition *condition = lw_condition_new_boolean (PROPERTY_NAME, VALUE);
          if (condition != NULL)
          {
            conditions = g_list_prepend (conditions, condition);
            lw_dependancy_set_conditions (dependancy, conditions);
            g_list_free_full (conditions, (GDestroyNotify) lw_condition_free);
          }
        }
      }
      lw_dictionary_install_set_dependancies (self, dependancies);
      g_list_free_full (dependancies, (GDestroyNotify) lw_dependancy_free);
    }

errored:

    return;
}



const gchar*
lw_dictionary_install_get_id (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    return priv->id;
}


void
lw_dictionary_install_set_id (LwDictionaryInstall *self,
                             const gchar         *ID)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallClass *klass = NULL;
    LwDictionaryInstallClassPrivate *klasspriv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    klass = LW_DICTIONARY_INSTALL_CLASS (self);
    klasspriv = klass->priv;
    if (g_strcmp0 (priv->id, ID) == 0) goto errored;

    g_free (priv->id);
    priv->id = g_strdup (ID);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_ID]);

errored:

    return;
}


void
lw_dictionary_install_sync_id (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gchar *id = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    if (!g_type_is_a (priv->gtype, LW_TYPE_DICTIONARY) && priv->name) goto errored;
    id = g_strdup_printf("%s/%s", g_type_name (priv->gtype), priv->name);
    if (id == NULL) goto errored;

    lw_dictionary_install_set_id (self, id);

errored:

    g_free (id);
    id = NULL;
}

/*
//! @brief Tells the installer mechanism if it is going to fail if it tries
//!        installing because of missing info
//! @param self The LwDictionary objcet to check the validity of the urls of.  This tells you
//!        if the LWDictionary object can have the lw_installdictionary_install method
//!        called without crashing.
//!
gboolean 
lw_dictionary_installer_is_valid (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
		priv = lw_dictionary_install_get_instance_private (self);
    install = priv->install;

    return (install->files != NULL && install->downloads != NULL);
}
*/


static gchar*
_get_uri_basename (const gchar *URI)
{
    //Sanity checks
    if (URI == NULL) return NULL;

    //Declarations
    gint length = -1;
    gchar *c = NULL;
    gchar *basename = NULL;

    //Initializations
    length = strlen(URI);
    if (length < 1) goto errored;
    c = g_utf8_strrchr (URI, length, '/');
    if (c == NULL || *c == '\0') goto errored;
    basename = g_strdup(c + 1);

errored:

    return basename;
}


static gchar*
_try_get_filename_from_uri (const gchar *URI, gchar **basename)
{
    //Sanity checks
    if (URI == NULL) 
    {
      if (basename != NULL) *basename = NULL;
      return NULL;
    }

    //Declarations
    gchar *filename = NULL;

    if (g_file_test (URI, G_FILE_TEST_IS_REGULAR))
    {
      filename = g_strdup (URI);
    }
    else
    {
      filename = g_filename_from_uri (URI, NULL, NULL);
      if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR))
      {
        g_free (filename);
        filename = NULL;
      }
    }

    if (basename != NULL)
    {
      if (filename)
      {
        *basename = g_path_get_basename (filename);
      }
      else //Try to grab the basename from the uri
      {
        *basename = _get_uri_basename (URI);
      }
    }

    return filename;
}


gboolean 
lw_dictionary_install_download (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->progress)) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *URI = NULL;
    gchar *filename = NULL;
    gchar *basename = NULL;
    gchar *target = NULL;
    const gchar *STAGE_NAME = "download";
    GList *targets = NULL;
/*TODO
    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    progress = lw_dictionary_install_get_progress (self);
    URI = lw_dictionary_install_get_download_uri (self);
    if (URI == NULL) goto errored;
    filename = _try_get_filename_from_uri (URI, &basename);
    if (basename == NULL) goto errored;
    target = lw_util_build_filename (LW_PATH_CACHE, basename);
    if (target == NULL) goto errored;
    targets = g_list_prepend (targets, target);

    //File is located locally so copy it
    if (g_file_test (URI, G_FILE_TEST_IS_REGULAR))
    {
      const gchar *MESSAGE = gettext("Copying from %s...");
      lw_progress_set_secondary_message_printf (progress, MESSAGE, URI);
      lw_io_copy (URI, target, progress);
    }
    //Download the file
    else
    {
      const gchar *MESSAGE = gettext("Downloading from %s...");
      lw_progress_set_secondary_message_printf (progress, MESSAGE, URI);
      lw_io_download (URI, target, progress);
      printf("BREAK %s %s\n", URI, target);
    }

    lw_dictionary_installstatehistory_add_named_paths (priv->history, STAGE_NAME, TRUE, targets);

errored:

    g_free (filename); filename = NULL;
    g_free (basename); basename = NULL;
    g_free (target); target = NULL;
    g_list_free (targets);
*/
    return (!lw_progress_errored (progress));
}

static void
_decompress (LwDictionaryInstallState *self, LwFilePath *filepath, InstallStateData *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (filepath != NULL);

    const gchar *MESSAGE = gettext("Decompressing %s...");
    const gchar *SUFFIX = lw_filepath_get_suffix (filepath);
    const gchar *BASENAME = lw_filepath_get_basename (filepath);
    const gchar *SUFFIXLESS = lw_filepath_get_suffixless (filepath);
    const gchar *SOURCE = lw_filepath_get_path (filepath);

    if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) goto errored;

    lw_progress_set_secondary_message_printf (data->progress, MESSAGE, BASENAME);
    if (g_strcmp0 (SUFFIX, ".gz") || g_strcmp0 (SUFFIX, ".gzip"))
    {
      gchar *target = g_strjoin (".", SUFFIXLESS, data->STAGE_NAME, NULL);
      lw_io_gunzip_file (SOURCE, target, data->progress);
      data->targets = g_list_prepend (data->targets, target);
    }
    else
    {
      gchar *target = g_strjoin (".", SOURCE, data->STAGE_NAME, NULL);
      lw_io_copy (SOURCE, target, data->progress);
      data->targets = g_list_prepend (data->targets, target);
    }

errored:

    return;
}
             

gboolean 
lw_dictionary_install_decompress (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallState *state = NULL;
    GList *targets = NULL;

    InstallStateData data = {
      .targets = NULL,
      .progress = lw_dictionary_install_get_progress (self),
      .STAGE_NAME = "decompressed",
      .dictionary_install = self
    };

    //Initializations
		priv = lw_dictionary_install_get_instance_private (self);
    state = lw_dictionary_installstatehistory_current (priv->history);

    lw_dictionary_installstate_foreach (state, (LwDictionaryInstallStateForeachFunc) _decompress, &data);
    data.targets = g_list_reverse (data.targets);
    lw_dictionary_installstatehistory_add_named_paths (priv->history, data.STAGE_NAME, TRUE, data.targets);

errored:

    g_list_free_full (data.targets, (GDestroyNotify) g_free); data.targets = NULL;

    return (!lw_progress_errored (data.progress));
}


static void
_convert_text_encoding (LwDictionaryInstallState *self, 
                        LwFilePath               *filepath, 
                        InstallStateData         *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (filepath != NULL);

    //Declarations
    const gchar *MESSAGE = gettext("Converting encoding to UTF from %s...");
    const gchar *SUFFIX = lw_filepath_get_suffix (filepath);
    const gchar *BASENAME = lw_filepath_get_basename (filepath);
    const gchar *SUFFIXLESS = lw_filepath_get_suffixless (filepath);
    const gchar *SOURCE = lw_filepath_get_path (filepath);
    const gchar *TEXT_ENCODING = lw_dictionary_install_get_text_encoding (data->dictionary_install);

    if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) goto errored;

    lw_progress_set_secondary_message_printf (data->progress, MESSAGE, TEXT_ENCODING);
    
    if (g_strcmp0 (TEXT_ENCODING, "UTF-8") == 0)
    {
      gchar *target = g_strjoin (".", SUFFIXLESS, data->STAGE_NAME, NULL);
      lw_io_copy (SOURCE, target, data->progress);
      data->targets = g_list_prepend (data->targets, target);
    }
    else
    {
      gchar *target = g_strjoin (".", SUFFIXLESS, data->STAGE_NAME, NULL);
      lw_io_copy_with_encoding (SOURCE, target, TEXT_ENCODING, "UTF-8", data->progress);
      data->targets = g_list_prepend (data->targets, target);
    }

errored:

    return;
}


gboolean 
lw_dictionary_install_convert_text_encoding (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwDictionaryInstallState *state = NULL;
    GList *targets = NULL;

    InstallStateData data = {
      .targets = NULL,
      .progress = lw_dictionary_install_get_progress (self),
      .STAGE_NAME = "encoding",
      .dictionary_install = self
    };

    //Initializations
		priv = lw_dictionary_install_get_instance_private (self);
    state = lw_dictionary_installstatehistory_current (priv->history);
    if (state == NULL) goto errored;

    lw_dictionary_installstate_foreach (state, (LwDictionaryInstallStateForeachFunc) _convert_text_encoding, &data);
    data.targets = g_list_reverse (data.targets);
    lw_dictionary_installstatehistory_add_named_paths (priv->history, data.STAGE_NAME, TRUE, data.targets);

errored:

    g_list_free_full (data.targets, (GDestroyNotify) g_free); data.targets = NULL;

    return (!lw_progress_errored (data.progress));
}


static void
_finish (LwDictionaryInstallState *self, 
         LwFilePath               *filepath, 
         InstallStateData         *data)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (filepath != NULL);

    const gchar *MESSAGE = gettext("Finalizing...");
    const gchar *SUFFIX = lw_filepath_get_suffix (filepath);
    const gchar *BASENAME = lw_filepath_get_basename (filepath);
    const gchar *SUFFIXLESS = lw_filepath_get_suffixless (filepath);
    const gchar *SOURCE = lw_filepath_get_path (filepath);

    if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) goto errored;

    lw_progress_set_secondary_message (data->progress, MESSAGE);

    gchar *target = g_strdup (lw_dictionary_install_get_install_path (data->dictionary_install));
    lw_io_copy (SOURCE, target, data->progress);
    data->targets = g_list_prepend (data->targets, target);

errored:

    return;
}


gboolean 
lw_dictionary_install_finish (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    LwDictionaryInstallState *state = NULL;

    InstallStateData data = {
      .targets = NULL,
      .progress = lw_dictionary_install_get_progress (self),
      .STAGE_NAME = "finish",
      .dictionary_install = self
    };

    //Initializations
		priv = lw_dictionary_install_get_instance_private (self);
    progress = lw_dictionary_install_get_progress (self);
    state = lw_dictionary_installstatehistory_current (priv->history);
    if (state == NULL) goto errored;

    lw_dictionary_installstate_foreach (state, (LwDictionaryInstallStateForeachFunc) _finish, &data);
    data.targets = g_list_reverse (data.targets);
    lw_dictionary_installstatehistory_add_named_paths (priv->history, data.STAGE_NAME, FALSE, data.targets);

errored:

    g_list_free_full (data.targets, (GDestroyNotify) g_free); data.targets = NULL;

    return (!lw_progress_errored (data.progress));
}


static void
_clean (LwDictionaryInstallStateHistory *self,
        LwDictionaryInstallState        *state,
        gpointer                         data)
{
    //Sanity checks
    g_return_if_fail (state != NULL);

    if (lw_dictionary_installstate_is_temporary (state))
    {
      lw_dictionary_installstate_remove_files (state);
    }
}


void 
lw_dictionary_install_clean_temporary_files (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY_INSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);

    lw_dictionary_installstatehistory_foreach (priv->history, _clean, NULL);
}


gboolean 
lw_dictionary_install_install (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY_INSTALL (self), FALSE);
    if (lw_progress_errored (self->priv->progress)) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *NAME = NULL;
    const gchar *MESSAGE = NULL;

    //Initializations
    priv = lw_dictionary_install_get_instance_private (self);
    progress = lw_dictionary_install_get_progress (self);
    NAME = lw_dictionary_install_get_name (self);
    MESSAGE = gettext("Installing %s Dictionary...");

    lw_progress_set_primary_message_printf (progress, MESSAGE, NAME);

    lw_dictionary_install_download (self);
    lw_dictionary_install_decompress (self);
    lw_dictionary_install_convert_text_encoding (self);
    lw_dictionary_install_finish (self);
    lw_dictionary_install_clean_temporary_files (self);

errored:
  
    return (!lw_progress_errored (progress));
}


