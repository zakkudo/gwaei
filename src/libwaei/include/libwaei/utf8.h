#ifndef LW_UTF8_INCLUDED
#define LW_UTF8_INCLUDED

#include <glib-object.h>
#include <libwaei/progress.h>

G_BEGIN_DECLS

#define LW_UTF8NORMALIZEFLAGNAME_NONE "None"
#define LW_UTF8NORMALIZEFLAGNAME_CASE_INSENSITIVE "Case Insensitive"
#define LW_UTF8NORMALIZEFLAGNAME_FURIGANA_INSENSITIVE "Furigana Insensitive"
#define LW_UTF8NORMALIZEFLAGNAME_INSENSITIVE "Insensitive"

typedef enum {
  LW_UTF8NORMALIZEFLAG_NONE = 0,
  LW_UTF8NORMALIZEFLAG_PRINTABLE = (1 << 0),
  LW_UTF8NORMALIZEFLAG_COMPARABLE = (1 << 1),
  LW_UTF8NORMALIZEFLAG_CASEFOLD = (1 << 2),
  LW_UTF8NORMALIZEFLAG_FURIGANAFOLD = (1 << 3),
  LW_UTF8NORMALIZEFLAG_ALL = (
    LW_UTF8NORMALIZEFLAG_PRINTABLE &
    LW_UTF8NORMALIZEFLAG_COMPARABLE &
    LW_UTF8NORMALIZEFLAG_CASEFOLD &
    LW_UTF8NORMALIZEFLAG_FURIGANAFOLD
  )
} LwUtf8NormalizeFlag;
GType lw_utf8normalizeflag_get_type (void);
LwUtf8NormalizeFlag lw_utf8normalizeflag_clean (LwUtf8NormalizeFlag flags);
#define LW_TYPE_UTF8NORMALIZEFLAG (lw_utf8normalizeflag_get_type ())

gchar* lw_utf8_set_null_next_char (gchar *TEXT);
gint* lw_utf8_get_numbers (const gchar *TEXT);
gint lw_utf8_replace_linebreaks_with_nullcharacter (gchar *TEXT);
void lw_utf8_sanitize (gchar *buffer);
GUnicodeScript lw_utf8_get_script (gchar *TEXT);
gchar* lw_utf8_convert_printf_pattern_to_regex_pattern (const gchar *PATTERN);

void lw_utf8_furiganafold (gchar *TEXT, gsize length, LwProgress *progress);
void lw_utf8_casefold (gchar *TEXT, gsize length, LwProgress *progress);

gint lw_utf8_count_lines (gchar *buffer);
gchar** lw_utf8_split_lines (gchar *buffer, gint *num_lines);

gboolean lw_utf8_validate (const gchar *TEXT, gint length, LwProgress *progress);

G_END_DECLS

#endif
