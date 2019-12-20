
#include "dictionaries-column.h"

/**
 * lw_dictionaries_column_get_type:
 * Returns: The #GType of the enumeration for column handling
 */
GType lw_dictionaries_column_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_DICTIONARIES_COLUMN_NAME, LW_DICTIONARIES_COLUMN_NAME_NAME, LW_DICTIONARIES_COLUMN_NICK_NAME },
        { LW_DICTIONARIES_COLUMN_TYPE_NAME, LW_DICTIONARIES_COLUMN_NAME_TYPE_NAME, LW_DICTIONARIES_COLUMN_NICK_TYPE_NAME },
        { LW_DICTIONARIES_COLUMN_POINTER, LW_DICTIONARIES_COLUMN_NAME_POINTER, LW_DICTIONARIES_COLUMN_NICK_POINTER },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwDictionariesColumn", values);
    }

    return type;
}
