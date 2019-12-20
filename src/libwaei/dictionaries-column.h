#ifndef LW_DICTIONARIES_COLUMN_INCLUDED
#define LW_DICTIONARIES_COLUMN_INCLUDED

#include "glib-object.h"

G_BEGIN_DECLS

typedef enum {
  LW_DICTIONARIES_COLUMN_NAME, //!< Name of the dictionary
  LW_DICTIONARIES_COLUMN_TYPE_NAME, //!< Name of the dictionary type
  LW_DICTIONARIES_COLUMN_ID, //!< Id of the dictionary
  LW_DICTIONARIES_COLUMN_POINTER, //!< Pointer to the LwDictionary object
  TOTAL_DICTIONARIES_COLUMNS
} LwDictionariesColumn;
GType lw_dictionaries_column_get_type (void);
#define LW_TYPE_DICTIONARIES_COLUMN (lw_dictionaries_column_get_type ())

#define LW_DICTIONARIES_COLUMN_NAME_NAME "Name"
#define LW_DICTIONARIES_COLUMN_NAME_TYPE_NAME "Type Name"
#define LW_DICTIONARIES_COLUMN_NAME_POINTER "Pointer"

#define LW_DICTIONARIES_COLUMN_NICK_NAME "name"
#define LW_DICTIONARIES_COLUMN_NICK_TYPE_NAME "type-name"
#define LW_DICTIONARIES_COLUMN_NICK_POINTER "pointer"

G_END_DECLS

#endif
