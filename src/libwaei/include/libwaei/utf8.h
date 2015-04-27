#ifndef LW_UTF8_INCLUDED
#define LW_UTF8_INCLUDED

#include <glib-object.h>
#include <libwaei/progress.h>

G_BEGIN_DECLS

#define LW_UTF8_ERROR lw_utf8_error_quark ()

typedef enum {
  LW_UTF8_ERRORCODE_INVALID_CHARACTER,
  LW_UTF8_ERRORCODE_VALIDATION_ERROR_
} LwUtf8ErrorCode;

#define LW_UTF8FLAGNAME_NONE "None"
#define LW_UTF8FLAGNAME_PRINTABLE "Printable"
#define LW_UTF8FLAGNAME_COMPARABLE "Comparable"
#define LW_UTF8FLAGNAME_CASEFOLD "Case Fold"
#define LW_UTF8FLAGNAME_FURIGANAFOLD "Furigana Fold"
#define LW_UTF8FLAGNAME_ALL "All"

#define LW_UTF8FLAGNICK_NONE "none"
#define LW_UTF8FLAGNICK_PRINTABLE "printable"
#define LW_UTF8FLAGNICK_COMPARABLE "comparable"
#define LW_UTF8FLAGNICK_CASEFOLD "case-fold"
#define LW_UTF8FLAGNICK_FURIGANAFOLD "furigana-fold"
#define LW_UTF8FLAGNICK_ALL "all"

typedef enum {
  LW_UTF8FLAG_NONE = 0,
  LW_UTF8FLAG_PRINTABLE = (1 << 0),
  LW_UTF8FLAG_COMPARABLE = (1 << 1),
  LW_UTF8FLAG_CASEFOLD = (1 << 2),
  LW_UTF8FLAG_FURIGANAFOLD = (1 << 3),
  LW_UTF8FLAG_ALL = (
    LW_UTF8FLAG_PRINTABLE |
    LW_UTF8FLAG_COMPARABLE |
    LW_UTF8FLAG_CASEFOLD |
    LW_UTF8FLAG_FURIGANAFOLD
  )
} LwUtf8Flag;

typedef gsize(*LwUtf8ChunkHandler)(gchar*, gsize, gpointer, GError**);

GType lw_utf8flag_get_type (void);
LwUtf8Flag lw_utf8flag_clean (LwUtf8Flag flags);
#define LW_TYPE_UTF8FLAG (lw_utf8flag_get_type ())

gchar* lw_utf8_set_null_next_char (gchar *TEXT);
gint* lw_utf8_get_numbers (const gchar *TEXT);
gsize lw_utf8_replace_linebreaks_with_nullcharacter (gchar *CONTENTS, gsize content_length, gsize *max_line_length, LwProgress *progress);
gchar* lw_utf8_sanitize (gchar const * BUFFER);
GUnicodeScript lw_utf8_get_script (gchar *TEXT);
gchar* lw_utf8_convert_printf_pattern_to_regex_pattern (const gchar *PATTERN);

void lw_utf8_furiganafold (gchar *TEXT, gssize length, LwProgress *progress);
void lw_utf8_casefold (gchar *TEXT, gssize length, LwProgress *progress);

gint lw_utf8_count_lines (gchar *buffer);
gchar** lw_utf8_split_lines (gchar *buffer, gint *num_lines);

gboolean lw_utf8_validate (const gchar *TEXT, gint length, LwProgress *progress);

gchar* lw_utf8_normalize (const gchar *TEXT, gssize length, LwUtf8Flag flags);
gint lw_utf8_normalize_chunk (gchar ** output_chunk, const gchar *TEXT, LwUtf8Flag flags, gssize max_length);
void lw_utf8_normalize_chunked (gchar const * contents, gsize content_length, LwUtf8Flag flags, LwUtf8ChunkHandler chunk_handler, gpointer chunk_handler_data, LwProgress *progress);

gboolean lw_utf8_isescaped(gchar const * STR, gchar const * CHAR_PTR);

G_END_DECLS

#endif
