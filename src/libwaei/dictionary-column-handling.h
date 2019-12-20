

typedef enum {
  LW_DICTIONARY_COLUMN_HANDLING_UNUSED, //!< An unused field
  LW_DICTIONARY_COLUMN_HANDLING_SEARCH_ONLY, //!< Column is not indexed, but it is still searched
  LW_DICTIONARY_COLUMN_HANDLING_INDEX_AND_SEARCH, //!< Key is indexed and is included by default for all searches
  LW_DICTIONARY_COLUMN_HANDLING_FILTER_ONLY, //!< Key is indexed, but is only included when queried explicitly
  TOTAL_DICTIONARY_COLUMN_HANDLING
} LwDictionaryColumnHandling;
GType lw_dictionary_column_handling_get_type (void);
#define LW_TYPE_DICTIONARY_COLUMN_HANDLING (lw_dictionary_column_handling_get_type ())

#define LW_DICTIONARY_COLUMN_HANDLING_NAME_UNUSED "Unused"
#define LW_DICTIONARY_COLUMN_HANDLING_NAME_INDEX_AND_SEARCH "Index and Search"
#define LW_DICTIONARY_COLUMN_HANDLING_NAME_FILTER_ONLY "Filter Only"

#define LW_DICTIONARY_COLUMN_HANDLING_NICK_UNUSED "unused"
#define LW_DICTIONARY_COLUMN_HANDLING_NICK_INDEX_AND_SEARCH "index-and-search"
#define LW_DICTIONARY_COLUMN_HANDLING_NICK_FILTER_ONLY "filter-only"
