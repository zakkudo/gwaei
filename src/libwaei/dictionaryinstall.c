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
      case PROP_DOWNLOAD_PREFERENCE_KEY:
        lw_dictionaryinstall_set_download_key (self, g_value_get_string (value));
        break;
      case PROP_PREFERENCES:
        lw_dictionaryinstall_set_preferences (self, g_value_get_object (value));
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
      case PROP_NAME:
        g_value_set_string (value,lw_dictionaryinstall_get_name (self));
        break;
      case PROP_DESCRIPTION:
        g_value_set_string (value,lw_dictionaryinstall_get_description (self));
        break;
      case PROP_TEXT_ENCODING:
        g_value_set_string (value, lw_dictionaryinstall_get_text_encoding (self));
      case PROP_DEPENDENCIES:
        g_value_set_pointer (value, lw_dictionaryinstall_get_dependencies (self));
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
}



/*

gchar**
lw_dictionary_installer_get_filelist (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **filelist;

    //Initializations
    priv = self->priv;
    install = priv->install;

    if (install->filelist == NULL)
    {
      filelist = g_strsplit (install->files, ";", -1);
    } 

    return filelist;
}


//!
//! @brief Updates the encoding of the LwDictionary
//! @param self The LwDictInfo object to set the ENCODING to
//! @param ENCODING Tells the LwDictInfo object what the initial character encoding of the downloaded file will be
//!
void 
lw_dictionary_installer_set_text_encoding (LwDictionary *self, const LwEncoding ENCODING)
{
		LwDictionaryPrivate *priv;

		priv = self->priv;
    priv->install->encoding = ENCODING;
}


//!
//! @brief Updates the split state of the LwDictionary
//! @param self The LwDictInfo objcet to set the POSTPROCESS variable on
//! @param POSTPROCESS The split setting to copy to the LwDictionary.
//!
void 
lw_dictionary_installer_set_postprocess (LwDictionary *self, const gboolean POSTPROCESS)
{
		LwDictionaryPrivate *priv;

		priv = self->priv;
    priv->install->postprocess = POSTPROCESS;
}
*/


static gchar**
lw_dictionary_installer_get_downloadlist (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);
    g_return_val_if_fail (self->priv->install != NULL, NULL);
    g_return_val_if_fail (self->priv->install->downloads != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryInstall *install = NULL;

    //Initalizations
    priv = self->priv;
    install = priv->install;

    if (install->downloadlist == NULL)
    {
      install->downloadlist = g_strsplit (install->downloads, ";", -1);
    }

    return install->downloadlist;
    */
  return NULL;
}


static gchar**
lw_dictionary_installer_get_decompresslist (LwDictionary *self)
{
  /*
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *filename;
    gchar *path;

    //Initializations
    priv = self->priv;
    install = priv->install;
    templist = NULL;

    if (install->decompresslist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_downloadlist (self));
      if (templist == NULL) goto errored;

      while (*tempiter != NULL)
      {
        filename = strrchr(*tempiter, '/');

        if (filename == NULL || *(filename + 1) == '\0') goto errored;
        filename++;

        path = lw_util_build_filename (LW_PATH_CACHE, filename);
        if (path == NULL) goto errored;
        g_free(*tempiter); *tempiter = path; path = NULL;

        tempiter++;
      }

      install->decompresslist = templist; templist = NULL;
    }

    return install->decompresslist;

errored:
    if (templist != NULL) g_strfreev (templist); templist = NULL;
    */
    return NULL;
}


static gchar**
lw_dictionary_installer_get_encodelist (LwDictionary *self)
{
  /*
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    const gchar* encodingname;
    gchar *extension;
    gchar *path;

    //Initializations
    priv = self->priv;
    install = priv->install;
    templist = NULL;

    if (install->encodelist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_decompresslist (self));
      if (templist == NULL) goto errored;
      encodingname = lw_util_get_encodingname (install->encoding);

      while (*tempiter != NULL)
      {
        extension = strrchr(*tempiter, '.');
        if (extension == NULL) goto errored;

        *extension = '\0';
        path = g_strjoin (".", *tempiter, encodingname, NULL);
        if (path == NULL) goto errored;
        g_free(*tempiter); *tempiter = path; path = NULL;

        tempiter++;
      }

      install->encodelist = templist; templist = NULL;
    }

    return install->encodelist;

errored:
    if (templist != NULL) g_strfreev (templist); templist = NULL;
    */
    return NULL;
}


static gchar**
lw_dictionary_installer_get_postprocesslist (LwDictionary *self)
{
  /*
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    const gchar* encodingname;
    gchar *extension;
    gchar *path;

    //Initializations
    priv = self->priv;
    install = priv->install;
    encodingname = lw_util_get_encodingname (LW_ENCODING_UTF8);
    templist = NULL;

    if (install->postprocesslist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_encodelist (self));
      if (templist == NULL) goto errored;

      while (*tempiter != NULL)
      {
        extension = strrchr(*tempiter, '.');
        if (extension == NULL) goto errored;

        *extension = '\0';
        path = g_strjoin (".", *tempiter, encodingname, NULL);
        if (path == NULL) goto errored;
        g_free(*tempiter); *tempiter = path; path = NULL;

        tempiter++;
      }

      install->postprocesslist = templist; templist = NULL;
    }

    return install->postprocesslist;

errored:
    if (templist != NULL) g_strfreev (templist); templist = NULL;
    */
    return NULL;
}


static gchar**
lw_dictionary_installer_get_installlist (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *path;
    
    //Initializations
    priv = self->priv;
    install = priv->install;
    templist = NULL;

    if (install->installlist == NULL)
    {
      tempiter = templist = lw_dictionary_installer_get_filelist (self);
      if (templist == NULL) goto errored;

      while (*tempiter != NULL)
      {
        path = lw_util_build_filename (LW_PATH_CACHE, *tempiter);
        if (path == NULL) goto errored;
        g_free (*tempiter); *tempiter = path; path = NULL;
    
        tempiter++;
      }

      install->installlist = templist; templist = NULL;
    }

    return install->installlist;

errored:
    if (templist != NULL) g_strfreev (templist); templist = NULL;
    */
    return NULL;
}


static gchar**
lw_dictionary_installer_get_installedlist (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *path;
    gchar *directory;
    
    //Initializations
    priv = self->priv;
    install = priv->install;
    directory = lw_dictionary_get_directory (G_OBJECT_TYPE (self));
    templist = NULL;

    if (install->installedlist == NULL)
    {
      tempiter = templist = lw_dictionary_installer_get_filelist (self);
      if (templist == NULL) goto errored;

      while (*tempiter != NULL)
      {
        path = g_build_filename (directory, *tempiter, NULL);
        if (path == NULL) goto errored;
        g_free (*tempiter); *tempiter = path; path = NULL;
    
        tempiter++;
      }

      install->installedlist = templist; templist = NULL;
    }

    return install->installedlist;

errored:
    if (templist != NULL) g_strfreev (templist); templist = NULL;
    if (directory != NULL) g_free (directory); directory = NULL;
    return NULL;
    */
    return FALSE;
}



//!
//! @brief Tells the installer mechanism if it is going to fail if it tries
//!        installing because of missing info
//! @param self The LwDictionary objcet to check the validity of the urls of.  This tells you
//!        if the LWDictionary object can have the lw_installdictionary_install method
//!        called without crashing.
//!
gboolean 
lw_dictionary_installer_is_valid (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
		priv = self->priv;
    install = priv->install;

    return (install->files != NULL && install->downloads != NULL);
    */
    return FALSE;
}


//!
//! @brief Downloads or copies the file to the self directory to be worked on
//!        This function should normally only be used in the lw_installdictionary_install method.
//! @param self The LwDictionary object to use to download the self with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_download (LwDictionary *self, 
                                  LwProgress   *progress)
{
  /*TODO
    //Sanity check
    if (lw_progress_should_abort (progress)) return FALSE;
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;

    //Initializations
    priv = self->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_downloadlist (self);
    targetiter = targetlist = lw_dictionary_installer_get_decompresslist (self);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING;

    if (sourcelist != NULL && targetlist != NULL)
    {

      priv->install->index = 0;
      while (*sourceiter != NULL && *targetiter != NULL)
      {
        //File is located locally so copy it
        if (g_file_test (*sourceiter, G_FILE_TEST_IS_REGULAR))
        {
          const gchar *MESSAGE = gettext("Copying from %s...");
          lw_progress_set_secondary_message_printf (progress, MESSAGE, *sourceiter);
          lw_io_copy (*sourceiter, *targetiter, progress);
        }
        //Download the file
        else
        {
          const gchar *MESSAGE = gettext("Downloading from %s...");
          lw_progress_set_secondary_message_printf (progress, MESSAGE, *sourceiter);
          lw_io_download (*sourceiter, *targetiter, progress);
        }

        sourceiter++;
        targetiter++;
        priv->install->index++;
      }
    }

    return (!lw_progress_errored (progress));
    */
    return FALSE;
}


//!
//! @brief Detects the compression scheme of a file and decompresses it using the approprate function.
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param self The LwDictionary object to use to decompress the self with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_decompress (LwDictionary *self, 
                                    LwProgress   *progress)
{
  /*TODO
    //Sanity check
    g_return_val_if_fail (self != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
		LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;

    //Initializations
		priv = self->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_decompresslist (self);
    targetiter = targetlist = lw_dictionary_installer_get_encodelist (self);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_DECOMPRESSING;

    if (sourcelist != NULL && targetlist != NULL)
    {
      priv->install->index = 0;
      while (*sourceiter != NULL && *targetiter != NULL)
      {
        if (g_file_test (*sourceiter, G_FILE_TEST_IS_REGULAR))
        {
          gchar *filename = g_path_get_basename (*sourceiter);
          if (filename != NULL)
          {
            const gchar *MESSAGE = gettext("Decompressing %s...");
            lw_progress_set_secondary_message_printf (progress, MESSAGE, filename);
            g_free (filename); filename = NULL;
          }
          if (g_str_has_suffix (*sourceiter, "gz") || g_str_has_suffix (*sourceiter, "gzip"))
            lw_io_gunzip_file (*sourceiter, *targetiter, progress);
          else
            lw_io_copy (*sourceiter, *targetiter, progress);
        }


        sourceiter++;
        targetiter++;
        priv->install->index++;
      }
    }

    return (!lw_progress_errored (progress));
    */
}


//!
//! @brief Converts the encoding to UTF8 for the file
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param self The LwDictionary object to use to convert the text encoding the self with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_convert_encoding (LwDictionary *self,
                                          LwProgress   *progress) 
{
  /*TODO
    //Sanity check
    if (lw_progress_should_abort (progress)) return FALSE;
    g_return_val_if_fail (self != NULL, FALSE);

    //Declarations
		LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;
    const gchar *encodingname;

    //Initializations
		priv = self->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_encodelist (self);
    targetiter = targetlist = lw_dictionary_installer_get_postprocesslist (self);
    encodingname = lw_util_get_encodingname (priv->install->encoding);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_ENCODING;

    if (sourcelist != NULL && targetlist != NULL)
    {
      priv->install->index = 0;
      while (*sourceiter != NULL && *targetiter != NULL)
      {
        const gchar *MESSAGE = gettext("Converting encoding to UTF from %s...");
        lw_progress_set_secondary_message_printf (progress, MESSAGE, encodingname);
        if (priv->install->encoding == LW_ENCODING_UTF8)
          lw_io_copy (*sourceiter, *targetiter, progress);
        else
          lw_io_copy_with_encoding (*sourceiter, *targetiter, encodingname, "UTF-8", progress);

        sourceiter++;
        targetiter++;
        priv->install->index++;
      }
    }

    return (!lw_progress_errored (progress));
    */
  return FALSE;
}


//!
//! @brief does the required postprocessing on a self
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param self The LwDictionary object to use for postprocessing the self with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_postprocess (LwDictionary *self, 
                                     LwProgress   *progress)
{
  /*TODO
    //Sanity check
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (progress != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryClass *klass = NULL;
		LwDictionaryPrivate *priv = NULL;
    gchar **sourcelist = NULL, **sourceiter = NULL;
    gchar **targetlist = NULL, **targetiter = NULL;

    //Initializations
    klass = LW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self));
		priv = self->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_postprocesslist (self);
    targetiter = targetlist = lw_dictionary_installer_get_installlist (self);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING;

    const gchar *MESSAGE = gettext("Postprocessing...");
    lw_progress_set_secondary_message (progress, MESSAGE);

    if (sourcelist != NULL && targetlist != NULL)
    {
      priv->install->index = 0;
      if (klass->priv->installer_postprocess != NULL)
      {        
        klass->priv->installer_postprocess (self, sourcelist, targetlist, progress);
        priv->install->index++;
      }
      else
      {
        while (*sourceiter != NULL && *targetiter != NULL)
        {
          lw_io_copy (*sourceiter, *targetiter, progress);

          sourceiter++;
          targetiter++;
          priv->install->index++;
        }
      }
    }

    //Finish
    return (!lw_progress_errored (progress));
    */
  return FALSE;
}


//!
//! @brief does the required postprocessing on a self
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param self The LwDictionary object to use for finalizing the self with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_install (LwDictionary *self, 
                                 LwProgress   *progress)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    gchar **sourcelist = NULL, **sourceiter = NULL;
    gchar **targetlist = NULL, **targetiter = NULL;
    const gchar *MESSAGE = NULL;

    //Initializations
    priv = self->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_installlist (self);
    targetiter = targetlist = lw_dictionary_installer_get_installedlist (self);
    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_FINISHING;

    MESSAGE = gettext("Installing to %s...");

    if (sourcelist != NULL && targetiter != NULL)
    {
      lw_progress_set_secondary_message_printf (progress, MESSAGE, *targetiter);
      priv->install->index = 0;
      while (*sourceiter != NULL && *targetiter != NULL)
      {
        lw_io_copy (*sourceiter, *targetiter, progress);

        sourceiter++;
        targetiter++;
        priv->install->index++;
      }
    }

    if (!lw_progress_errored (progress))
      priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_INSTALLED;
    else
      priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_UNINSTALLED;

    lw_dictionary_sync_progress_cb (self, progress);

    //Finish
    return (!lw_progress_errored (progress));
    */
  return FALSE;
}


//!
//! @brief removes temporary files created by installation in the self cache folder
//! @param self The LwDictionary object to use to clean the files.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//!
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


//!
//! @brief Returns the percent progress as a double for only the current LwDictionary
//! @param self The LwDictionary to get the progress of
//! @param fraction The fraction percentage of the current process
//! @param The fraction percentage of all the LwDictionary processes together.
//!
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
    status = lw_dictionary_installer_get_status (self);
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
LwDictionaryInstallerStatus
lw_dictionary_installer_get_status (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations;
    priv = self->priv;
    install = priv->install;

    return install->status;
}


void 
lw_dictionary_installer_set_status (LwDictionary *self, LwDictionaryInstallerStatus status)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = self->priv;
    install = priv->install;

    install->status = status;
}


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


gboolean 
lw_dictionary_installer_get_postprocessing (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->postprocess;
}


void
lw_dictionary_installer_set_postprocessing (LwDictionary *self, gboolean postprocess)
{
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->priv != NULL);
    g_return_if_fail (self->priv->install != NULL);

    self->priv->install->postprocess = postprocess;
}
*/


LwEncoding 
lw_dictionary_installer_get_encoding (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->encoding;
    */
    return 0;
}


const gchar* 
lw_dictionary_installer_get_downloads (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->downloads;
    */
  return NULL;
}


void
lw_dictionary_installer_set_downloads (LwDictionary *self, 
                                       const gchar  *DOWNLOADS)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (DOWNLOADS != NULL);
    g_assert (self->priv->install != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    LwPreferences *preferences;
    const gchar *KEY;

    //Initializations
    priv = self->priv;
    install = priv->install;
    preferences = install->preferences;
    KEY = install->key;

    lw_preferences_set_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY, DOWNLOADS);
    */
}


void
lw_dictionary_installer_reset_downloads (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_assert (self->priv->install != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    LwPreferences *preferences;
    const gchar *KEY;

    //Initializations
    priv = self->priv;
    install = priv->install;
    preferences = install->preferences;
    KEY = install->key;

    lw_preferences_reset_value_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY);
    */
}


const gchar* 
lw_dictionary_installer_get_files (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->files;
    */
  return NULL;
}


void
lw_dictionary_installer_set_files (LwDictionary *self, const gchar *files)
{
  /*TODO
    //Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->priv != NULL);
    g_return_if_fail (self->priv->install != NULL);

    if (self->priv->install->files != NULL)
      g_free (self->priv->install->files);
    
    self->priv->install->files = g_strdup (files);
    */
}


const gchar* 
lw_dictionary_installer_get_description (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->description;
    */
  return NULL;
}


gboolean
lw_dictionary_installer_is_builtin (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_return_val_if_fail (self->priv->install != NULL, FALSE);

    return self->priv->install->builtin;
*/
    return FALSE;
}


//!
//! @brief Installs a LwDictionary object using the provided gui update callback
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param self The LwDictionary object to use for installing the self with.
//! @param cb A LwIoProgressCallback used to giver user feedback on how far the installation is.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_install (LwDictionary *self)
{
  /*TODO
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), FALSE);
    g_return_val_if_fail (self->priv != NULL, FALSE);
    g_assert (self->priv->install != NULL);
    if (lw_progress_errored (progress)) return FALSE;

    const gchar *name = lw_dictionary_get_name (self);
    const gchar *MESSAGE = gettext("Installing %s Dictionary...");
    lw_progress_set_primary_message_printf (progress, MESSAGE, name);

    lw_dictionary_installer_download (self, progress);
    lw_dictionary_installer_decompress (self, progress);
    lw_dictionary_installer_convert_encoding (self, progress);
    lw_dictionary_installer_postprocess (self, progress);
    lw_dictionary_installer_install (self, progress);
    lw_dictionary_installer_clean (self, progress);

    return (!lw_progress_errored (progress));
    */
    return FALSE;
}


/*TODO
LwDictionaryInstall*
lw_dictionary_steal_installer (LwDictionary *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_DICTIONARY (self), NULL);

    //Declarations
    LwDictionaryInstall *install;

    install = self->priv->install;
    self->priv = NULL;  
    
    return install;
}


void
lw_dictionary_set_installer (LwDictionary        *self,
                             LwDictionaryInstall *install)
{
    g_return_if_fail (LW_IS_DICTIONARY (self));
    g_return_if_fail (install != NULL);

    self->priv->install = install;
}


void
lw_dictionary_set_installer_full (LwDictionary *self,
                                  const gchar  *FILES,
                                  const gchar  *DOWNLOADS,
                                  const gchar  *DESCRIPTION,
                                  LwEncoding    encoding,
                                  gboolean      postprocess)
{
    g_return_if_fail (LW_IS_DICTIONARY (self));
    g_return_if_fail (FILES != NULL && *FILES != '\0');

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = self->priv;
    if (priv->install != NULL) lw_dictionaryinstall_free (priv->install); 
    priv->install = lw_dictionaryinstall_new ();
    if (priv->install == NULL) return;
    install = priv->install;

    if (FILES != NULL) install->files = g_strdup (FILES);
    if (DOWNLOADS != NULL) install->downloads = g_strdup (DOWNLOADS);
    if (DESCRIPTION != NULL) install->description = g_strdup (DESCRIPTION);
    install->encoding = encoding;
    install->postprocess = postprocess;
}


void
lw_dictionary_set_builtin_installer_full (LwDictionary  *self,
                                          const gchar   *FILES,
                                          LwPreferences *preferences,
                                          const gchar   *KEY,
                                          const gchar   *DESCRIPTION,
                                          LwEncoding     encoding,
                                          gboolean       postprocess)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARY (self));
    g_return_if_fail (FILES != NULL);
    g_return_if_fail (preferences != NULL);
    g_return_if_fail (KEY != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    lw_dictionary_set_installer_full (self, FILES, NULL, DESCRIPTION, encoding, postprocess);

    priv = self->priv;
    install = priv->install;
    if (install == NULL) return;

    install->listenerid = lw_preferences_add_change_listener_by_schema (
      preferences, 
      LW_SCHEMA_DICTIONARY, 
      KEY, 
      lw_dictionary_sync_downloadlist_cb, 
      self
    );

    install->preferences = preferences;
    install->key = KEY;
    install->builtin = TRUE;
}
*/


void
lw_dictionaryinstall_set_name (LwDictionaryInstall *self,
                               const gchar         *NAME)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (g_strcmp0 (NAME, priv->data.name) != 0);

    if (priv->data.name != NULL)
    {
      g_free (priv->data.name);
    }

    priv->data.name = g_strdup (NAME);

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_NAME]);
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
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (gtype != priv->data.gtype);
    
    priv->data.gtype = gtype;

    if (changed) g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_GTYPE]);
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

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DOWNLOAD_PREFERENCE_KEY]);

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


gchar**
lw_dictionaryinstall_get_dependencies (LwDictionaryInstall *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;

    //Initializations
    priv = self->priv;

    return priv->data.dependencies;
}


void
lw_dictionaryinstall_set_dependencies (LwDictionaryInstall *self,
                                       gchar               **dependencies)
{
    //Sanity checks
    g_return_if_fail (LW_IS_DICTIONARYINSTALL (self));

    //Declarations
    LwDictionaryInstallPrivate *priv = NULL;
    gboolean changed = FALSE;

    //Initializations
    priv = self->priv;
    changed = (dependencies != priv->data.dependencies);
    if (!changed) goto errored;

    g_strfreev (priv->data.dependencies);
    priv->data.dependencies = g_strdupv (dependencies);

    g_object_notify_by_pspec (G_OBJECT (self), _klasspriv->pspec[PROP_DEPENDENCIES]);

errored:

    return;
}


