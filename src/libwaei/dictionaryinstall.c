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
//!  @file dictionaryinstall.c
//!
//!  @brief LwDictionary objects represent a loaded dictionaryinstall that the program
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

#include <libwaei/dictionaryinstall-private.h>

static LwDictionaryInstallClass *_klass = NULL;
static LwDictionaryInstallClassPrivate *_klasspriv = NULL;

G_DEFINE_TYPE (LwDictionaryInstall, lw_dictionaryinstall, G_TYPE_OBJECT)

//!
//! @brief Creates a new LwDictionaryList object
//! @param MAX The maximum items you want in the self before old ones are deleted
//! @return An allocated LwDictionaryList that will be needed to be freed by lw_dictionary_free.
//!
LwDictionaryInstall* 
lw_dictionaryinstall_new ()
{
    LwDictionaryInstall *dictionaryinstall = NULL;

    //Initializations
    dictionaryinstall = LW_DICTIONARYINSTALL (g_object_new (LW_TYPE_DICTIONARYINSTALL, NULL));

    return dictionaryinstall;
}


static void 
lw_dictionaryinstall_init (LwDictionaryInstall *self)
{
    self->priv = LW_DICTIONARYINSTALL_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(LwDictionaryInstallPrivate));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    priv->data.history = lw_dictionaryinstallstatehistory_new ();
}


static void 
lw_dictionaryinstall_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALL (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ID:
        lw_dictionaryinstall_set_id (self, g_value_get_string (value));
        break;
      case PROP_NAME:
        lw_dictionaryinstall_set_name (self, g_value_get_string (value));
        break;
      case PROP_DESCRIPTION:
        lw_dictionaryinstall_set_description (self, g_value_get_string (value));
        break;
      case PROP_GTYPE:
        lw_dictionaryinstall_set_gtype (self, g_value_get_gtype (value));        
        break;
      case PROP_TEXT_ENCODING:
        lw_dictionaryinstall_set_text_encoding (self, g_value_get_string (value));
        break;
      case PROP_DOWNLOAD_KEY:
        lw_dictionaryinstall_set_download_key (self, g_value_get_string (value));
        break;
      case PROP_PREFERENCES:
        lw_dictionaryinstall_set_preferences (self, g_value_get_object (value));
        break;
      case PROP_DOWNLOAD_URI:
        lw_dictionaryinstall_set_download_uri (self, g_value_get_string (value));
        break;
      case PROP_SPLIT_PLACES_FROM_NAMES:
        lw_dictionaryinstall_set_split_places_from_names (self, g_value_get_boolean (value));
        break;
      case PROP_MERGE_RADICALS_INTO_KANJI:
        lw_dictionaryinstall_set_merge_radicals_into_kanji (self, g_value_get_boolean (value));
        break;
      case PROP_PROGRESS:
        lw_dictionaryinstall_set_progress (self, g_value_get_object (value));
        break;
      case PROP_DEPENDENCIES:
        lw_dictionaryinstall_set_dependancies (self, g_value_get_pointer (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaryinstall_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALL (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_ID:
        g_value_set_string (value, lw_dictionaryinstall_get_id (self));
        break;
      case PROP_NAME:
        g_value_set_string (value, lw_dictionaryinstall_get_name (self));
        break;
      case PROP_DESCRIPTION:
        g_value_set_string (value, lw_dictionaryinstall_get_description (self));
        break;
      case PROP_GTYPE:
        g_value_set_gtype (value, lw_dictionaryinstall_get_gtype (self));
        break;
      case PROP_TEXT_ENCODING:
        g_value_set_string (value, lw_dictionaryinstall_get_text_encoding (self));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, lw_dictionaryinstall_get_preferences (self));
        break;
      case PROP_DOWNLOAD_KEY:
        g_value_set_string (value, lw_dictionaryinstall_get_download_key (self));
        break;
      case PROP_DOWNLOAD_URI:
        g_value_set_string (value, lw_dictionaryinstall_get_download_uri (self));
        break;
      case PROP_INSTALL_PATH:
        g_value_set_string (value, lw_dictionaryinstall_get_install_path (self));
        break;
      case PROP_SPLIT_PLACES_FROM_NAMES:
        g_value_set_boolean (value, lw_dictionaryinstall_get_split_places_from_names (self));
        break;
      case PROP_MERGE_RADICALS_INTO_KANJI:
        g_value_set_boolean (value, lw_dictionaryinstall_get_merge_radicals_into_kanji (self));
        break;
      case PROP_PROGRESS:
        g_value_set_object (value, lw_dictionaryinstall_get_progress (self));
        break;
      case PROP_DEPENDENCIES:
        g_value_set_pointer (value, lw_dictionaryinstall_get_dependancies (self));
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_dictionaryinstall_finalize (GObject *object)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initalizations
    self = LW_DICTIONARYINSTALL (object);
    priv = self->priv;

    G_OBJECT_CLASS (lw_dictionaryinstall_parent_class)->finalize (object);
}


static void
lw_dictionaryinstall_dispose (GObject *object)
{
    //Declarations
    LwDictionaryInstall *self = NULL;
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    self = LW_DICTIONARYINSTALL (object);
    priv = self->priv;

/*
    if (install->files != NULL) g_free (install->files); install->files = NULL;
    if (install->downloads != NULL) g_free (install->downloads); install->downloads = NULL;

    if (install->decompresslist != NULL) g_strfreev (install->decompresslist); install->decompresslist = NULL;
    if (install->encodelist) g_strfreev (install->encodelist); install->encodelist = NULL;
    if (install->postprocesslist) g_strfreev (install->postprocesslist); install->postprocesslist = NULL;
    if (install->installlist) g_strfreev (install->installlist); install->installlist = NULL;
    if (install->installedlist) g_strfreev (install->installedlist); install->installedlist = NULL;

    if (install->preferences != NULL && install->listenerid != 0)
    {
      lw_preferences_remove_change_listener_by_schema (install->preferences, LW_SCHEMA_DICTIONARY, install->listenerid);
      install->listenerid = 0;
    }
*/

    G_OBJECT_CLASS (lw_dictionaryinstall_parent_class)->dispose (object);
}


static void
lw_dictionaryinstall_class_init (LwDictionaryInstallClass *klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (LwDictionaryInstallClassPrivate, 1);
    object_class->set_property = lw_dictionaryinstall_set_property;
    object_class->get_property = lw_dictionaryinstall_get_property;
    object_class->dispose = lw_dictionaryinstall_dispose;
    object_class->finalize = lw_dictionaryinstall_finalize;

    g_type_class_add_private (object_class, sizeof (LwDictionaryInstallPrivate));

    _klass = klass;
    _klasspriv = klass->priv;

    _klasspriv->pspec[PROP_ID] = g_param_spec_string (
        "id",
        "dictionary install name",
        "Set the preferences object",
        NULL,
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_ID, _klasspriv->pspec[PROP_ID]);

    _klasspriv->pspec[PROP_NAME] = g_param_spec_string (
        "name",
        "dictionary install name",
        "Set the preferences object",
        "Unnamed",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_NAME, _klasspriv->pspec[PROP_NAME]);

    _klasspriv->pspec[PROP_DESCRIPTION] = g_param_spec_string (
        "description",
        "dictionary install name",
        "Set the preferences object",
        "No Description",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DESCRIPTION, _klasspriv->pspec[PROP_DESCRIPTION]);

    _klasspriv->pspec[PROP_GTYPE] = g_param_spec_gtype (
        "dictionary-gtype",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_DICTIONARY,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_GTYPE, _klasspriv->pspec[PROP_GTYPE]);

    _klasspriv->pspec[PROP_TEXT_ENCODING] = g_param_spec_string (
        "text-encoding",
        "dictionary install name",
        "Set the preferences object",
        "utf-8",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_TEXT_ENCODING, _klasspriv->pspec[PROP_TEXT_ENCODING]);

    _klasspriv->pspec[PROP_DOWNLOAD_KEY] = g_param_spec_string (
        "download-preference-key",
        "dictionary install name",
        "Set the preferences object",
        NULL,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DOWNLOAD_KEY, _klasspriv->pspec[PROP_DOWNLOAD_KEY]);

    _klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, _klasspriv->pspec[PROP_PREFERENCES]);

    _klasspriv->pspec[PROP_SPLIT_PLACES_FROM_NAMES] = g_param_spec_boolean (
        "split-places-from-names",
        "dictionary install name",
        "Set the preferences object",
        FALSE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_SPLIT_PLACES_FROM_NAMES, _klasspriv->pspec[PROP_SPLIT_PLACES_FROM_NAMES]);

    _klasspriv->pspec[PROP_MERGE_RADICALS_INTO_KANJI] = g_param_spec_boolean (
        "merge-radicals-into-kanji",
        "dictionary install name",
        "Set the preferences object",
        FALSE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_MERGE_RADICALS_INTO_KANJI, _klasspriv->pspec[PROP_MERGE_RADICALS_INTO_KANJI]);

    _klasspriv->pspec[PROP_PROGRESS] = g_param_spec_object (
        "progress",
        "dictionary install name",
        "Set the preferences object",
        LW_TYPE_PROGRESS,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PROGRESS, _klasspriv->pspec[PROP_PROGRESS]);

    _klasspriv->pspec[PROP_DOWNLOAD_URI] = g_param_spec_string (
        "download-uri",
        "dictionary install name",
        "Set the preferences object",
        "",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DOWNLOAD_URI, _klasspriv->pspec[PROP_DOWNLOAD_URI]);

    _klasspriv->pspec[PROP_INSTALL_PATH] = g_param_spec_string (
        "install-path",
        "dictionary install name",
        "Set the preferences object",
        "",
        G_PARAM_READABLE
    );
    g_object_class_install_property (object_class, PROP_INSTALL_PATH, _klasspriv->pspec[PROP_INSTALL_PATH]);

    _klasspriv->pspec[PROP_DEPENDENCIES] = g_param_spec_pointer (
        "dependencies",
        "Required dictionaries to be install",
        "A list of the prerequisite dictionaries for this to be installed.",
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_DEPENDENCIES, _klasspriv->pspec[PROP_DEPENDENCIES]);
}


void
lw_dictionaryinstall_set_name (LwDictionaryInstall *self,
                               const gchar         *NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (NAME, priv->data.name) == 0)
    {
      goto errored;
    }

    g_free (priv->data.name);
    priv->data.name = g_strdup (NAME);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_NAME]);

    lw_dictionaryinstall_sync_id (self);

errored:

    return;
}


const gchar*
lw_dictionaryinstall_get_name (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.name;
}
                        

void
lw_dictionaryinstall_set_description (LwDictionaryInstall *self,
                                      const gchar         *DESCRIPTION)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (DESCRIPTION, priv->data.description) != 0);

    if (priv->data.description != NULL)
    {
      g_free (priv->data.description);
    }

    priv->data.description = g_strdup (DESCRIPTION);

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DESCRIPTION]);
}


const gchar*
lw_dictionaryinstall_get_description (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.description;
}


void
lw_dictionaryinstall_set_text_encoding (LwDictionaryInstall *self,
                                        const gchar         *TEXT_ENCODING)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (TEXT_ENCODING, priv->data.text_encoding) != 0);

    if (priv->data.text_encoding != NULL)
    {
      g_free (priv->data.text_encoding);
    }

    priv->data.text_encoding = g_strdup (TEXT_ENCODING);

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_TEXT_ENCODING]);
}


const gchar*
lw_dictionaryinstall_get_text_encoding (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.text_encoding;
}

void
lw_dictionaryinstall_set_gtype (LwDictionaryInstall *self,
                                GType                gtype)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (gtype == priv->data.gtype)
    {
      goto errored;
    }
    
    priv->data.gtype = gtype;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_GTYPE]);
    lw_dictionaryinstall_sync_id (self);

errored:

    return;
}


GType
lw_dictionaryinstall_get_gtype (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    
    return priv->data.gtype;
}


void
lw_dictionaryinstall_set_download_key (LwDictionaryInstall *self,
                                       const gchar         *DOWNLOAD_KEY)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (DOWNLOAD_KEY, priv->data.download_key) != 0);
    if (changed == FALSE) goto errored;

    g_free (priv->data.download_key);
    priv->data.download_key = g_strdup (DOWNLOAD_KEY);

    lw_dictionaryinstall_sync_download_uri (self);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DOWNLOAD_KEY]);

errored:

    return;
}


const gchar*
lw_dictionaryinstall_get_download_key (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.download_key;
}


void
lw_dictionaryinstall_set_download_uri (LwDictionaryInstall *self,
                                       const gchar         *DOWNLOAD_URI)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (DOWNLOAD_URI, priv->data.download_uri) != 0);
    if (changed == FALSE) goto errored;

    g_free (priv->data.download_uri);
    priv->data.download_uri = g_strdup (DOWNLOAD_URI);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DOWNLOAD_URI]);

errored:

    return;
}


void
lw_dictionaryinstall_sync_download_uri (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwPreferences *preferences = NULL;
    const gchar *KEY = NULL;
    gchar *uri = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    preferences = lw_dictionaryinstall_get_preferences (self);
    if (preferences == NULL) goto errored;
    KEY = lw_dictionaryinstall_get_download_key (self);
    if (KEY == NULL) goto errored;
    uri = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY);
    changed = (g_strcmp0 (uri, priv->data.download_uri) != 0);
    if (changed == FALSE) goto errored;

    lw_dictionaryinstall_set_download_uri (self, uri);

errored:

    g_free (uri); uri = NULL;
}


const gchar*
lw_dictionaryinstall_get_download_uri (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.download_uri;
}


const gchar*
lw_dictionaryinstall_get_install_path (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.install_path;
}


void
lw_dictionaryinstall_sync_install_path (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwPreferences *preferences = NULL;
    const gchar *KEY = NULL;
    gchar *uri = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    preferences = lw_dictionaryinstall_get_preferences (self);
    if (preferences == NULL) goto errored;
    KEY = lw_dictionaryinstall_get_download_key (self);
    if (KEY == NULL) goto errored;
    uri = lw_preferences_get_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY);
    changed = (g_strcmp0 (uri, priv->data.download_uri) != 0);
    if (changed == FALSE) goto errored;

    lw_dictionaryinstall_set_download_uri (self, uri);

errored:

    g_free (uri); uri = NULL;
}


void
lw_dictionaryinstall_set_preferences (LwDictionaryInstall *self,
                                      LwPreferences       *preferences)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (preferences != priv->data.preferences);
    if (!changed) goto errored;

    if (preferences != NULL)
    {
      g_object_ref (preferences);
    }

    if (priv->data.preferences != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.preferences),
        (gpointer) &(priv->data.preferences)
      );
      g_object_unref (priv->data.preferences);
    }

    priv->data.preferences = preferences;

    if (priv->data.preferences != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.preferences),
        (gpointer*) &(priv->data.preferences)
      );
    }

    lw_dictionaryinstall_sync_download_uri (self);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_PREFERENCES]);

errored:

    return;
}

LwPreferences*
lw_dictionaryinstall_get_preferences (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.preferences;
}


void
lw_dictionaryinstall_set_merge_radicals_into_kanji (LwDictionaryInstall *self,
                                                    gboolean             merge)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (merge != priv->config.merge_radicals_into_kanji);
    if (!changed) goto errored;

    priv->config.merge_radicals_into_kanji = merge;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_MERGE_RADICALS_INTO_KANJI]);

errored:

    return;
}


gboolean
lw_dictionaryinstall_get_merge_radicals_into_kanji (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.merge_radicals_into_kanji;
}


void
lw_dictionaryinstall_set_progress (LwDictionaryInstall *self,
                                   LwProgress          *progress)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (progress != priv->data.progress);
    if (!changed) goto errored;

    if (progress != NULL)
    {
      g_object_ref (progress);
    }

    if (priv->data.progress != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.progress),
        (gpointer*) &(priv->data.progress)
      );
      g_object_unref (priv->data.progress);
    }

    priv->data.progress = progress;

    if (priv->data.progress != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.progress),
        (gpointer*) &(priv->data.progress)
      );
    }

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_PROGRESS]);

errored:

    return;
}


LwProgress*
lw_dictionaryinstall_get_progress (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.progress == NULL)
    {
      LwProgress *progress = lw_progress_new ();
      lw_dictionaryinstall_set_progress (self, progress);
      progress = NULL;
    }

    return priv->data.progress;
}


void
lw_dictionaryinstall_set_split_places_from_names (LwDictionaryInstall *self,
                                                  gboolean             split)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (split != priv->config.split_places_from_names);
    if (!changed) goto errored;

    priv->config.split_places_from_names = split;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_SPLIT_PLACES_FROM_NAMES]);

errored:

    return;
}


gboolean
lw_dictionaryinstall_get_split_places_from_names (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->config.split_places_from_names;
}


GList*
lw_dictionaryinstall_get_dependancies (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.dependancies;
}


void
lw_dictionaryinstall_set_dependancies (LwDictionaryInstall *self,
                                       GList               *dependancies)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    GList *sanitized_list_copy = NULL;

    //Initializations
    priv = self->priv;
    if (dependancies == priv->data.dependancies) goto errored;

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

    g_list_free_full (priv->data.dependancies, (GDestroyNotify) lw_dependancy_free);
    priv->data.dependancies = sanitized_list_copy;

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DEPENDENCIES]);

errored:

    return;
}


void
lw_dictionaryinstall_set_simple_boolean_dependancy (LwDictionaryInstall *self,
                                                    const gchar         *NAME,
                                                    const gchar         *PROPERTY_NAME,
                                                    gboolean             VALUE)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));
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
      lw_dictionaryinstall_set_dependancies (self, dependancies);
      g_list_free_full (dependancies, (GDestroyNotify) lw_dependancy_free);
    }

errored:

    return;
}



const gchar*
lw_dictionaryinstall_get_id (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.id;
}


void
lw_dictionaryinstall_set_id (LwDictionaryInstall *self,
                             const gchar         *ID)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;
    if (g_strcmp0 (priv->data.id, ID) == 0) goto errored;

    g_free (priv->data.id);
    priv->data.id = g_strdup (ID);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_ID]);

errored:

    return;
}


void
lw_dictionaryinstall_sync_id (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), NULL);

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gchar *id = NULL;

    //Initializations
    priv = self->priv;

    if (g_type_is_a (priv->data.gtype, LW_TYPE_DICTIONARY) && priv->data.name)
    {
      id = lw_dictionary_build_id_from_type (priv->data.gtype, priv->data.name);
    }

    lw_dictionaryinstall_set_id (self, id);
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
		priv = self->priv;
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
lw_dictionaryinstall_download (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->data.progress)) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *URI = NULL;
    gchar *filename = NULL;
    gchar *basename = NULL;
    gchar *target = NULL;
    const gchar *STAGE_NAME = "download";

    //Initializations
    priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    URI = lw_dictionaryinstall_get_download_uri (self);
    if (URI == NULL) goto errored;
    filename = _try_get_filename_from_uri (URI, &basename);
    if (basename == NULL) goto errored;
    target = lw_util_build_filename (LW_PATH_CACHE, basename);
    if (target == NULL) goto errored;
    GList *targets = NULL;
    targets = g_list_prepend (targets, target);

    lw_dictionaryinstallstatehistory_add_paths (priv->data.history, STAGE_NAME, targets);

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
    }

errored:

    g_free (filename); filename = NULL;
    g_free (basename); basename = NULL;
    g_free (target); target = NULL;
    g_list_free (targets);

    return (!lw_progress_errored (progress));
}


gboolean 
lw_dictionaryinstall_decompress (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->data.progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    LwDictionaryInstallState *state = NULL;
    const gchar *STAGE_NAME = "decompressed";
    GList *targets = NULL;

    //Initializations
		priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    state = lw_dictionaryinstallstatehistory_current (priv->data.history);

    {
      gint i = 0;
      for (i = 0; state != NULL && i < state->length; i++) 
      {
        LwDictionaryInstallStateFile *file = state->files[i];
        if (file == NULL) continue;

        const gchar *MESSAGE = gettext("Decompressing %s...");
        const gchar *SUFFIX = file->SUFFIX;
        const gchar *BASENAME = file->basename;
        const gchar *SUFFIXLESS = file->suffixless;
        const gchar *SOURCE = file->path;

        if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) continue;

        lw_progress_set_secondary_message_printf (progress, MESSAGE, BASENAME);
        if (g_strcmp0 (SUFFIX, ".gz") || g_strcmp0 (SUFFIX, ".gzip"))
        {
          gchar *target = g_strjoin (".", SUFFIXLESS, STAGE_NAME, NULL);
          lw_io_gunzip_file (SOURCE, target, progress);
          targets = g_list_prepend (targets, target);
        }
        else
        {
          gchar *target = g_strjoin (".", SOURCE, STAGE_NAME, NULL);
          lw_io_copy (SOURCE, target, progress);
          targets = g_list_prepend (targets, target);
        }
      }
      targets = g_list_reverse (targets);
      lw_dictionaryinstallstatehistory_add_paths (priv->data.history, STAGE_NAME, targets);
    }

errored:

    g_list_free_full (targets, (GDestroyNotify) g_free); targets = NULL;

    return (!lw_progress_errored (progress));
}


gboolean 
lw_dictionaryinstall_convert_encoding (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->data.progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *TEXT_ENCODING = NULL;
    LwDictionaryInstallState *state = NULL;
    const gchar *STAGE_NAME = "encoding";
    GList *targets = NULL;

    //Initializations
		priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    TEXT_ENCODING = priv->data.text_encoding;
    if (TEXT_ENCODING == NULL) goto errored;
    state = lw_dictionaryinstallstatehistory_current (priv->data.history);
    if (state == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; state != NULL && i < state->length; i++) 
      {
        LwDictionaryInstallStateFile *file = state->files[i];
        if (file == NULL) continue;

        const gchar *MESSAGE = gettext("Converting encoding to UTF from %s...");
        const gchar *SUFFIX = file->SUFFIX;
        const gchar *BASENAME = file->basename;
        const gchar *SUFFIXLESS = file->suffixless;
        const gchar *SOURCE = file->path;

        if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) continue;

        lw_progress_set_secondary_message_printf (progress, MESSAGE, TEXT_ENCODING);
        
        if (g_strcmp0 (TEXT_ENCODING, "UTF-8") == 0)
        {
          gchar *target = g_strjoin (".", SUFFIXLESS, STAGE_NAME, NULL);
          lw_io_copy (SOURCE, target, progress);
          targets = g_list_prepend (targets, target);
        }
        else
        {
          gchar *target = g_strjoin (".", SUFFIXLESS, STAGE_NAME, NULL);
          lw_io_copy_with_encoding (SOURCE, target, TEXT_ENCODING, "UTF-8", progress);
          targets = g_list_prepend (targets, target);
        }
      }
      targets = g_list_reverse (targets);
      lw_dictionaryinstallstatehistory_add_paths (priv->data.history, STAGE_NAME, targets);
    }

errored:

    g_list_free_full (targets, (GDestroyNotify) g_free); targets = NULL;

    return (!lw_progress_errored (progress));
}


gboolean 
lw_dictionaryinstall_postprocess (LwDictionaryInstall *self)
{
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->data.progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *STAGE_NAME = NULL;
    LwDictionaryInstallState *state = NULL;
    GList *targets = NULL;

    //Initializations
		priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    STAGE_NAME = "postprocess";
    state = lw_dictionaryinstallstatehistory_current (priv->data.history);
    if (state == NULL) goto errored;

    {
      gint i = 0;
      for (i = 0; state != NULL && i < state->length; i++) 
      {
        LwDictionaryInstallStateFile *file = state->files[i];
        if (file == NULL) continue;

        const gchar *MESSAGE = gettext("Postprocessing...");
        const gchar *SUFFIX = file->SUFFIX;
        const gchar *BASENAME = file->basename;
        const gchar *SUFFIXLESS = file->suffixless;
        const gchar *SOURCE = file->path;

        if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) continue;

        lw_progress_set_secondary_message_printf (progress, MESSAGE);

        if (priv->config.split_places_from_names)
        {
          lw_dictionaryinstall_split_places_from_names (self);
        }
        if (priv->config.merge_radicals_into_kanji)
        {
          lw_dictionaryinstall_merge_radicals_into_kanji (self);
        }
      }
      targets = g_list_reverse (targets);
      lw_dictionaryinstallstatehistory_add_paths (priv->data.history, STAGE_NAME, targets);
    }

errored:

    g_list_free_full (targets, (GDestroyNotify) g_free); targets = NULL;

    return (!lw_progress_errored (progress));
}


gboolean 
lw_dictionaryinstall_finish (LwDictionaryInstall *self)
{
  printf("BREAK lw_dictionaryinstall_finish\n");
    //Sanity check
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_should_abort (self->priv->data.progress)) return FALSE;

    //Declarations
		LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    LwDictionaryInstallState *state = NULL;
    const gchar *STAGE_NAME = NULL;
    GList *targets = NULL;

    //Initializations
		priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    state = lw_dictionaryinstallstatehistory_current (priv->data.history);
    if (state == NULL) goto errored;
    STAGE_NAME = "finish";

    {
      gint i = 0;
      for (i = 0; state != NULL && i < state->length; i++) 
      {
        LwDictionaryInstallStateFile *file = state->files[i];
        if (file == NULL) continue;

        const gchar *MESSAGE = gettext("Finalizing...");
        const gchar *SUFFIX = file->SUFFIX;
        const gchar *BASENAME = file->basename;
        const gchar *SUFFIXLESS = file->suffixless;
        const gchar *SOURCE = file->path;

        if (!g_file_test (SOURCE, G_FILE_TEST_IS_REGULAR)) continue;

        lw_progress_set_secondary_message (progress, MESSAGE);

        gchar *target = g_strdup (lw_dictionaryinstall_get_install_path (self));
        lw_io_copy (SOURCE, target, progress);
        targets = g_list_prepend (targets, target);
      }
      targets = g_list_reverse (targets);
      lw_dictionaryinstallstatehistory_add_paths (priv->data.history, STAGE_NAME, targets);
    }

errored:

    g_list_free_full (targets, (GDestroyNotify) g_free); targets = NULL;

    return (!lw_progress_errored (progress));
}


void 
lw_dictionary_installer_clean (LwDictionary *self, 
                               LwProgress   *progress)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = self->priv;
    install = priv->install;

    if (install->filelist != NULL) g_strfreev (install->filelist); install->filelist = NULL;
    if (install->downloadlist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->decompresslist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->encodelist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->postprocesslist != NULL) g_strfreev (install->postprocesslist); install->postprocesslist = NULL;
    if (install->installlist != NULL) g_strfreev (install->installlist); install->installlist = NULL;
    if (install->installedlist != NULL) g_strfreev (install->installedlist); install->installedlist = NULL;
    */
}


gdouble 
lw_dictionary_installer_get_stage_progress (LwDictionary *self)
{
  /*TODO
    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstallerStatus status;
    gint index;
    gdouble current;
    gdouble final;
    gchar **list;
    gdouble fraction;
    
    //Initializations
		priv = self->priv;
    fraction = priv->progress;
    current = final = 0.0;

    //Get the current progress
    index = lw_dictionary_installer_get_file_index (self);

    switch (status)
    {
      case LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING:
        list = lw_dictionary_installer_get_downloadlist (self);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_DECOMPRESSING:
        list = lw_dictionary_installer_get_encodelist (self);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_ENCODING:
        list = lw_dictionary_installer_get_postprocesslist (self);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING:
        list = lw_dictionary_installer_get_installlist (self);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_FINISHING:
        list = lw_dictionary_installer_get_installedlist (self);
        break;
      default:
        list = NULL;
        break;
    }

    final = (gdouble) g_strv_length (list);
    current = (gdouble) index + fraction;
    if (final == 0.0)
      fraction = 0.0;
    else
      fraction = current / final;

    return fraction;
    */
    return 0.0;
}


//!
//! @brief Gets the total possible progress for the LwDictionary.  It should be
//!        divided by the current progress to get the appropriate fraction
//! @param self The LwDictInfo object to get the total progress of
//! @param fraction The fraction progress of the current process.
//!
gdouble 
lw_dictionary_installer_get_total_progress (LwDictionary *self)
{
  /*TODO
    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstallerStatus status;
    gdouble current;
    gdouble final;
    gdouble fraction;
    gdouble temp;
    gint i;
    gchar **list;

    //Definitions
		priv = self->priv;
    status = lw_dictionary_installer_get_status (self);
    current = final = 0.0;
    fraction = priv->progress;
    const gdouble DOWNLOAD_WEIGHT = 3.0;

    for (i = 0; i < TOTAL_LW_DICTIONARY_INSTALLER_STATUSES; i++)
    {
      switch (i)
      {
        case LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING:
          list = lw_dictionary_installer_get_downloadlist (self);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_DECOMPRESSING:
          list = lw_dictionary_installer_get_encodelist (self);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_ENCODING:
          list = lw_dictionary_installer_get_postprocesslist (self);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING:
          list = lw_dictionary_installer_get_installlist (self);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_FINISHING:
          list = lw_dictionary_installer_get_installedlist (self);
          break;
        default:
          break;
      }
      if (i == LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING)
        temp = (gdouble) g_strv_length (list) * DOWNLOAD_WEIGHT;
      else
        temp= (gdouble) g_strv_length (list);

      final += temp;
      if (i < status) current += temp;
    }

    temp = lw_dictionary_installer_get_stage_progress (self);
    if (i == LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING)
      current += temp * DOWNLOAD_WEIGHT;
    else
      current += temp;

    if (final == 0.0)
      fraction = 0.0;
    else
      fraction = current / final;

    return fraction;
    */
    return 0.0;
}


/*TODO
gint
lw_dictionary_installer_get_file_index (LwDictionary *self)
{
    //Sanity check
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = self->priv;
    install = priv->install;

    return install->index;
}
*/


gboolean 
lw_dictionaryinstall_install (LwDictionaryInstall *self)
{
  printf("BREAK lw_dictionaryinstall_install\n");
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    if (lw_progress_errored (self->priv->data.progress)) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    LwProgress *progress = NULL;
    const gchar *NAME = NULL;
    const gchar *MESSAGE = NULL;

    //Initializations
    priv = self->priv;
    progress = lw_dictionaryinstall_get_progress (self);
    NAME = lw_dictionaryinstall_get_name (self);
    MESSAGE = gettext("Installing %s Dictionary...");

    lw_progress_set_primary_message_printf (progress, MESSAGE, NAME);

    lw_dictionaryinstall_download (self);
    lw_dictionaryinstall_decompress (self);
    lw_dictionaryinstall_convert_encoding (self);
    lw_dictionaryinstall_postprocess (self);
    /*
    lw_dictionary_installer_install (self);
    lw_dictionary_installer_clean (self);
    */

errored:
  
    return (!lw_progress_errored (progress));
}


gboolean
lw_dictionaryinstall_split_places_from_names (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    g_return_val_if_fail (self->priv->data.gtype == LW_TYPE_EDICTIONARY, FALSE);
    if (self->priv->config.split_places_from_names == FALSE) return FALSE;
    if (lw_progress_errored (self->priv->data.progress)) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_warning ("This function is a stub");
    //return lw_io_split_places_from_names_dictionary (targetlist[0], targetlist[1], sourcelist[0], progress);
    return FALSE;
}


gboolean
lw_dictionaryinstall_merge_radicals_into_kanji (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARYINSTALL (self), FALSE);
    g_return_val_if_fail (self->priv->data.gtype == LW_TYPE_KANJIDICTIONARY, FALSE);
    if (self->priv->config.merge_radicals_into_kanji == FALSE) return FALSE;

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    g_warning ("This function is a stub");

    return FALSE;
    //return lw_io_create_mix_dictionary (targetlist[0], sourcelist[0], sourcelist[1], progress);
}




