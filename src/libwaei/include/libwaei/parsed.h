#ifndef LW_PARSED_INCLUDED
#define LW_PARSED_INCLUDED 

#include "dictionaryline.h"

G_BEGIN_DECLS

struct _LwParsed {
  gsize num_lines; //! < The number of lines
  gchar *contents_reference_pointer; //! < The reference point of each strv
  gsize content_length;
  LwDictionaryLine *lines; //! < A set of categorized strvs
  gint ref_count;
};

typedef struct _LwParsed LwParsed;

typedef gboolean (*LwParsedForeachFunc) (LwParsed *self, LwDictionaryLine *line, gpointer user_data);
#define LW_PARSED(obj) (LwParsed*)obj

LwParsed* lw_parsed_new (gchar *contents, gsize content_length);

GType lw_parsed_get_type (void);
void lw_parsed_unref (LwParsed *self);
LwParsed* lw_parsed_ref (LwParsed *self);

void lw_parsed_foreach (LwParsed *self, LwParsedForeachFunc func, gpointer data);
LwDictionaryLine* lw_parsed_get_line (LwParsed *self, gsize line_number);
gsize lw_parsed_length (LwParsed *self);

gsize lw_parsed_serialize (LwParsed *self, gchar *preallocated_buffer, LwProgress *progress);
gsize lw_parsed_deserialize_into (LwParsed *self, const gchar *serialized_data, gsize serialized_length, LwProgress *progress);

void lw_parsed_set_lines (LwParsed *self, LwDictionaryLine *lines, gsize num_lines);

G_END_DECLS

#endif

