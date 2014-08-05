#ifndef LW_UTF8_INCLUDED
#define LW_UTF8_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LW_NORMALIZATION_CASE_INSENSITIVE = (1 << 0),
  LW_NORMALIZATION_FURIGANA_INSENSITIVE = (1 << 1),
  LW_NORMALIZATION_INSENSITIVE = (LW_NORMALIZATION_CASE_INSENSITIVE | LW_NORMALIZATION_FURIGANA_INSENSITIVE)
} LwNormalizationFlags;

gchar* lw_utf8_set_null_next_char (gchar *TEXT);
gint* lw_utf8_get_numbers (const gchar *TEXT);
gint lw_utf8_replace_linebreaks_with_nullcharacter (gchar *TEXT);
gchar* lw_utf8_normalize (const gchar *TEXT, LwNormalizationFlags flags);
gchar* lw_utf8_furiganafold (const gchar *TEXT);
void lw_utf8_sanitize (gchar *buffer);
GUnicodeScript lw_utf8_get_script (gchar *TEXT);

G_END_DECLS

#endif
