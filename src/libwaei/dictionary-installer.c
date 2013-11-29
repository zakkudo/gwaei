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
//!  @file dictionary-installer.c
//!
//!  @brief LwDictionary objects represent a loaded dictionary that the program
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


LwDictionaryInstall*
lw_dictionaryinstall_new ()
{
		LwDictionaryInstall *install;

    install = g_new0 (LwDictionaryInstall, 1);

    return install;
}


void
lw_dictionaryinstall_free (LwDictionaryInstall *install)
{
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
}


LwDictionary* lw_dictionary_installer_new (GType type)
{
    g_return_val_if_fail (g_type_is_a (type, LW_TYPE_DICTIONARY) != FALSE, NULL);

    //Declarations
    LwDictionary *dictionary;

    //Initializations
    dictionary = LW_DICTIONARY (g_object_new (type, NULL));

    return dictionary;
}


gchar**
lw_dictionary_installer_get_filelist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **filelist;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;

    if (install->filelist == NULL)
    {
      filelist = g_strsplit (install->files, ";", -1);
    } 

    return filelist;
}


//!
//! @brief Updates the encoding of the LwDictionary
//! @param dictionary The LwDictInfo object to set the ENCODING to
//! @param ENCODING Tells the LwDictInfo object what the initial character encoding of the downloaded file will be
//!
void 
lw_dictionary_installer_set_encoding (LwDictionary *dictionary, const LwEncoding ENCODING)
{
		LwDictionaryPrivate *priv;

		priv = dictionary->priv;
    priv->install->encoding = ENCODING;
}


//!
//! @brief Updates the split state of the LwDictionary
//! @param dictionary The LwDictInfo objcet to set the POSTPROCESS variable on
//! @param POSTPROCESS The split setting to copy to the LwDictionary.
//!
void 
lw_dictionary_installer_set_postprocess (LwDictionary *dictionary, const gboolean POSTPROCESS)
{
		LwDictionaryPrivate *priv;

		priv = dictionary->priv;
    priv->install->postprocess = POSTPROCESS;
}


static gchar**
lw_dictionary_installer_get_downloadlist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);
    g_return_val_if_fail (dictionary->priv->install != NULL, NULL);
    g_return_val_if_fail (dictionary->priv->install->downloads != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    LwDictionaryInstall *install = NULL;

    //Initalizations
    priv = dictionary->priv;
    install = priv->install;

    if (install->downloadlist == NULL)
    {
      install->downloadlist = g_strsplit (install->downloads, ";", -1);
    }

    return install->downloadlist;
}


static gchar**
lw_dictionary_installer_get_decompresslist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *filename;
    gchar *path;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    templist = NULL;

    if (install->decompresslist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_downloadlist (dictionary));
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
    return NULL;
}


static gchar**
lw_dictionary_installer_get_encodelist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    const gchar* encodingname;
    gchar *extension;
    gchar *path;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    templist = NULL;

    if (install->encodelist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_decompresslist (dictionary));
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
    return NULL;
}


static gchar**
lw_dictionary_installer_get_postprocesslist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    const gchar* encodingname;
    gchar *extension;
    gchar *path;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    encodingname = lw_util_get_encodingname (LW_ENCODING_UTF8);
    templist = NULL;

    if (install->postprocesslist == NULL)
    {
      tempiter = templist = g_strdupv (lw_dictionary_installer_get_encodelist (dictionary));
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
    return NULL;
}


static gchar**
lw_dictionary_installer_get_installlist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *path;
    
    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    templist = NULL;

    if (install->installlist == NULL)
    {
      tempiter = templist = lw_dictionary_installer_get_filelist (dictionary);
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
    return NULL;
}


static gchar**
lw_dictionary_installer_get_installedlist (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    gchar **templist, **tempiter;
    gchar *path;
    gchar *directory;
    
    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    directory = lw_dictionary_get_directory (G_OBJECT_TYPE (dictionary));
    templist = NULL;

    if (install->installedlist == NULL)
    {
      tempiter = templist = lw_dictionary_installer_get_filelist (dictionary);
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
}



//!
//! @brief Tells the installer mechanism if it is going to fail if it tries
//!        installing because of missing info
//! @param dictionary The LwDictionary objcet to check the validity of the urls of.  This tells you
//!        if the LWDictionary object can have the lw_installdictionary_install method
//!        called without crashing.
//!
gboolean 
lw_dictionary_installer_is_valid (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
		priv = dictionary->priv;
    install = priv->install;

    return (install->files != NULL && install->downloads != NULL);
}


//!
//! @brief Downloads or copies the file to the dictionary directory to be worked on
//!        This function should normally only be used in the lw_installdictionary_install method.
//! @param dictionary The LwDictionary object to use to download the dictionary with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_download (LwDictionary *dictionary, 
                                  LwProgress   *progress)
{
    //Sanity check
    if (lw_progress_should_abort (progress)) return FALSE;
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
    LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;

    //Initializations
    priv = dictionary->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_downloadlist (dictionary);
    targetiter = targetlist = lw_dictionary_installer_get_decompresslist (dictionary);

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
          lw_progress_set_secondary_message (progress, MESSAGE, *sourceiter);
          lw_io_copy (*sourceiter, *targetiter, progress);
        }
        //Download the file
        else
        {
          const gchar *MESSAGE = gettext("Downloading from %s...");
          lw_progress_set_secondary_message (progress, MESSAGE, *sourceiter);
          lw_io_download (*sourceiter, *targetiter, progress);
        }

        sourceiter++;
        targetiter++;
        priv->install->index++;
      }
    }

    return (!lw_progress_errored (progress));
}


//!
//! @brief Detects the compression scheme of a file and decompresses it using the approprate function.
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param dictionary The LwDictionary object to use to decompress the dictionary with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_decompress (LwDictionary *dictionary, 
                                    LwProgress   *progress)
{
    //Sanity check
    g_return_val_if_fail (dictionary != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
		LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;

    //Initializations
		priv = dictionary->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_decompresslist (dictionary);
    targetiter = targetlist = lw_dictionary_installer_get_encodelist (dictionary);

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
            lw_progress_set_secondary_message (progress, MESSAGE, filename);
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
}


//!
//! @brief Converts the encoding to UTF8 for the file
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param dictionary The LwDictionary object to use to convert the text encoding the dictionary with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_convert_encoding (LwDictionary *dictionary,
                                          LwProgress   *progress) 
{
    //Sanity check
    if (lw_progress_should_abort (progress)) return FALSE;
    g_return_val_if_fail (dictionary != NULL, FALSE);

    //Declarations
		LwDictionaryPrivate *priv;
    gchar **sourcelist, **sourceiter;
    gchar **targetlist, **targetiter;
    const gchar *encodingname;

    //Initializations
		priv = dictionary->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_encodelist (dictionary);
    targetiter = targetlist = lw_dictionary_installer_get_postprocesslist (dictionary);
    encodingname = lw_util_get_encodingname (priv->install->encoding);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_ENCODING;

    if (sourcelist != NULL && targetlist != NULL)
    {
      priv->install->index = 0;
      while (*sourceiter != NULL && *targetiter != NULL)
      {
        const gchar *MESSAGE = gettext("Converting encoding to UTF from %s...");
        lw_progress_set_secondary_message (progress, MESSAGE, encodingname);
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
}


//!
//! @brief does the required postprocessing on a dictionary
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param dictionary The LwDictionary object to use for postprocessing the dictionary with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_postprocess (LwDictionary *dictionary, 
                                     LwProgress   *progress)
{
    //Sanity check
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (progress != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryClass *klass = NULL;
		LwDictionaryPrivate *priv = NULL;
    gchar **sourcelist = NULL, **sourceiter = NULL;
    gchar **targetlist = NULL, **targetiter = NULL;

    //Initializations
    klass = LW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (dictionary));
		priv = dictionary->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_postprocesslist (dictionary);
    targetiter = targetlist = lw_dictionary_installer_get_installlist (dictionary);

    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING;

    const gchar *MESSAGE = gettext("Postprocessing...");
    lw_progress_set_secondary_message (progress, MESSAGE);

    if (sourcelist != NULL && targetlist != NULL)
    {
      priv->install->index = 0;
      if (klass->priv->installer_postprocess != NULL)
      {        
        klass->priv->installer_postprocess (dictionary, sourcelist, targetlist, progress);
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
}


//!
//! @brief does the required postprocessing on a dictionary
//!        This function should normally only be used in the lw_installdictionary_install function.
//! @param dictionary The LwDictionary object to use for finalizing the dictionary with.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//! @param error A pointer to a GError object to pass errors to or NULL.
//! @see lw_installdictionary_download
//! @see lw_installdictionary_convert_encoding
//! @see lw_installdictionary_postprocess
//! @see lw_installdictionary_install
//!
gboolean 
lw_dictionary_installer_install (LwDictionary *dictionary, 
                                 LwProgress   *progress)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    if (lw_progress_should_abort (progress)) return FALSE;

    //Declarations
    LwDictionaryPrivate *priv = NULL;
    gchar **sourcelist = NULL, **sourceiter = NULL;
    gchar **targetlist = NULL, **targetiter = NULL;
    const gchar *MESSAGE = NULL;

    //Initializations
    priv = dictionary->priv;
    sourceiter = sourcelist = lw_dictionary_installer_get_installlist (dictionary);
    targetiter = targetlist = lw_dictionary_installer_get_installedlist (dictionary);
    priv->install->status = LW_DICTIONARY_INSTALLER_STATUS_FINISHING;

    MESSAGE = gettext("Installing to %s...");

    if (sourcelist != NULL && targetiter != NULL)
    {
      lw_progress_set_secondary_message (progress, MESSAGE, *targetiter);
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

    lw_dictionary_sync_progress_cb (dictionary, progress);

    //Finish
    return (!lw_progress_errored (progress));
}


//!
//! @brief removes temporary files created by installation in the dictionary cache folder
//! @param dictionary The LwDictionary object to use to clean the files.
//! @param data A gpointer to data to pass to the LwIoProgressCallback.
//!
void 
lw_dictionary_installer_clean (LwDictionary *dictionary, 
                               LwProgress   *progress)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;

    if (install->filelist != NULL) g_strfreev (install->filelist); install->filelist = NULL;
    if (install->downloadlist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->decompresslist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->encodelist != NULL) g_strfreev (install->downloadlist); install->downloadlist = NULL;
    if (install->postprocesslist != NULL) g_strfreev (install->postprocesslist); install->postprocesslist = NULL;
    if (install->installlist != NULL) g_strfreev (install->installlist); install->installlist = NULL;
    if (install->installedlist != NULL) g_strfreev (install->installedlist); install->installedlist = NULL;
}


gdouble 
lw_dictionary_installer_get_progress (LwDictionary *dictionary)
{
    //Declarations
		LwDictionaryPrivate *priv;

    //Initializations
		priv = dictionary->priv;

    return priv->progress;
}


//!
//! @brief Returns the percent progress as a double for only the current LwDictionary
//! @param dictionary The LwDictionary to get the progress of
//! @param fraction The fraction percentage of the current process
//! @param The fraction percentage of all the LwDictionary processes together.
//!
gdouble 
lw_dictionary_installer_get_stage_progress (LwDictionary *dictionary)
{
    //Declarations
		LwDictionaryPrivate *priv;
    LwDictionaryInstallerStatus status;
    gint index;
    gdouble current;
    gdouble final;
    gchar **list;
    gdouble fraction;
    
    //Initializations
		priv = dictionary->priv;
    fraction = priv->progress;
    current = final = 0.0;

    //Get the current progress
    status = lw_dictionary_installer_get_status (dictionary);
    index = lw_dictionary_installer_get_file_index (dictionary);

    switch (status)
    {
      case LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING:
        list = lw_dictionary_installer_get_downloadlist (dictionary);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_DECOMPRESSING:
        list = lw_dictionary_installer_get_encodelist (dictionary);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_ENCODING:
        list = lw_dictionary_installer_get_postprocesslist (dictionary);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING:
        list = lw_dictionary_installer_get_installlist (dictionary);
        break;
      case LW_DICTIONARY_INSTALLER_STATUS_FINISHING:
        list = lw_dictionary_installer_get_installedlist (dictionary);
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
}


//!
//! @brief Gets the total possible progress for the LwDictionary.  It should be
//!        divided by the current progress to get the appropriate fraction
//! @param dictionary The LwDictInfo object to get the total progress of
//! @param fraction The fraction progress of the current process.
//!
gdouble 
lw_dictionary_installer_get_total_progress (LwDictionary *dictionary)
{
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
		priv = dictionary->priv;
    status = lw_dictionary_installer_get_status (dictionary);
    current = final = 0.0;
    fraction = priv->progress;
    const gdouble DOWNLOAD_WEIGHT = 3.0;

    for (i = 0; i < TOTAL_LW_DICTIONARY_INSTALLER_STATUSES; i++)
    {
      switch (i)
      {
        case LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING:
          list = lw_dictionary_installer_get_downloadlist (dictionary);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_DECOMPRESSING:
          list = lw_dictionary_installer_get_encodelist (dictionary);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_ENCODING:
          list = lw_dictionary_installer_get_postprocesslist (dictionary);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_POSTPROCESSING:
          list = lw_dictionary_installer_get_installlist (dictionary);
          break;
        case LW_DICTIONARY_INSTALLER_STATUS_FINISHING:
          list = lw_dictionary_installer_get_installedlist (dictionary);
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

    temp = lw_dictionary_installer_get_stage_progress (dictionary);
    if (i == LW_DICTIONARY_INSTALLER_STATUS_DOWNLOADING)
      current += temp * DOWNLOAD_WEIGHT;
    else
      current += temp;

    if (final == 0.0)
      fraction = 0.0;
    else
      fraction = current / final;

    return fraction;
}


LwDictionaryInstallerStatus
lw_dictionary_installer_get_status (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations;
    priv = dictionary->priv;
    install = priv->install;

    return install->status;
}


void 
lw_dictionary_installer_set_status (LwDictionary *dictionary, LwDictionaryInstallerStatus status)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;

    install->status = status;
}


gint
lw_dictionary_installer_get_file_index (LwDictionary *dictionary)
{
    //Sanity check
    g_return_val_if_fail (dictionary != NULL, -1);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;

    return install->index;
}


gboolean 
lw_dictionary_installer_get_postprocessing (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->postprocess;
}


void
lw_dictionary_installer_set_postprocessing (LwDictionary *dictionary, gboolean postprocess)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (dictionary->priv != NULL);
    g_return_if_fail (dictionary->priv->install != NULL);

    dictionary->priv->install->postprocess = postprocess;
}


LwEncoding 
lw_dictionary_installer_get_encoding (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->encoding;
}


const gchar* 
lw_dictionary_installer_get_downloads (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->downloads;
}


void
lw_dictionary_installer_set_downloads (LwDictionary *dictionary, 
                                       const gchar  *DOWNLOADS)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (DOWNLOADS != NULL);
    g_assert (dictionary->priv->install != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    LwPreferences *preferences;
    const gchar *KEY;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    preferences = install->preferences;
    KEY = install->key;

    lw_preferences_set_string_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY, DOWNLOADS);
}


void
lw_dictionary_installer_reset_downloads (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_assert (dictionary->priv->install != NULL);

    //Declarations
    LwDictionaryPrivate *priv;
    LwDictionaryInstall *install;
    LwPreferences *preferences;
    const gchar *KEY;

    //Initializations
    priv = dictionary->priv;
    install = priv->install;
    preferences = install->preferences;
    KEY = install->key;

    lw_preferences_reset_value_by_schema (preferences, LW_SCHEMA_DICTIONARY, KEY);
}


const gchar* 
lw_dictionary_installer_get_files (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->files;
}


void
lw_dictionary_installer_set_files (LwDictionary *dictionary, const gchar *files)
{
    //Sanity checks
    g_return_if_fail (dictionary != NULL);
    g_return_if_fail (dictionary->priv != NULL);
    g_return_if_fail (dictionary->priv->install != NULL);

    if (dictionary->priv->install->files != NULL)
      g_free (dictionary->priv->install->files);
    
    dictionary->priv->install->files = g_strdup (files);
}


const gchar* 
lw_dictionary_installer_get_description (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->description;
}


gboolean
lw_dictionary_installer_is_builtin (LwDictionary *dictionary)
{
    //Sanity checks
    g_return_val_if_fail (dictionary != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv != NULL, FALSE);
    g_return_val_if_fail (dictionary->priv->install != NULL, FALSE);

    return dictionary->priv->install->builtin;
}

