//!
//! @file list.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "list.h"

#define lw_static_list_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_STATIC_LIST, LwStaticListClass)
#define lw_static_list_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_static_list_get_class(self), LW_TYPE_STATIC_LIST, LwStaticListClassPrivate)
#define lw_static_list_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_STATIC_LIST, LwStaticListClassPrivate)

static void lw_static_list_get_begin_iter (LwStaticList * self, LwIter * iter);
static void lw_static_list_get_end_iter (LwStaticList * self, LwIter * iter);
static gint lw_static_list_get_n_columns (LwStaticList * self);
static GType lw_static_list_get_column_type (LwStaticList * self, gint column);
static gboolean lw_static_list_get_iter_at_position (LwStaticList * self,  LwIter * iter, gint position);
static gpointer lw_static_list_iter_get (LwStaticList * self, gint column);
static void lw_static_list_iter_set (LwStaticList * self, gint column, gpointer data);

static gint lw_static_list_get_length (LwStaticList * self);
static void lw_static_list_allocate (LwStaticList * self, gint length);

typedef struct {
    gpointer ** columns;
} LwStaticListPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (LwStaticList, lw_static_list, LW_TYPE_LIST, G_ADD_PRIVATE (LwStaticList))

static void 
lw_static_list_init (LwStaticList *self)
{
}

static void
lw_static_list_class_init (LwStaticListClass * klass)
{
    LwListClass * list_class = NULL;

    list_class = LW_LIST_CLASS (klass);

    list_klass->get_begin_iter = lw_static_list_get_begin_iter;
    list_klass->get_end_iter = lw_static_list_get_end_iter;
    list_klass->get_n_columns = lw_static_list_get_n_columns;
    list_klass->get_column_type = lw_static_list_get_column_type;
    list_klass->get_iter_at_position = lw_static_list_get_iter_at_position;
    list_klass->get_length = lw_static_list_get_length;

    list_class->iter_get = lw_static_list_iter_get;
    list_class->iter_set = lw_static_list_iter_set;

    list_class->allocate = lw_static_list_allocate;
    list_class->sort = lw_static_list_sort;
}

static gboolean
lw_static_list_iter_is_valid (LwIter * iter)
{
    if (iter == NULL) return FALSE;
    if (!LW_IS_STATIC_LIST (iter->iterable)) return FALSE:

    LwDictonary * dictionary = NULL;

    dictionary = LW_STATIC_LIST (iter->iterable);

    if (position > length - 1) return FALSE:
    if (position < 0) return FALSE;

    return TRUE;
}

static void 
lw_static_list_get_begin_iter (LwStaticList * self,
                              LwIter       * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_STATIC_LIST (self));
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(LwIter));
    iter->iterator = self;
    iter->user_data1 = GINT_TO_POINTER (0);
}

static void 
lw_static_list_get_end_iter (LwStaticList * self, 
                                  LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_STATIC_LIST (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwStaticListPrivate * priv = NULL;
    gint end_position = -1;

    // Initializations
    priv = lw_static_list_get_instance_private (self);
    end_postion = lw_static_list_get_length (self);

    memset(iter, 0, sizeof(LwIter));
    iter->iterator = self;
    iter->user_data1 = GINT_TO_POINTER (end_position);
}

static gint
lw_static_list_get_n_columns (LwStaticList * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_LIST (self), 0);

    // Declarations
    LwStaticListClass * klass = NULL;

    // Initializations
    klass = LW_STATIC_LIST_GET_CLASS (self);

    return klass->get_n_columns (self);
}

priv->dictionary

static GType 
lw_static_list_get_column_type (LwStaticList * self,
                                     gint                column)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_LIST (klass), G_TYPE_INVALID);

    // Declarations
    LwStaticListClass * klass = NULL;

    // Initializations
    klass = LW_STATIC_LIST_GET_CLASS (self);

    return klass->get_column_type (self, column);
}

static gboolean 
lw_static_list_get_iter_at_position (LwStaticList * self, 
                                          LwIter       * iter, 
                                          gint           position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    gint length = -1;

    // Initializations
    length = lw_static_list_get_length (self);

    if (position > length -1) {
        position = length - 1;
    }

    if (position < 0) {
        position = 0;
    }

    memset(iter, 0, sizeof(LwIter));
    iter->iterable = self;
    iter->user_data1 = GINT_TO_POINTER (position);
}


static gint 
lw_static_list_get_length (LwStaticList * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_LIST (self), 0);

    // Declarations
    LwDictionaryPrivate * priv = NULL;

    // Initializations
    priv = lw_static_list_get_instance_private (self);

    return priv->length;
}

static gint
lw_static_list_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);
    g_return_val_if_fail (lw_static_list_iter_is_valid (self), -1);

    // Declarations
    gint position = -1;

    // Initializations
    position = GPOINTER_TO_INT (self->user_data1);

    return position;
}

static void 
lw_static_list_iter_get_value (LwIter * self, 
                               gint     column,
                               GValue * value)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (value != NULL);
    g_return_if_fail (lw_static_list_iter_is_valid (self));

    // Declarations
    LwStaticListPrivate * priv = NULL;
    LwListClass * list_class = NULL;
    GType type = G_TYPE_INVALID;

    // Initializations
    priv = lw_static_list_get_instance_private (self);
    list_class = LW_LIST_CLASS (priv->dictionary_class);
    type = lw_static_list_get_column_type (self, column);

    g_value_init (&value, type);

    G_TYPE_IS_OBJECT

    switch (type)
    {
        case G_TYPE_INT:
            g_value_set_int(value, column[line_number][column])
        case G_TYPE_UINT:
            g_value_set_uint(value, column[line_number][column])
        case G_TYPE_LONG:
            g_value_set_long(value, column[line_number][column])
        case G_TYPE_ULONG:
            g_value_set_ulong(value, column[line_number][column])
        case G_TYPE_FLOAT:
            g_value_set_float(value, column[line_number][column])
        case G_TYPE_DOUBLE:
            g_value_set_double(value, column[line_number][column])
        case G_TYPE_STRING:
            g_value_set_string(value, column[line_number][column])
        case G_TYPE_STRV:
            g_value_set_strv(value, column[line_number][column])
        case G_TYPE_POINTER:
            g_value_set_pointer(value, column[line_number][column])
        case G_TYPE_OBJECT:
            g_value_set_pointer(value, column[line_number][column])
        case G_TYPE_OBJECT:
            g_value_set_pointer(value, column[line_number][column])
    }

}

static gboolean 
lw_static_list_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_static_list_iter_is_valid (self), FALSE);

    // Declarations
    LwStaticList * static_list = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    static_list = LW_STATIC_LIST (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_static_list_get_length (static_list);

    position += 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    user->user_data1 = GINT_TO_POINTER (position);
}

static gboolean 
lw_static_list_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_static_list_iter_is_valid (self), FALSE);

    // Declarations
    LwStaticList * static_list = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    static_list = LW_STATIC_LIST (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_static_list_get_length (static_list);

    position -= 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    user->user_data1 = GINT_TO_POINTER (position);
}


static void
lw_static_list_allocate (LwStaticList * self, gint length)
{
    g_free (priv->columns);
    priv->columns = g_new0 (Column*, length * n_columns);
    priv->length = length;
}

