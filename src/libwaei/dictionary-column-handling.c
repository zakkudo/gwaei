


/**
 * lw_dictionarycolumnhandling_get_type:
 * Returns: The #GType of the enumeration for column handling
 */
GType lw_dictionarycolumnhandling_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      GEnumValue values[] = {
        { LW_DICTIONARY_COLUMN_HANDLING_UNUSED, LW_DICTIONARY_COLUMN_HANDLING_NAME_UNUSED, LW_DICTIONARY_COLUMN_HANDLING_NICK_UNUSED },
        { LW_DICTIONARY_COLUMN_HANDLING_INDEX_AND_SEARCH, LW_DICTIONARY_COLUMN_HANDLING_NAME_INDEX_AND_SEARCH, LW_DICTIONARY_COLUMN_HANDLING_NICK_INDEX_AND_SEARCH },
        { LW_DICTIONARY_COLUMN_HANDLING_FILTER_ONLY, LW_DICTIONARY_COLUMN_HANDLING_NAME_FILTER_ONLY, LW_DICTIONARY_COLUMN_HANDLING_NICK_FILTER_ONLY },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwDictionaryColumnHandling", values);
    }

    return type;
}
