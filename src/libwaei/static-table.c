//!
//! @file table.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "table.h"

#define lw_static_table_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_STATIC_TABLE, LwStaticTableClass)
#define lw_static_table_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_static_table_get_class(self), LW_TYPE_STATIC_TABLE, LwStaticTableClassPrivate)
#define lw_static_table_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_STATIC_TABLE, LwStaticTableClassPrivate)

static void lw_static_table_get_begin_iter (LwStaticTable * self, LwIter * iter);
static void lw_static_table_get_end_iter (LwStaticTable * self, LwIter * iter);
static gint lw_static_table_get_n_columns (LwStaticTable * self);
static GType lw_static_table_get_column_type (LwStaticTable * self, gint column);
static gboolean lw_static_table_get_iter_at_position (LwStaticTable * self,  LwIter * iter, gint position);
static gpointer lw_static_table_iter_get (LwStaticTable * self, gint column);
static void lw_static_table_iter_set (LwStaticTable * self, gint column, gpointer data);

static gint lw_static_table_get_length (LwStaticTable * self);
static void lw_static_table_allocate (LwStaticTable * self, gint length);

typedef struct {
    gpointer ** columns;
} LwStaticTablePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (LwStaticTable, lw_static_table, LW_TYPE_TABLE, G_ADD_PRIVATE (LwStaticTable))

static void 
lw_static_table_init (LwStaticTable *self)
{
}

static void
lw_static_table_class_init (LwStaticTableClass * klass)
{
    LwTableClass * table_class = NULL;

    table_class = LW_TABLE_CLASS (klass);

    table_klass->get_begin_iter = lw_static_table_get_begin_iter;
    table_klass->get_end_iter = lw_static_table_get_end_iter;
    table_klass->get_n_columns = lw_static_table_get_n_columns;
    table_klass->get_column_type = lw_static_table_get_column_type;
    table_klass->get_iter_at_position = lw_static_table_get_iter_at_position;
    table_klass->get_length = lw_static_table_get_length;

    table_class->iter_get = lw_static_table_iter_get;
    table_class->iter_set = lw_static_table_iter_set;

    table_class->allocate = lw_static_table_allocate;
    table_class->sort = lw_static_table_sort;
}

static gboolean
lw_static_table_iter_is_valid (LwIter * iter)
{
    if (iter == NULL) return FALSE;
    if (!LW_IS_STATIC_TABLE (iter->iterable)) return FALSE:

    LwDictonary * dictionary = NULL;

    dictionary = LW_STATIC_TABLE (iter->iterable);

    if (position > length - 1) return FALSE:
    if (position < 0) return FALSE;

    return TRUE;
}

static void 
lw_static_table_get_begin_iter (LwStaticTable * self,
                              LwIter       * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_STATIC_TABLE (self));
    g_return_if_fail (iter != NULL);

    memset(iter, 0, sizeof(LwIter));
    iter->iterator = self;
    iter->user_data1 = GINT_TO_POINTER (0);
}

static void 
lw_static_table_get_end_iter (LwStaticTable * self, 
                                  LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_STATIC_TABLE (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwStaticTablePrivate * priv = NULL;
    gint end_position = -1;

    // Initializations
    priv = lw_static_table_get_instance_private (self);
    end_postion = lw_static_table_get_length (self);

    memset(iter, 0, sizeof(LwIter));
    iter->iterator = self;
    iter->user_data1 = GINT_TO_POINTER (end_position);
}

static gint
lw_static_table_get_n_columns (LwStaticTable * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_TABLE (self), 0);

    // Declarations
    LwStaticTableClass * klass = NULL;

    // Initializations
    klass = LW_STATIC_TABLE_GET_CLASS (self);

    return klass->get_n_columns (self);
}

priv->dictionary

static GType 
lw_static_table_get_column_type (LwStaticTable * self,
                                     gint                column)
{
    //Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_TABLE (klass), G_TYPE_INVALID);

    // Declarations
    LwStaticTableClass * klass = NULL;

    // Initializations
    klass = LW_STATIC_TABLE_GET_CLASS (self);

    return klass->get_column_type (self, column);
}

static gboolean 
lw_static_table_get_iter_at_position (LwStaticTable * self, 
                                          LwIter       * iter, 
                                          gint           position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_TABLE (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    gint length = -1;

    // Initializations
    length = lw_static_table_get_length (self);

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
lw_static_table_get_length (LwStaticTable * self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_STATIC_TABLE (self), 0);

    // Declarations
    LwDictionaryPrivate * priv = NULL;

    // Initializations
    priv = lw_static_table_get_instance_private (self);

    return priv->length;
}

static gint
lw_static_table_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);
    g_return_val_if_fail (lw_static_table_iter_is_valid (self), -1);

    // Declarations
    gint position = -1;

    // Initializations
    position = GPOINTER_TO_INT (self->user_data1);

    return position;
}

static gboolean 
lw_static_table_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_static_table_iter_is_valid (self), FALSE);

    // Declarations
    LwStaticTable * static_table = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    static_table = LW_STATIC_TABLE (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_static_table_get_length (static_table);

    position += 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    user->user_data1 = GINT_TO_POINTER (position);
}

static gboolean 
lw_static_table_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);
    g_return_val_if_fail (lw_static_table_iter_is_valid (self), FALSE);

    // Declarations
    LwStaticTable * static_table = NULL;
    gint position = -1;
    gint length = -1;
    
    // Initializations
    static_table = LW_STATIC_TABLE (self->iterable);
    position = GPOINTER_TO_INT (self->user_data1);
    length = lw_static_table_get_length (static_table);

    position -= 1;

    if (position > length - 1) {
        position = length - 1;
    } else if (position < 0) {
        position = 0;
    }

    user->user_data1 = GINT_TO_POINTER (position);
}


static void
lw_static_table_allocate (LwStaticTable * self, gint length)
{
    g_free (priv->columns);
    priv->columns = g_new0 (Column*, length * n_columns);
    priv->length = length;
}

