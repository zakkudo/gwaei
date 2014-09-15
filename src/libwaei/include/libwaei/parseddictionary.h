#ifndef LW_PARSEDDICTIONARY_INCLUDED
#define LW_PARSEDDICTIONARY_INCLUDED 

#include "dictionaryline.h"

G_BEGIN_DECLS

struct _LwParsedDictionary {
  gsize num_lines; //! < The number of lines
  gchar *contents_reference_pointer; //! < The reference point of each strv
  gsize content_length;
  LwDictionaryLine *lines; //! < A set of categorized strvs
  gint ref_count;
};

typedef struct _LwParsedDictionary LwParsedDictionary;

typedef gboolean (*LwParsedDictionaryForeachFunc) (LwParsedDictionary *self, LwDictionaryLine *line, gpointer user_data);
#define LW_PARSEDDICTIONARY(obj) (LwParsedDictionary*)obj

LwParsedDictionary* lw_parseddictionary_new (gsize num_lines, gchar *contents);

GType lw_parseddictionary_get_type (void);
void lw_parseddictionary_unref (LwParsedDictionary *self);
LwParsedDictionary* lw_parseddictionary_ref (LwParsedDictionary *self);

void lw_parseddictionary_foreach (LwParsedDictionary *self, LwParsedDictionaryForeachFunc func, gpointer data);
LwDictionaryLine* lw_parseddictionary_get_line (LwParsedDictionary *self, gsize line_number);
gsize lw_parseddictionary_length (LwParsedDictionary *self);

gsize lw_parseddictionary_get_serialized_length (LwParsedDictionary *self, LwProgress *progress);
gsize lw_parseddictionary_serialize (LwParsedDictionary *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_parseddictionary_deserialize_into (LwParsedDictionary *self, const gchar *serialized_data, LwProgress *progress);

void lw_parseddictionary_set_lines (LwParsedDictionary *self, LwDictionaryLine *lines, gsize num_lines);

G_END_DECLS

#endif

