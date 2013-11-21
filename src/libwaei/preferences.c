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
//! @file preferences.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <gio/gio.h>

#include <libwaei/libwaei.h>
#include <libwaei/gettext.h>
#include <libwaei/preferences-private.h>

static LwPreferences *_preferences = NULL;

G_DEFINE_ABSTRACT_TYPE (LwPreferences, lw_preferences, G_TYPE_OBJECT)

typedef enum
{
  PROP_0,
  PROP_BACKEND,
} LwPreferencesProps;


//!
//! @brief Creates a new LwDictInfo object
//! @return An allocated LwDictInfo that will be needed to be freed by lw_preferences_free.
//!
LwPreferences* 
lw_preferences_new (GSettingsBackend *backend)
{
  LwPreferences *preferences;

  preferences = LW_PREFERENCES (g_object_new (LW_TYPE_PREFERENCES, "backend", backend, NULL));

  return preferences;
}


LwPreferences*
lw_preferences_get_default ()
{
    if (_preferences == NULL)
    {
      LwPreferences *preferences = lw_preferences_new (NULL);
      g_object_add_weak_pointer (G_OBJECT (preferences), (gpointer*) &_preferences);
    }
    else
    {
        g_object_ref (_preferences);
    }

    return _preferences;
}


static void 
lw_preferences_init (LwPreferences *preferences)
{
    preferences->priv = LW_PREFERENCES_GET_PRIVATE (preferences);
    memset(preferences->priv, 0, sizeof(LwPreferencesPrivate));
}


static void 
lw_preferences_finalize (GObject *object)
{
    //Declarations
    LwPreferences *preferences = NULL;
    LwPreferencesPrivate *priv = NULL;

    //Initalizations
    preferences = LW_PREFERENCES (object);
    priv = preferences->priv;

    {
      GList *iter = NULL;
      GSettings *settings = NULL;
      for (iter = priv->settingslist; iter != NULL; iter = iter->next)
      {
        settings = (GSettings*) iter->data;
        g_object_unref (settings);
        iter->data = NULL;
      }
        g_list_free (priv->settingslist);
        priv->settingslist = NULL;
    }

    if (priv->backend != NULL)
    {
      g_object_unref (priv->backend);
      priv->backend = NULL;
    }

    memset(preferences->priv, 0, sizeof(LwPreferencesPrivate));

    G_OBJECT_CLASS (lw_preferences_parent_class)->finalize (object);
}


static void 
lw_preferences_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    //Declarations
    LwPreferences *preferences;
    LwPreferencesPrivate *priv;

    //Initializations
    preferences = LW_PREFERENCES (object);
    priv = preferences->priv;

    switch (property_id)
    {
      case PROP_BACKEND:
        if (priv->backend != NULL) g_object_unref (priv->backend);
        priv->backend = g_value_dup_object (value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
lw_preferences_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
    //Declarations
    LwPreferences *preferences = NULL;
    LwPreferencesPrivate *priv = NULL;

    //Initializations
    preferences = LW_PREFERENCES (object);
    priv = preferences->priv;

    switch (property_id)
    {
      case PROP_BACKEND:
        g_value_set_object (value, priv->backend);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lw_preferences_class_init (LwPreferencesClass *klass)
{
    //Declarations
    GParamSpec *pspec = NULL;
    GObjectClass *object_class = NULL;
    LwPreferencesClass *preferences_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lw_preferences_set_property;
    object_class->get_property = lw_preferences_get_property;
    object_class->finalize = lw_preferences_finalize;

    preferences_class = LW_PREFERENCES_CLASS (klass);

    g_type_class_add_private (object_class, sizeof (LwPreferencesPrivate));

    pspec = g_param_spec_object ("backend",
                                 "The backend used by GSettings",
                                 "Set the preferences's backend",
                                 G_TYPE_SETTINGS_BACKEND,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_BACKEND, pspec);
}
 

gboolean 
lw_preferences_schema_is_installed (const char *SCHEMA)
{
    //Declarations
    const gchar * const * iter;
    gboolean exists;

    //Initializations
    iter = g_settings_list_schemas ();

    while (iter != NULL && *iter != NULL && strcmp(*iter, SCHEMA) != 0)
      iter++;

    exists = (iter != NULL && *iter != NULL);

    if (!exists)
    {
      g_critical ("The GSettings schema \"%s\" isn't installed!  You must make "
                  "sure both gsettings-desktop-schemas from your package "
                  "manager and org.gnome.gwaei.gschema.xml are installed at "
                  "least locally if not globally. See the man page for "
                  "glib-compile-schemas for more information.\n", SCHEMA);
    }

    return exists;
}


//!
//! @brief Fetches a gsettings object by id, and stores it, using the cached one if available
//!
GSettings* 
lw_preferences_get_settings_object (LwPreferences *preferences, const char *SCHEMA)
{
    //Sanity checks
    g_return_val_if_fail (preferences != NULL, NULL);
    g_return_val_if_fail (SCHEMA != NULL, NULL);

    //Declarations
    LwPreferencesPrivate *priv = NULL;
    char *schema = NULL;
    GSettings *settings = NULL;

    //Initializations
    priv = preferences->priv;

    //Look for an already created gsetting object
    {
      GList *iter = NULL;
      for (iter = priv->settingslist; iter != NULL; iter = iter->next)
      {
        settings = (GSettings*) iter->data;
        g_object_get (G_OBJECT (settings), "schema", &schema, NULL);
        if (schema != NULL && strcmp(schema, SCHEMA) == 0)
          break;
        if (schema != NULL)
          g_free (schema);
        settings = NULL;
      }
      if (settings != NULL) 
      {
        g_free (schema);
      }
      else
      {
      }
    }

    //If not found, create our own and add it to the list
    if (settings == NULL)
    {
      g_assert (lw_preferences_schema_is_installed (SCHEMA));

      if (priv->backend == NULL)
        settings = g_settings_new (SCHEMA);
      else
        settings = g_settings_new_with_backend (SCHEMA, priv->backend);
      if (settings != NULL)
      {
        priv->settingslist = g_list_append (priv->settingslist, settings);
      }
    }

    return settings;
}


//!
//! @brief Resets a value in a key
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key A string identifying the key to reset
//!
void 
lw_preferences_reset_value (GSettings* settings, const char *key)
{
    g_settings_reset (settings, key);
}


//!
//! @brief Resets a value in a key
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema A string identifying the schema there the key is
//! @param key A string identifying the key to reset
//!
void 
lw_preferences_reset_value_by_schema (LwPreferences* preferences, const char* schema, const char *key)
{
    GSettings* settings;
    
    settings = lw_preferences_get_settings_object (preferences, schema);
    if (settings != NULL)
    {
      lw_preferences_reset_value (settings, key);
    }
}


//!
//! @brief Returns an integer from the preference backend 
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//!
int 
lw_preferences_get_int (GSettings *settings, const char *key)
{
    return g_settings_get_int (settings, key);
}


//!
//! @brief Returns an integer from the preference backend 
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The key to use to look up the pref
//! @param key The key to use to look up the pref
//!
int 
lw_preferences_get_int_by_schema (LwPreferences* preferences, const char* schema, const char *key)
{
    GSettings* settings;
    int value;
    
    value = 0;
    settings = lw_preferences_get_settings_object (preferences, schema);

    if (settings != NULL)
    {
      value = lw_preferences_get_int (settings, key);
    }

    return value;
}


//!
//! @brief Sets the int to the key in the preferences backend
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//! @param request The value to set
//!
void 
lw_preferences_set_int (GSettings *settings, const char *key, const int request)
{
    g_settings_set_int (settings, key, request);
}


//!
//! @brief Sets the int to the key in the preferences backend
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The schema to look for
//! @param key The key to use to look up the pref inside of the schema
//! @param request The value to set
//!
void 
lw_preferences_set_int_by_schema (LwPreferences* preferences, const char* schema, const char *key, const int request)
{
    GSettings* settings;
    
    settings = lw_preferences_get_settings_object (preferences, schema);
    if (settings != NULL)
    {
      lw_preferences_set_int (settings, key, request);
    }
}


//!
//! @brief Returns an boolean from the preference backend 
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//!
gboolean 
lw_preferences_get_boolean (GSettings *settings, const char *key)
{
    return g_settings_get_boolean (settings, key);
}


//!
//! @brief Returns an boolean from the preference backend 
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The key to use to look up the pref
//! @param key The key to use to look up the pref
//!
gboolean 
lw_preferences_get_boolean_by_schema (LwPreferences* preferences, const char* schema, const char *key)
{
    GSettings* settings;
    gboolean value;
    
    settings = lw_preferences_get_settings_object (preferences, schema);
    value = FALSE; 

    if (settings != NULL)
    {
      value = lw_preferences_get_boolean (settings, key);
    }

    return value;
}


//!
//! @brief Sets the boolean to the key in the preferences backend
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//! @param request The value to set
//!
void 
lw_preferences_set_boolean (GSettings *settings, const char *key, const gboolean request)
{
    g_settings_set_boolean (settings, key, request);
}


//!
//! @brief Sets the boolean to the key in the preferences backend
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The schema to look for
//! @param key The key to use to look up the pref inside of the schema
//! @param request The value to set
//!
void 
lw_preferences_set_boolean_by_schema (LwPreferences* preferences, const char* schema, const char *key, const gboolean request)
{
    GSettings* settings;
    
    settings = lw_preferences_get_settings_object (preferences, schema);

    if (settings != NULL)
    {
      lw_preferences_set_boolean (settings, key, request);
    }
}


//!
//! @brief Returns an string from the preference backend 
//! @param output string to copy the pref to
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//! @param n The max characters to copy to output
//!
void 
lw_preferences_get_string (char *output, GSettings *settings, const char *key, const int n)
{
    gchar *value = NULL; 

    value = g_settings_get_string (settings, key);
    g_assert (value != NULL);
    strncpy(output, value, n);
    output[n - 1] = '\0';

    g_free (value);
    value = NULL;
}


//!
//! @brief Returns an string from the preference backend 
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param output string to copy the pref to
//! @param schema The key to use to look up the pref
//! @param key The key to use to look up the pref
//! @param n The max characters to copy to output
//!
void 
lw_preferences_get_string_by_schema (LwPreferences* preferences, char *output, const char *schema, const char *key, const int n)
{
    GSettings* settings;
    
    settings = lw_preferences_get_settings_object (preferences, schema);

    lw_preferences_get_string (output, settings, key, n);
}


//!
//! @brief Sets the string to the key in the preferences backend
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The key to use to look up the pref
//! @param request The value to set
//!
void 
lw_preferences_set_string (GSettings *settings, const char *key, const char* request)
{
    g_settings_set_string (settings, key, request);
}


//!
//! @brief Sets the string to the key in the preferences backend
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The schema to look for
//! @param key The key to use to look up the pref inside of the schema
//! @param request The value to set
//!
void 
lw_preferences_set_string_by_schema (LwPreferences *preferences, const char* schema, const char *key, const char* request)
{
    GSettings *settings;

    settings = lw_preferences_get_settings_object (preferences, schema);

    if (settings != NULL)
    {
      lw_preferences_set_string (settings, key, request);
    }
}


//!
//! @brief Adds a preference change listener for the selected key
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param key The preference key
//! @param callback_function The function to call when the key changes
//! @param data The userdata to pass to the callback function
//! @returns A gulong used to remove a signal later if desired
//!
gulong 
lw_preferences_add_change_listener (GSettings *settings, const char *key, void (*callback_function) (GSettings*, gchar*, gpointer), gpointer data)
{
    g_assert (key != NULL);

    //Declarations
    char *signal_name;
    gulong id;
    GVariant *value;

    //Set up the signal
    signal_name = g_strdup_printf ("changed::%s", key);
    id = g_signal_connect (G_OBJECT (settings), signal_name, G_CALLBACK (callback_function), data);

    //Trigger an initial fire of the change listener
    value = g_settings_get_value (settings, key);
    if (value != NULL) g_settings_set_value (settings, key, value);

    //Cleanup
    g_variant_unref (value);
    value = NULL;
    g_free (signal_name);
    signal_name = NULL;

    return id;
}


//!
//! @brief Adds a preference change listener for the selected key
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema The key to use to look up the pref
//! @param key The preference key
//! @param callback_function The function to call when the key changes
//! @param data The userdata to pass to the callback function
//! @returns A gulong used to remove a signal later if desired
//!
gulong 
lw_preferences_add_change_listener_by_schema (LwPreferences *preferences, const char* schema, const char *key, void (*callback_function) (GSettings*, gchar*, gpointer), gpointer data)
{
    g_assert (schema != NULL && key != NULL);

    GSettings *settings;
    gulong id;

    settings = lw_preferences_get_settings_object (preferences, schema);
    id = lw_preferences_add_change_listener (settings, key, callback_function, data);


    return id;
}


//!
//! @brief Used to remove a listener
//! @param settings The GSettings object to act on You will have to get it yourself using lw_preferences_get_settings_object
//! @param id The signalid returned by lw_preferences_add_change_listener
//!
void 
lw_preferences_remove_change_listener (GSettings *settings, gulong id)
{
    if (g_signal_handler_is_connected (G_OBJECT (settings), id))
    {
      g_signal_handler_disconnect (G_OBJECT (settings), id);
    }
    else
    {
    }
}


//!
//! @brief Used to remove a listener
//! @param preferences The LwPreferences to fetch the GSettings object matching the schema
//! @param schema A schema of the GSettings object the signal was connected to
//! @param id The signalid returned by lw_preferences_add_change_listener
//!
void 
lw_preferences_remove_change_listener_by_schema (LwPreferences *preferences, const char* schema, gulong id)
{
    GSettings *settings;

    settings = lw_preferences_get_settings_object (preferences, schema);

    if (settings != NULL)
    {
      lw_preferences_remove_change_listener (settings, id);
    }
    else
    {
    }
}






