#ifndef LW_UTF8_INCLUDED
#define LW_UTF8_INCLUDED

#include <glib-object.h>
#include <libwaei/progress.h>

G_BEGIN_DECLS

#define LW_UTF8_ERROR lw_utf8_error_quark ()
GQuark lw_utf8_error_quark (void);

typedef enum {
  LW_UTF8_ERRORCODE_VALIDATION_ERROR,
  LW_UTF8_ERRORCODE_NORMALIZATION_ERROR
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

/**
 * LwUtf8Flag:
 * @LW_UTF8FLAG_NONE: Don't do any normalization
 * @LW_UTF8FLAG_PRINTABLE: Normalize only things that print the same
 * @LW_UTF8FLAG_COMPARABLE: Normalize for comparison, but the printing may change
 * @LW_UTF8FLAG_CASEFOLD: Convert capital letters to lowercase letters
 * @LW_UTF8FLAG_FURIGANAFOLD: Convert katakana to hiragana
 * @LW_UTF8FLAG_ALL: All options from above are applied
 *
 * Enum values for deciding how a string will be normalized.  You can logically
 * or them together for different combinations.
 */
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

/**
 * LwUtf8ChunkHandler:
 * @chunk: Chunk of text to operate on from a larger file
 * @chunk_length: The length of this chunk of text
 * @data: User data passed to the handler
 * @error: A pointer to set #GErrors which will stop chunk handling
 * Returns: The number of bytes used from the @chunk.  If it doesn't equal chunk_length, chunk parsing will be halted.
 */
typedef gsize(*LwUtf8ChunkHandler)(gchar * chunk, gsize chunk_length, gpointer data, GError ** error);

GType lw_utf8flag_get_type (void);
LwUtf8Flag lw_utf8flag_clean (LwUtf8Flag flags);
#define LW_TYPE_UTF8FLAG (lw_utf8flag_get_type ())

gchar* lw_utf8_set_null_next_char (gchar *TEXT);
gint* lw_utf8_get_numbers (const gchar *TEXT);
gsize lw_utf8_replace_linebreaks_with_nullcharacter (gchar *CONTENTS, gsize content_length, gsize *max_line_length, LwProgress *progress);
gchar* lw_utf8_sanitize (gchar const * TEXT);
GUnicodeScript lw_utf8_get_script (gchar *TEXT);
gchar* lw_utf8_convert_printf_pattern_to_regex_pattern (const gchar *PATTERN);

void lw_utf8_furiganafold (gchar *text, gsize length, LwProgress *progress);
void lw_utf8_casefold (gchar *text, gsize length, LwProgress *progress);

gint lw_utf8_count_lines (gchar const * TEXT);

gboolean lw_utf8_validate (const gchar *TEXT, gsize length, LwProgress *progress);

gchar* lw_utf8_normalize (const gchar *TEXT, gsize length, LwUtf8Flag flags);
void lw_utf8_normalize_chunked (gchar const * contents, gsize content_length, LwUtf8Flag flags, LwUtf8ChunkHandler chunk_handler, gpointer chunk_handler_data, LwProgress *progress);

gboolean lw_utf8_isescaped (gchar const * TEXT, gchar const * CHAR_PTR);

gboolean lw_utf8_contains_kanji (gchar const * TEXT);
gboolean lw_utf8_contains_furigana (gchar const * TEXT);
gboolean lw_utf8_contains_romaji (gchar const * TEXT);
gboolean lw_utf8_contains_number (gchar const * TEXT);

G_END_DECLS

#endif
