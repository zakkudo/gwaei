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
//! @file application.c
//!
//! @brief To be written
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include <waei/gettext.h>
#include <waei/waei.h>
#include <waei/application-private.h>

G_DEFINE_TYPE (WApplication, w_application, G_TYPE_APPLICATION)

//!
//! @brief creates a new instance of the gwaei applicaiton
//!
GApplication* 
w_application_new (GApplicationFlags flags)
{
    //Declarations
    WApplication *self = NULL;
    const gchar *id = "glib.org.waei";
    //GApplicationFlags flags = G_APPLICATION_HANDLES_COMMAND_LINE | G_APPLICATION_IS_SERVICE;
    //GApplicationFlags flags = G_APPLICATION_HANDLES_COMMAND_LINE | G_APPLICATION_IS_LAUNCHER;
    flags |= G_APPLICATION_HANDLES_COMMAND_LINE;

    //Initializations
    self = g_object_new (
      W_TYPE_APPLICATION,
      "application-id", id,
      "flags", flags,
      NULL
    );
    g_application_set_inactivity_timeout (G_APPLICATION (self), 60 * 100);

    return G_APPLICATION (self);
}


static void 
w_application_init (WApplication *self)
{
    self->priv = W_APPLICATION_GET_PRIVATE (self);
    memset(self->priv, 0, sizeof(WApplicationPrivate));
}


static void 
w_application_constructed (GObject *object)
{
    //Chain the parent class
    {
      G_OBJECT_CLASS (w_application_parent_class)->constructed (object);
    }

    //Declarations
    WApplication *self = NULL;
    WApplicationPrivate *priv = NULL;

    //Initializations
    self = W_APPLICATION (object);
    priv = self->priv;

    lw_regex_initialize ();
}


static void 
w_application_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    //Declarations
    WApplication *self = NULL;
    WApplicationPrivate *priv = NULL;

    //Initializations
    self = W_APPLICATION (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_MORPHOLOGYENGINE:
        w_application_set_morphologyengine (self, g_value_get_object (value));
        break;
      case PROP_PREFERENCES:
        w_application_set_preferences (self, g_value_get_object (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void 
w_application_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    //Declarations
    WApplication *self = NULL;
    WApplicationPrivate *priv = NULL;

    //Initializations
    self = W_APPLICATION (object);
    priv = self->priv;

    switch (property_id)
    {
      case PROP_MORPHOLOGYENGINE:
        g_value_set_object (value, w_application_get_morphologyengine (self));
        break;
      case PROP_PREFERENCES:
        g_value_set_object (value, w_application_get_preferences (self));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
w_application_dispose (GObject *object)
{
    //Declarations
    WApplication *self = NULL;

    //Initializations
    self = W_APPLICATION (object);

    G_OBJECT_CLASS (w_application_parent_class)->dispose (object);
}



static void 
w_application_finalize (GObject *object)
{
    //Declarations
    WApplication *self = NULL;
    WApplicationPrivate *priv = NULL;

    //Initializations
    self = W_APPLICATION (object);
    priv = self->priv;

    if (priv->data.dictionarylist != NULL) g_object_unref (priv->data.dictionarylist); priv->data.dictionarylist = NULL;
    if (priv->data.dictionaryinstalllist != NULL) g_object_unref (priv->data.dictionaryinstalllist); priv->data.dictionaryinstalllist = NULL;

    w_application_set_preferences (self, NULL);

    lw_regex_free ();

    G_OBJECT_CLASS (w_application_parent_class)->finalize (object);
}


static int 
w_application_command_line (GApplication            *application,
                            GApplicationCommandLine *command_line)
{
    //Sanity checks
    g_return_val_if_fail (G_IS_APPLICATION (application), 1);

    printf("BREAK global w_application_command_line\n");

    //Declarations
    WApplication *self = NULL;
    WApplicationPrivate *priv = NULL;
    gchar **argv = NULL;
    gint argc = 0;
    gint resolution = -1;

    //Initializations
    self = W_APPLICATION (application);
    priv = self->priv;
    argv = g_application_command_line_get_arguments (command_line, &argc);

    g_application_hold (application);

    if (g_application_command_line_get_is_remote (command_line))
    {
      printf("BREAK is remote\n");
      WCommand *command = w_command_new (self, command_line);
      resolution = w_command_run (command);
      g_object_unref (command); command = NULL;
    }
    else
    {
      resolution = 0;
    }

    g_application_release (application);

    if (!g_application_command_line_get_is_remote (command_line))
    {
    }

    

    return resolution;
}


static void
w_application_activate (GApplication *self)
{
}


gboolean
w_application_local_command_line (GApplication              *application,
                                  gchar                   ***arguments,
                                  int                       *exit_status)
{
  printf("BREAK local_command_line\n");
    return FALSE;
}


static void
w_application_startup (GApplication *self)
{
    G_APPLICATION_CLASS (w_application_parent_class)->startup (self);
}


static void
w_application_class_init (WApplicationClass *klass)
{
    GObjectClass *object_class = NULL;
    GApplicationClass *application_class = NULL;

    object_class = G_OBJECT_CLASS (klass);
    klass->priv = g_new0 (WApplicationClassPrivate, 1);
    application_class = G_APPLICATION_CLASS (klass);

    object_class->constructed = w_application_constructed;
    object_class->set_property = w_application_set_property;
    object_class->get_property = w_application_get_property;
    application_class->command_line = w_application_command_line;
    application_class->local_command_line = w_application_local_command_line;
    application_class->startup = w_application_startup;
    application_class->activate = w_application_activate;
    object_class->dispose = w_application_dispose;
    object_class->finalize = w_application_finalize;

    g_type_class_add_private (object_class, sizeof (WApplicationPrivate));

    WApplicationClassPrivate *klasspriv = klass->priv;

    klasspriv->pspec[PROP_PREFERENCES] = g_param_spec_object (
        "preferences",
        "FIlename construct prop",
        "Set the filename",
        LW_TYPE_PREFERENCES,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_PREFERENCES, klasspriv->pspec[PROP_PREFERENCES]);

    klasspriv->pspec[PROP_MORPHOLOGYENGINE] = g_param_spec_object (
        "morphology-engine",
        "FIlename construct prop",
        "Set the filename",
        LW_TYPE_MORPHOLOGYENGINE,
        G_PARAM_CONSTRUCT | G_PARAM_READWRITE
    );
    g_object_class_install_property (object_class, PROP_MORPHOLOGYENGINE, klasspriv->pspec[PROP_MORPHOLOGYENGINE]);
}


//!
//! @brief Prints to the terminal the about message for the program.
//!
void 
w_application_print_about (WApplication *self)
{
    const gchar *name;
    name = w_application_get_program_name (W_APPLICATION (self));

    printf (gettext ("%s version %s"), name, VERSION);

    printf ("\n\n");

    printf ("Check for the latest updates at <http://gwaei.sourceforge.net/>\n");
    printf ("Code Copyright (C) 2009-2012 Zachary Dovel\n\n");

    printf ("License:\n");
    printf ("Copyright (C) 2008 Free Software Foundation, Inc.\nLicense GPLv3+: "
            "GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\nThis"
            " is free software: you are free to change and redistribute it.\nThe"
            "re is NO WARRANTY, to the extent permitted by law.\n\n"             );
}


//!
//! @brief Returns the program name.  It should not be freed or modified
//! @returns A constanst string representing the program name
//!
const char*
w_application_get_program_name (WApplication *self) 
{
  return gettext("Waei Japanese-English Dictionary");
}


void
w_application_set_morphologyengine (WApplication       *self,
                                    LwMorphologyEngine *morphologyengine)
{
  printf("BREAK w_application_set_morphologyengine %d\n", morphologyengine);
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);

    //Declaration
    WApplicationPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (morphologyengine != NULL)
    {
      g_object_ref (morphologyengine);
    }

    if (priv->data.morphologyengine != NULL)
    {
      g_object_remove_weak_pointer (
        G_OBJECT (priv->data.morphologyengine),
        (gpointer*) &priv->data.morphologyengine
      );
    }

    priv->data.morphologyengine = morphologyengine;

    if (priv->data.morphologyengine != NULL)
    {
      g_object_add_weak_pointer (
        G_OBJECT (priv->data.morphologyengine),
        (gpointer*) &priv->data.morphologyengine
      );
    }
}


LwMorphologyEngine*
w_application_get_morphologyengine (WApplication *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);

    //Declaration
    WApplicationPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.morphologyengine == NULL)
    {
      w_application_set_morphologyengine (self, lw_morphologyengine_new ("en_US"));
    }

    return priv->data.morphologyengine;
}


LwDictionaryList* 
w_application_get_dictionarylist (WApplication *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);

printf("BREAK w_application_get_dictionarylist\n");

    //Declarations
    WApplicationPrivate *priv = NULL;
    LwMorphologyEngine *morphologyengine = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    morphologyengine = w_application_get_morphologyengine (self);
    preferences = w_application_get_preferences (self);

    if (priv->data.dictionarylist == NULL)
    {
      priv->data.dictionarylist = lw_dictionarylist_new (preferences);
      lw_dictionarylist_load_installed (priv->data.dictionarylist, morphologyengine);
      lw_dictionarylist_load_order (priv->data.dictionarylist);
    }

    return priv->data.dictionarylist;
}


LwDictionaryInstallList* 
w_application_get_dictionaryinstalllist (WApplication *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);

    //Declarations
    WApplicationPrivate *priv = NULL;
    LwPreferences *preferences = NULL;

    //Initializations
    priv = self->priv;
    preferences = w_application_get_preferences (self);

    if (priv->data.dictionaryinstalllist == NULL)
    {
      priv->data.dictionaryinstalllist = lw_dictionaryinstalllist_new (preferences);
      lw_dictionaryinstalllist_load_default (priv->data.dictionaryinstalllist);
    }

    return priv->data.dictionaryinstalllist;
}


LwPreferences*
w_application_get_preferences (WApplication *self)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);

    //Declarations
    WApplicationPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    if (priv->data.preferences == NULL)
    {
      g_io_extension_point_register ("gsettings-backend");
      w_application_set_preferences (self, lw_preferences_new (g_memory_settings_backend_new ()));
    }

    return priv->data.preferences;
}


void
w_application_set_preferences (WApplication *self,
                               LwPreferences *preferences)
{
    //Sanity checks
    g_return_if_fail (W_IS_APPLICATION (self));

    //Declarations
    WApplicationPrivate *priv = NULL;
    WApplicationClass *klass = NULL;
    WApplicationClassPrivate *klasspriv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    klass = W_APPLICATION_CLASS (self);
    klasspriv = klass->priv;
    changed = (preferences != priv->data.preferences);

    if (priv->data.preferences != NULL)
    {
      g_object_remove_weak_pointer (G_OBJECT (priv->data.preferences), (gpointer*) &(priv->data.preferences));
      g_object_unref (priv->data.preferences);
      priv->data.preferences = NULL;
    }

    priv->data.preferences = preferences;

    if (priv->data.preferences != NULL) {
      g_object_ref (priv->data.preferences);
      g_object_add_weak_pointer (G_OBJECT (priv->data.preferences), (gpointer*) &(priv->data.preferences));
    }

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PREFERENCES]);
}


LwDictionary*
w_application_fuzzy_find_dictionary (WApplication *self,
                                     const gchar  *FUZZY_DESCRIPTION)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);
    g_return_val_if_fail (FUZZY_DESCRIPTION != NULL, NULL);

    //Declarations
    LwDictionaryList *dictionary_list = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    dictionary_list = w_application_get_dictionarylist (self);
    if (dictionary_list == NULL) goto errored;
    dictionary = lw_dictionarylist_fuzzy_find (dictionary_list, FUZZY_DESCRIPTION);
    if (dictionary == NULL) goto errored;

errored:

    return dictionary;
}


LwDictionaryInstall*
w_application_fuzzy_find_dictionaryinstall (WApplication *self,
                                            const gchar  *FUZZY_DESCRIPTION)
{
    //Sanity checks
    g_return_val_if_fail (W_IS_APPLICATION (self), NULL);
    g_return_val_if_fail (FUZZY_DESCRIPTION != NULL, NULL);

    //Declarations
    LwDictionaryInstallList *dictionary_install_list = NULL;
    LwDictionaryInstall *dictionaryinstall = NULL;

    //Initializations
    dictionary_install_list = w_application_get_dictionaryinstalllist (self);
    if (dictionary_install_list == NULL) goto errored;
    dictionaryinstall = lw_dictionaryinstalllist_fuzzy_find (dictionary_install_list, FUZZY_DESCRIPTION);
    if (dictionaryinstall == NULL) goto errored;

errored:

    return dictionaryinstall;
}



