#include <libwaei/sort-direction.h>

/**
 * lw_sort_direction_get_type:
 * Returns: The #GType of the enumeration for column handling
 */
GType lw_sort_direction_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_SORT_DIRECTION_ASCENDING, LW_SORT_DIRECTION_NAME_ASCENDING, LW_SORT_DIRECTION_NICK_ASCENDING },
        { LW_SORT_DIRECTION_DESCENDING, LW_SORT_DIRECTION_NAME_DESCENDING, LW_SORT_DIRECTION_NICK_DESCENDING },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwSortDirection", values);
    }

    return type;
}
