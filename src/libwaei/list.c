//!
//! @file list.c
//!

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include "gettext.h"

#include "list.h"

#define lw_list_get_class(self) G_TYPE_INSTANCE_GET_CLASS(self, LW_TYPE_LIST, LwListClass)
#define lw_list_get_class_private(self) G_TYPE_CLASS_GET_PRIVATE(lw_list_get_class(self), LW_TYPE_LIST, LwListClassPrivate)
#define lw_list_class_get_private(klass) G_TYPE_CLASS_GET_PRIVATE(klass, LW_TYPE_LIST, LwListClassPrivate)


typedef enum {
  SIGNALID_ROW_CHANGED,
  SIGNALID_ROW_INSERTED,
  SIGNALID_ROW_DELETED,
  SIGNALID_ROWS_REORDERED,
  TOTAL_SIGNALIDS
} SignalId;

typedef void (* LwListRowChangedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwListRowInsertedFunc) (LwList *list, gint index, LwIter * iter);
typedef void (* LwListRowDeletedFunc) (LwList *list, gint index);
typedef void (* LwListRowsReorderedFunc) (LwList *list, gint index, LwIter * iter, gint * new_order);

typedef struct {
  guint signalid[TOTAL_SIGNALIDS];

  /* Signals */
  LwListRowChangedFunc row_changed;
  LwListRowInsertedFunc row_inserted;
  LwListRowDeletedFunc row_deleted;
  LwListRowsReorderedFunc rows_reordered;
} LwListClassPrivate;

G_DEFINE_ABSTRACT_TYPE (LwList, lw_list, G_TYPE_OBJECT)

static void 
lw_list_init (LwList *self)
{
    GType column_type = G_TYPE_INVALID;

    column_type = lw_list_get_column_type (self);

    priv->column_type_class = G_ENUM_CLASS (g_type_class_ref (column_type));
}

static void
lw_list_finalize (GObject * object)
{
    TODO Cleanup memory here
    gint n_columns = 0;
    gint column = 0;

    n_columns = lw_list_get_n_columns (self);

    while (column < n_columns)
    {
        type = lw_list_get_column_type (self, column);
        fundamental_type = G_FUNDAMENTAL_TYPE (type);

        if (type == G_TYPE_STRV)
        {
            lw_list_get_begin_iter (self, &iter);

            while (!lw_iter_is_end (&iter))
            {
                g_free (klass->iter_get (self, column));
                lw_iter_next (&iter);
            }
        } else if (fundamental_type == G_TYPE_OBJECT)
        {
            lw_list_get_begin_iter (self, &iter);

            while (!lw_iter_is_end (&iter))
            {
                g_object_unref (klass->iter_get (self, column));
                lw_iter_next (&iter);
            }
        }

        column += 1;
    }

    g_type_class_unref (priv->column_type_class);
    priv->column_type_class = NULL;

    G_OBJECT_CLASS (lw_dictionary_parent_class)->finalize (object);
}

static void
lw_list_class_init (LwListClass * klass)
{
    //Declarations
    GObjectClass *object_class = NULL;

    //Initializations
    object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = lw_list_finalize;

    // Declarations
    LwListClassPrivate * klasspriv = NULL;
    
    // Initializations
    klasspriv = lw_list_class_get_private (klass);

    /**
     * LwResults::row-changed:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_CHANGED] = g_signal_new (
        "row-changed",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::row-inserted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_INSERTED] = g_signal_new (
        "row-inserted",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_inserted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::row-deleted:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROW_DELETED] = g_signal_new (
        "row-deleted",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, row_deleted),
        NULL, NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT
    );

    /**
     * LwResults::rows-reordered:
     * @self: A #LwResults
     * @position: The index of the row that changed
     *
     * A signal handler for #GtkTreeModel compatibility
     */
    klasspriv->signalid[SIGNALID_ROWS_REORDERED] = g_signal_new (
        "rows-reordered",
        G_OBJECT_CLASS_TYPE (klass),
        G_SIGNAL_RUN_FIRST,
        G_STRUCT_OFFSET (LwListClassPrivate, rows_reordered),
        NULL, NULL,
        g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1,
        G_TYPE_POINTER
    );
}

void 
lw_list_get_begin_iter (LwList * self,
                            LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    klass->get_begin_iter (self, iter);
}

void 
lw_list_get_end_iter (LwList * self, 
                      LwIter     * iter)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));
    g_return_if_fail (iter != NULL);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    klass->get_end_iter (self, iter);
}

gint
lw_list_get_n_columns (LwList *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), 0);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    return klass->get_n_columns (self);
}

GType 
lw_list_get_column_type (LwList * self,
                             gint         column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), G_TYPE_INVALID);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    return klass->get_column_type (self, column);
}

GType 
lw_list_get_column_value_type (LwList * self,
                             gint         column)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), G_TYPE_INVALID);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS (self);

    return klass->get_column_value_type (self, column);
}

gchar const *
lw_list_get_column_nick (LwList *self, gint column)
{
    GEnumValue * value = g_enum_get_value (priv->column_type_class, column);

    if (value == NULL)
    {
        return NULL;
    }

    return value->nick;
}

gchar const *
lw_list_get_column_name (LwList *self, gint column)
{
    GEnumValue * value = g_enum_get_value (priv->column_type_class, column);

    if (value == NULL)
    {
        return NULL;
    }

    return value->name;
}



gboolean 
lw_list_get_iter_at_position (LwList * self, 
                                  LwIter     * iter, 
                                  gint         position)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), FALSE);
    g_return_val_if_fail (iter != NULL, FALSE);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS(self);

    return klass->get_iter_at_position (self, iter, position);
}


gint 
lw_list_get_length (LwList *self)
{
    // Sanity checks
    g_return_val_if_fail (LW_IS_LIST (self), 0);

    // Declarations
    LwListClass * klass = NULL;

    // Initializations
    klass = LW_LIST_GET_CLASS(self);

    return klass->get_length (self);
}

void
lw_list_normalize (LwList * self, LwUtf8Flag flags)
{
    //Create Normalize backing for lists?? 
        LwParsed * parsed = lw_parsed_new ();
        TODO

    while (!lw_list_iter_is_end (&iter))
    {
        for (i = 0; i < n_columns; i += 1)
        {

        }
    }
}


static void
serialize_strv (gchar * preallocated_buffer, gint * offset_out, gchar ** strv)
{
    gint length = 0;

    memcpy(preallocated_buffer, g_strv_length (strv), sizeof(guint));
    offset += sizeof(guint);

    while (*strv != NULL)
    {
        length = strlen(*strv) + 1;
        offset += length;
        memcpy(preallocated_buffer, *strv, length);
        strv += 1;
    }
}

static gchar ** 
deserialize_strv (gchar * preallocated_buffer, gsize content_length, gint * offset_out)
{
    guint length = 0;
    gchar ** strv = NULL;
    gint i = 0;

    memcpy(&length, preallocated_buffer[*offset_out], sizeof(guint));
    *offset_out += sizeof(guint);
    strv = g_new0 (gchar**, length + 1);

    while (i < length)
    {
        strv[i] = preallocated_buffer[offset_out];
        *offset_out += strlen(strv[i]) + 1;
        i += 1;
    }
}

static gsize
lw_list_iter_write_column (LwIter * self, 
        gchar * preallocated_buffer, 
        gint * offset_out, 
        LwIter * iter, 
        gint  column)
{
    // Declarations
    gpointer data = NULL;
    GType type = G_TYPE_INVALID;
    GType fundamental_type = G_TYPE_INVALID;
    gint size = 0;
    
    // Initializations
    data = klass->iter_get (self, column);
    type = lw_list_get_column_type (self, column);

    if (type != G_TYPE_STRV) {
        type = G_TYPE_FUNDAMENTAL (type);
    }

    switch(type) {
        case G_TYPE_INT:
            size = sizeof(gint);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_UINT:
            size = sizeof(guint);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_LONG:
            size = sizeof(glong);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_ULONG:
            size = sizeof(gulong);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_CHAR:
            size = sizeof(gchar);
            memcpy(preallocated_buffer, data, size);
            offset += size;
        cae G_TYPE_UCHAR:
            size = sizeof(guchar);
            memcpy(preallocated_buffer, data, size);
            offset += size;
        case G_TYPE_FLOAT:
            size = sizeof(gfloat);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_DOUBLE:
            size = sizeof(gdouble);
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_STRING:
            size = strlen(data) + 1;
            memcpy(preallocated_buffer, data, size);
            offset += size;
            break;
        case G_TYPE_STRV:
            size += serialize_strv (preallocaed_buffer, &offset, data);
            break;
        case G_TYPE_BOXED:
        case G_TYPE_POINTER:
        case G_TYPE_OBJECT:
        default:
            break;

    }

    return size;
}

// Creates a serialized string of the contents, removing unused bytes
static gsize
lw_list_serialize (LwList * self,
                   gchar        * preallocated_buffer,
                   LwProgress   * progress)
{
    g_return_val_if_fail (LW_IS_LIST (self), 0);

    gsize size = 0;
    LwIter iter = {0};
    gint num_columns = 0;
    gint column = 0;
    gint offset = 0;
    GType type = G_TYPE_INVALID;

    num_colums = lw_list_num_columns (self);

    lw_list_get_begin_iter (self, &iter);

    while (!lw_list_iter_is_end (&iter))
    {
        while (column < num_columns)
        {
            lw_list_iter_write_column (self, preallocated_buffer, &offset, &iter, column);

            column += 1;
        }

        column = 0;
        lw_list_iter_next (&iter);
    }

}

static gboolean
lw_list_iter_read_column (LwIter * self, 
                            gchar        * content, 
                            gsize          length, 
                            gsize        * offset_out, 
                            gint           column)
{
    g_return_val_if_fail (content != NULL, FALSE);
    g_return_val_if_fail (offset_out != NULL, FALSE);
    g_return_val_if_fail (column_out != NULL, FALSE);
    g_return_val_if_fail (line_number_out != NULL, FALSE);

    GType type = lw_list_get_column_type (self, *column_out);

    switch(type)
    {
        case G_TYPE_INT:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(gint);
            break;
        case G_TYPE_UINT:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(guint);
            break;
        case G_TYPE_LONG:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(glong);
            break;
        case G_TYPE_ULONG:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(gulong);
            break;
        case G_TYPE_CHAR:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(gchar);
        case G_TYPE_UCHAR:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(guchar);
        case G_TYPE_FLOAT:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(gfloat);
            break;
        case G_TYPE_DOUBLE:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += sizeof(gdouble);
            break;
        case G_TYPE_STRING:
            klass->iter_set (self, column, content[*offset_out]);
            *offset_out += strlen(content[*offset_out]) + 1;
            break;
        case G_TYPE_STRV:
            klass->iter_set (self, column, deserialize_strv (content, content_length, offset_out));
            break;
        case G_TYPE_BOXED:
        case G_TYPE_POINTER:
        case G_TYPE_OBJECT:
        default:
            break;
    }
}

// 
static gsize 
lw_list_deserialize (LwList * self,
                     gchar const  * serialized_data, 
                     gsize          serialized_length, 
                     LwProgress   * progress)
{
    // Declarations
    gint n_columns = -1;
    gint length = 0; 
    gint length = 0;

    // Initializations
    n_columns = lw_list_get_n_columns (self);
    length = calculate_length(self, serialized_data);

    klass->allocate (self, length);

    while (!lw_list_iter_is_end (&iter))
    {
        column = 0;

        while (column < n_columns)
        {
            lw_list_iter_read_column (self, serialized_data, serialized_length, &offset, column, line_number);
            column += 1;
        }

        lw_list_iter_next (&iter);
    }

    return offset;
}




static GHashTable *
lw_sequence_list_previous_order_table_new (GSequence * sequence) {
    GHashTable * table = NULL;
    GSequenceIter * iter = NULL;
    gint position = 0;

    table = g_hash_table_new (g_direct_hash, g_direct_equal);
    iter = g_sequence_get_begin_iter (sequence);

    while (!g_sequence_iter_is_end (iter)) {
        g_hash_table_insert (table, iter, GINT_TO_POINTER (position));
        position += 1;
        iter = g_sequence_iter_next (iter);
    }

    return table;
}

static gint *
lw_sequence_list_map_new_order (GSequence  * sequence, 
                           GHashTable * previous_order_table)
{
    gint length = g_sequence_get_length (sequence);
    gint * new_order = NULL;
    GSequenceIter * iter = NULL;
    gint i = 0;

    new_order = g_new0 (gint, length);
    iter = g_sequence_get_begin_iter (sequence);

    while (!g_sequence_iter_is_end (iter)) {
        new_order[i] = GPOINTER_TO_INT (g_hash_table_lookup (previous_order_table, iter));
        iter = g_sequence_iter_next (iter);
    }

    return new_order;
}


void
lw_list_emit_row_inserted (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_INSERTED], 0, index);
}

void
lw_list_emit_row_changed (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_CHANGED], 0, index);
}

void
lw_list_emit_rows_reordered (LwList * self, gint * new_order)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROWS_REORDERED], 0, new_order);
}

void
lw_list_emit_row_deleted (LwList * self, gint index)
{
    // Sanity checks
    g_return_if_fail (LW_IS_LIST (self));

    // Declarations
    LwListClassPrivate * klasspriv = NULL;

    // Initializations
    klasspriv = lw_list_get_class_private (self);

    g_signal_emit (G_OBJECT (self), klasspriv->signalid[SIGNALID_ROW_DELETED], 0, index);
}
