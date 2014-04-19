#ifndef LW_MORPHOLOGYSTRING_INCLUDED
#define LW_MORPHOLOGYSTRING_INCLUDED 

#include <glib.h>

G_BEGIN_DECLS

//Normalized
//Case insensitive
//Furigana Insensitive
//Stem form


struct _Group {
  gchar *str;
  gchar *insensitive_str;
};

struct _LwToken {
  struct _Group *raw;
  struct _Group stem;
  gchar *spellcheck;
  gint start_offset;
  gint end_offset;
};
typedef struct _LwToken LwToken;

LwToken* lw_token_new (const gchar *TEXT);
void lw_token_free (LwToken *self);

struct _LwMorphologyString {
  gchar *raw;
  gchar *normalized;
  gchar *internal;

  GList *tokens;
  GHashTable *lookup;

  GRegex *regex;
};
typedef struct _LwMorphologyString LwMorphologyString;

LwMorphologyString* lw_morphologystring_new (const gchar *TEXT, LwNormalizationFlags flags);
void lw_morphologystring_free (LwMorphologyString *self);

G_END_DECLS

#endif
