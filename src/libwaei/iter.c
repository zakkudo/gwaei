
gint
lw_iter_get_position (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, -1);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->list);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_get_position (self);
}

gboolean 
lw_iter_next (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->list);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_next (self);
}

gboolean 
lw_iter_previous (LwIter * self)
{
    // Sanity checks
    g_return_val_if_fail (self != NULL, FALSE);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;

    // Initializations
    list = LW_LIST (self->list);
    klass = LW_LIST_GET_CLASS (list);

    return klass->iter_previous (self);
}


void
lw_iter_get_value (LwIter * self, 
                   gint     column,
                   GValue * value)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (value != NULL);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;
    gpointer data = NULL;
    GType type = G_TYPE_INVALID;
    GType fundamental_type = G_TYPE_INVALID;

    // Initializations
    list = LW_LIST (self->list);
    klass = LW_LIST_GET_CLASS (list);
    type = lw_list_get_column_type (list, column);
    fundamental_type = G_TYPE_FUNDAMENTAL (type);
    data = klass->iter_get (self, column);

    g_value_init (&value, fundamental_type);

    switch (type)
    {
        case G_TYPE_INT:
            g_value_set_int (&value, *((gint*) data));
            break;
        case G_TYPE_UINT:
            g_value_set_uint (&value, *((guint*) data));
            break;
        case G_TYPE_LONG:
            g_value_set_long (&value, *((glong*) data));
            break;
        case G_TYPE_ULONG:
            g_value_set_ulong (&value, *((gulong*) data));
            break;
        case G_TYPE_CHAR:
            g_value_set_char (&value, *((gchar*) data));
            break;
        case G_TYPE_UCHAR:
            g_value_set_uchar (&value, *((uchar*) data));
            break;
        case G_TYPE_FLOAT:
            g_value_set_float (&value, *((gfloat*) data));
            break;
        case G_TYPE_DOUBLE:
            g_value_set_double (&value, *((gdouble*) data));
            break;
        case G_TYPE_STRING:
            g_value_set_static_string (&value, *((gchar*) data));
            break;
        case G_TYPE_POINTER:
            g_value_set_pointer (&value, data);
            break;
        case G_TYPE_STRING:
            g_value_set_static_string (&value, data);
            break;
        case G_TYPE_BOXED:
            g_value_set_static_boxed (&value, data);
            break;
        case G_TYPE_OBJECT:
            g_value_set_object (&value, data);
            break;
        default:
            g_assert_not_reached ();
    }
}

void
lw_iter_get_valist (LwIter *self,
                     va_list * va)
{
    LwList * list = NULL;
    gint i = 0;
    gint column;
    GValue value = {0};
    GType type = G_TYPE_INVALID;
    gpointer data = NULL;
    gpointer destination = NULL;

    while ((column = va_arg (va, gint)) != -1)
    {
        data = klass->iter_get (self, column);
        list = LW_LIST (self->iterable);
        type = G_TYPE_FUNDAMENTAL (lw_list_get_column_type (list));
        i += 1;

        destination = va_arg (va, gpointer);

        switch (type) {
            case G_TYPE_INT:
                memcpy(destination, data, sizeof(gint));
                break;
            case G_TYPE_UINT:
                memcpy(destination, data, sizeof(guint));
                break;
            case G_TYPE_LONG:
                memcpy(destination, data, sizeof(glong));
                break;
            case G_TYPE_ULONG:
                memcpy(destination, data, sizeof(gulong));
                break;
            case G_TYPE_CHAR:
                memcpy(destination, data, sizeof(gchar));
                break;
            case G_TYPE_UCHAR:
                memcpy(destination, data, sizeof(guchar));
                break;
            case G_TYPE_FLOAT:
                memcpy(destination, data, sizeof(gfloat));
                break;
            case G_TYPE_DOUBLE:
                memcpy(destination, data, sizeof(gdouble));
                break;
            case G_TYPE_STRING:
                memcpy(destination, data, sizeof(gchar*));
                break;
            case G_TYPE_POINTER:
            case G_TYPE_BOXED:
            case G_TYPE_OBJECT:
                memcpy(destination, data, sizeof(gpointer));
                break;
            default:
                g_assert_not_reached ();
        }

        i += 1;
    }
} 

void
lw_iter_get (LwIter * self,
             ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, self);

    lw_iter_get_valist (self, self, va)

    va_end (va);
}

void
lw_iter_set_value (LwIter * self,
                   gint     column
                   GValue * value)
{
    // Sanity checks
    g_return_if_fail (self != NULL);
    g_return_if_fail (value != NULL);

    // Declarations
    LwList * list = NULL;
    LwListClass * klass = NULL;
    GType type = G_TYPE_INVALID;
    GType fundamental_type = G_TYPE_INVALID;
    gpointer data = NULL;

    // Initializations
    list = LW_LIST (self->list);
    klass = LW_LIST_GET_CLASS (list);
    type = lw_list_get_column_type (list, column);
    fundamental_type = G_TYPE_FUNDAMENTAL (type);
    gpointer data = g_value_get_pointer (value);

    if (fundamental_type == G_TYPE_OBJECT)
    {
        g_return_if_fail (G_IS_OBJECT (data));
    }

    klass->iter_set (self, column, data);
}

void
lw_iter_set_valist (LwIter *self,
                     va_list * va)
{
    gint i = 0;
    gint column;
    GValue value = {0};
    GType type = G_TYPE_INVALID;

    while ((column = va_arg (va, i)) != -1)
    {
        type = lw_list_get_column_type (i);
        i += 1;

        switch (type) {
            case G_TYPE_INT:
                g_value_init (&value, G_TYPE_INT);
                g_value_set(va_arg (va, gint));
                lw_iter_set_value(iter, column, &value);
                break;
        }


        lw_iter_set_value();
        i += 1;
    }
} 

void
lw_iter_set (LwIter * self,
             ...)
{
    //Declarations
    va_list va;

    //Initializations
    va_start(va, self);

    lw_iter_set_valist (self, self, va)

    va_end (va);
}
