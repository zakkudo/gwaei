


/**
 * lw_search_status_get_type:
 * Returns: The #GType of the flags
 */
GType
lw_search_status_get_type ()
{
    static GType type = 0;

    if (G_UNLIKELY (type == 0))
    {
      static GEnumValue values[] = {
        { LW_SEARCH_STATUS_UNSTARTED, LW_SEARCH_STATUS_NAME_UNSTARTED, LW_SEARCH_STATUS_NICK_UNSTARTED },
        { LW_SEARCH_STATUS_SEARCHING, LW_SEARCH_STATUS_NAME_SEARCHING, LW_SEARCH_STATUS_NICK_SEARCHING },
        { LW_SEARCH_STATUS_CANCELING, LW_SEARCH_STATUS_NAME_CANCELING, LW_SEARCH_STATUS_NICK_CANCELING },
        { LW_SEARCH_STATUS_CANCELLED, LW_SEARCH_STATUS_NAME_CANCELLED, LW_SEARCH_STATUS_NICK_CANCELLED },
        { LW_SEARCH_STATUS_FINISHED,  LW_SEARCH_STATUS_NAME_FINISHED,  LW_SEARCH_STATUS_NICK_FINISHED },
        { 0, NULL, NULL },
      };

      type = g_enum_register_static ("LwSearchStatus", values);
    }

    return type;
}
