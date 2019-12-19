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

/**
 * SECTION:mapped_file
 * @short_description: A wrapper for creating and managing mapped files
 * @title: LwMappedFile
 *
 * This is a wrapper around #GMappedFile with extra convenience methods
 * and it also stores the path of the mapped file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gstdio.h>

#include "gettext.h"
#include "mapped-file.h"

#define lw_mapped_file_class_get_private() G_TYPE_CLASS_GET_PRIVATE(g_type_class_peek(LW_TYPE_MAPPED_FILE), LW_TYPE_MAPPED_FILE, LwMappedFileClassPrivate)

typedef enum
{
    PROP_0,
    PROP_CONTENTS,
    PROP_CONTENT_LENGTH,
    PROP_DELETE_ON_FREE,
    PROP_WRITABLE,
    PROP_PATH,
    TOTAL_PROPS
} Prop;

typedef enum {
    CLASS_SIGNALID_UNUSED,
    TOTAL_CLASS_SIGNALIDS
} ClassSignalId;

typedef struct {
    gchar * path;
    GMappedFile * mapped_file;
    gboolean delete_on_free;
    gboolean writable;
} LwMappedFilePrivate;

typedef struct {
    guint signalid[TOTAL_CLASS_SIGNALIDS];
    GParamSpec *pspec[TOTAL_PROPS];
} LwMappedFileClassPrivate;

G_DEFINE_TYPE_WITH_CODE (LwMappedFile, lw_mapped_file, G_TYPE_OBJECT, G_ADD_PRIVATE(LwMappedFile) g_type_add_class_private(LW_TYPE_MAPPED_FILE, sizeof(LwMappedFileClassPrivate)) )

/**
* lw_mapped_file_new:
* @PATH: (transfer none) (type filename): The path to write the mapped file to or load from
* Returns: A new #LWMappedFile that can be freed with g_object_unref()
*/
LwMappedFile*
lw_mapped_file_new (gchar const * PATH)
{
    //Declarations
    LwMappedFile *self = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *path = NULL;

    //Initializations
    self = LW_MAPPED_FILE (g_object_new (LW_TYPE_MAPPED_FILE, "path", PATH, NULL));

    return self;
}


    static void 
lw_mapped_file_init (LwMappedFile *self)
{
}


    static void 
lw_mapped_file_finalize (GObject *object)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initalizations
    self = LW_MAPPED_FILE (object);
    priv = lw_mapped_file_get_instance_private (self);

    if (priv->delete_on_free && priv->path != NULL)
    {
        g_remove (priv->path);
    }

    lw_mapped_file_set_path (self, NULL);

    G_OBJECT_CLASS (lw_mapped_file_parent_class)->finalize (object);
}


    static void 
lw_mapped_file_set_property (GObject      * object,
        guint          property_id,
        const GValue * value,
        GParamSpec   * pspec)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    self = LW_MAPPED_FILE (object);
    priv = lw_mapped_file_get_instance_private (self);

    switch (property_id)
    {
        case PROP_PATH:
            lw_mapped_file_set_path (self, g_value_get_string (value));
            break;
        case PROP_DELETE_ON_FREE:
            lw_mapped_file_set_delete_on_free (self, g_value_get_boolean (value));
            break;
        case PROP_WRITABLE:
            lw_mapped_file_set_writable (self, g_value_get_boolean (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}


    static void 
lw_mapped_file_get_property (GObject    * object,
        guint        property_id,
        GValue     * value,
        GParamSpec * pspec)
{
    //Declarations
    LwMappedFile *self = NULL;
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    self = LW_MAPPED_FILE (object);
    priv = lw_mapped_file_get_instance_private (self);

    switch (property_id)
    {
        case PROP_CONTENTS:
            g_value_set_string (value, lw_mapped_file_get_contents (self));
            break;
        case PROP_CONTENT_LENGTH:
            g_value_set_ulong (value, lw_mapped_file_length (self));
            break;
        case PROP_PATH:
            g_value_set_string (value, lw_mapped_file_get_path (self));
            break;
        case PROP_DELETE_ON_FREE:
            g_value_set_boolean (value, lw_mapped_file_get_delete_on_free (self));
            break;
        case PROP_WRITABLE:
            g_value_set_boolean (value, lw_mapped_file_is_writable (self));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}


    static void
lw_mapped_file_class_init (LwMappedFileClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;
    LwMappedFileClassPrivate * klasspriv = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);
    object_class->set_property = lw_mapped_file_set_property;
    object_class->get_property = lw_mapped_file_get_property;
    object_class->finalize = lw_mapped_file_finalize;

    klasspriv = lw_mapped_file_class_get_private ();

    klasspriv->pspec[PROP_CONTENTS] = g_param_spec_string (
            "contents",
            gettext("Contents"),
            gettext("The content string for the mapped file"),
            "",
            G_PARAM_READABLE
            );
    g_object_class_install_property (object_class, PROP_CONTENTS, klasspriv->pspec[PROP_CONTENTS]);

    klasspriv->pspec[PROP_CONTENT_LENGTH] = g_param_spec_ulong (
            "content-length",
            gettext("Content Length"),
            gettext("The length of the contents"),
            0,
            G_MAXULONG,
            0,
            G_PARAM_READABLE
            );
    g_object_class_install_property (object_class, PROP_CONTENT_LENGTH, klasspriv->pspec[PROP_CONTENT_LENGTH]);

    klasspriv->pspec[PROP_PATH] = g_param_spec_string (
            "path",
            gettext("Path"),
            gettext("The path of the mapped file"),
            NULL,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE
            );
    g_object_class_install_property (object_class, PROP_PATH, klasspriv->pspec[PROP_PATH]);

    klasspriv->pspec[PROP_DELETE_ON_FREE] = g_param_spec_boolean (
            "delete-on-free",
            gettext("Delete on Free"),
            gettext("Delete the mapped file when the object is freed"),
            FALSE,
            G_PARAM_CONSTRUCT | G_PARAM_READWRITE
            );
    g_object_class_install_property (object_class, PROP_DELETE_ON_FREE, klasspriv->pspec[PROP_DELETE_ON_FREE]);

    klasspriv->pspec[PROP_WRITABLE] = g_param_spec_boolean (
            "writable",
            gettext("Writable"),
            gettext("Sets of the buffer is writable"),
            FALSE,
            G_PARAM_READWRITE
            );
    g_object_class_install_property (object_class, PROP_WRITABLE, klasspriv->pspec[PROP_WRITABLE]);
}


    static GMappedFile *
_get_mapped_file (LwMappedFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), NULL);

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    GError *error = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    if (priv->path == NULL) goto errored;

    if (priv->mapped_file == NULL)
    {
        priv->mapped_file = g_mapped_file_new (priv->path, priv->writable, &error);
    }
    if (error != NULL) goto errored;

    mapped_file = priv->mapped_file;

errored:

    if (error != NULL)
    {
        g_clear_error (&error);
        error = NULL;

        if (priv->mapped_file != NULL)
        {
            g_mapped_file_unref (priv->mapped_file);
            priv->mapped_file = NULL;
            mapped_file = NULL;
        }
    }

    return mapped_file;
}


    void
_clear_mapped_file (LwMappedFile *self)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPED_FILE (self));

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    GError *error = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);

    if (priv->mapped_file != NULL)
    {
        g_mapped_file_unref (priv->mapped_file);
        priv->mapped_file = NULL;
    }
}


/**
 * lw_mapped_file_length:
 * @self: A #LwMappedFile
 * Returns: The length of the mapped file on disk in bytes
 */
    gsize
lw_mapped_file_length (LwMappedFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), 0);

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    gsize length = 0;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    mapped_file = _get_mapped_file (self);
    if (mapped_file == NULL) goto errored;
    length = g_mapped_file_get_length (mapped_file);

errored:

    return length;
}


/**
 * lw_mapped_file_get_contents:
 * @self: A #LwMappedFile
 * Returns: The mapped file's contents as a char array
 */
    gchar *
lw_mapped_file_get_contents (LwMappedFile *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), NULL);

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    GMappedFile *mapped_file = NULL;
    gchar *contents = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    mapped_file = _get_mapped_file (self);
    if (mapped_file == NULL) goto errored;
    contents = g_mapped_file_get_contents (mapped_file);

errored:

    return contents;
}


/**
 * lw_mapped_file_set_path:
 * @self: A #LwMappedFile
 * @PATH: The path of the file that the mapped file references
 */
    void
lw_mapped_file_set_path (LwMappedFile * self,
        gchar const  * PATH)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPED_FILE (self));

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClassPrivate * klasspriv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    klasspriv = lw_mapped_file_class_get_private ();

    if (g_strcmp0 (priv->path, PATH) == 0) goto errored;

    g_free (priv->path);
    priv->path = g_strdup (PATH);

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_PATH]);

errored:

    return; 
}


/**
 * lw_mapped_file_get_path:
 * @self: A #LwMappedFile
 * Returns: (transfer none) (type filename): The path of the file that the mapped file references. The string is owned by the mapped file and should not be modified or freed.
 */
    gchar const *
                 lw_mapped_file_get_path (LwMappedFile * self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), NULL);

    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);

    return priv->path;
}


/**
 * lw_mapped_file_set_delete_on_free:
 * @self: A #LwMappedFile
 * @delete_on_free: When %TRUE, the file that this object points to will be deleted when the object is finalized
 */
    void
lw_mapped_file_set_delete_on_free (LwMappedFile * self,
        gboolean       delete_on_free)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPED_FILE (self));

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClassPrivate * klasspriv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    if (priv->delete_on_free == delete_on_free) goto errored;
    klasspriv = lw_mapped_file_class_get_private ();

    priv->delete_on_free = delete_on_free;

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_DELETE_ON_FREE]);

errored:

    return;
}


/**
 * lw_mapped_file_get_delete_on_free:
 * @self: A #LwMappedFile
 * Returns: %TRUE if the fill will be deleted when this object is freed
 */
    gboolean
lw_mapped_file_get_delete_on_free (LwMappedFile *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), FALSE);

    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);

    return priv->delete_on_free;
}


/**
 * lw_mapped_file_set_writable:
 * @self: A #LwMappedFile
 * @writable: Sets if this file is writable
 *
 * Changes the writable flag on the underlying #GMappedFile by
 * unreferencing it and replacing it with a new #GMappedFile.
 * This any buffer that was loaded with lw_mapped_file_get_contents()
 * is no longer valid and will have to be refetched.
 */
    void
lw_mapped_file_set_writable (LwMappedFile *self,
        gboolean      writable)
{
    //Sanity checks
    g_return_if_fail (LW_IS_MAPPED_FILE (self));

    //Declarations
    LwMappedFilePrivate *priv = NULL;
    LwMappedFileClassPrivate * klasspriv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);
    klasspriv = lw_mapped_file_class_get_private ();
    if (writable == priv->writable) goto errored;
    priv->writable = writable;

    _clear_mapped_file (self);

    g_object_notify_by_pspec (G_OBJECT (self), klasspriv->pspec[PROP_WRITABLE]);

errored:

    return;
}


/**
 * lw_mapped_file_is_writable:
 * @self: A #LwMappedFile
 * Returns: %TRUE if the mapped file is set to be writable
 */
    gboolean
lw_mapped_file_is_writable (LwMappedFile *self)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_MAPPED_FILE (self), FALSE);

    //Declarations
    LwMappedFilePrivate *priv = NULL;

    //Initializations
    priv = lw_mapped_file_get_instance_private (self);

    return priv->writable;
}

