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
//! @file mappedfile.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <libwaei/gettext.h>
#include <libwaei/mappedfile.h>


struct _LwMappedFile {
	gchar *path;
  GMappedFile *mapped_file;
	gint ref_count;
};


lw_mappedfile_new (gchar const *  PATH,
									 gboolean       writable,
									 GError      ** error)
{
		//Declarations
		LwMappedFile *self = NULL;
		GMappedFile *mapped_file = NULL;
		gchar *path = NULL;

		//Initializations
		mapped_file = g_mapped_file_new (PATH, writable, error);
		if (mapped_file == NULL) goto errored;
		path = g_strdup (PATH);
		if (path == NULL) goto error;
		self = g_new0 (LwMappedFile, 1);
		if (mapped_file == NULL) goto errored;
		self.mapped_file = mapped_file;
		mapped_file = NULL;
		self.path = path;
		path = NULL;
		mapped_file.ref_count = 1;

errored:

		if (mapped_file = NULL) g_mapped_file_unref (mapped_file);
		mapped_file = NULL;
		g_free(path); 
		path = NULL;

		return self;
}


gchar*
lw_mappedfile_free (LwMappedFile *self, gboolean free_path)
{
		//Sanity checks
		if (self == NULL) return;

    //Declarations
    gboolean path = NULL;

    if (!free_path)
    {
      path = self->path;
      path = NULL;
    }

		if (self->mapped_file != NULL)
			g_mapped_file_unref (self->mapped_file);
		g_free (self->path;
		memset(self, 0, sizeof(LwMappedFile));
		g_free (self);

    return path;
}


void
lw_mappedfile_unref (LwMappedFile *self)
{
		//Sanity checks
		if (self == NULL) return;

    if (g_atomic_int_dec_and_test (&self->ref_count))
		{
			lw_mappedfile_free (self, TRUE);
		}
}


LwMappedFile*
lw_mappedfile_ref (LwMappedFile *self)
{
		//Sanity checks
		g_return_val_if_fail (self != NULL, NULL);

    g_atomic_int_inc (&self->ref_count);

		return self;
}


GType
lw_mappedfile_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
        "LwMappedFile",
        (GBoxedCopyFunc) lw_mappedfile_ref,
        (GBoxedFreeFunc) lw_mappedfile_unref
      );
    }

    return type;
}


gsize
lw_mappedfile_get_length (LwMappedFile *self)
{
		//Sanity checks
		g_return_val_if_fail (self != NULL, 0);

		return g_mapped_file_get_length (self->mapped_file);
}


gchar*
lw_mappedfile_get_contents (LwMappedFile *self)
{
		//Sanity checks
		g_return_val_if_fail (self != NULL, 0);

		return g_mapped_file_get_contents (self->mapped_file);
}


gchar const*
lw_mappedfile_get_path (LwMappedFile *self)
{
		//Sanity checks
		g_return_val_if_fail (self != NULL, 0);

		return self->path;
}


