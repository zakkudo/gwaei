#ifndef LW_WORD_INCLUDED
#define LW_WORD_INCLUDED

G_BEGIN_DECLS

#define LW_WORDFIELDNAME_KANJI "Kanji"
#define LW_WORDFIELDNAME_READING "Reading"
#define LW_WORDFIELDNAME_DEFINITION "Definition"
#define LW_WORDFIELDNAME_CORRECT_GUESSES "Correct Guesses"
#define LW_WORDFIELDNAME_INCORRECT_GUESSES "Incorrect Guesses"
#define LW_WORDFIELDNAME_TIMESTAMP "Timestamp"

#define LW_WORDFIELDNICK_KANJI "kanji"
#define LW_WORDFIELDNICK_READING "reading"
#define LW_WORDFIELDNICK_DEFINITION "definition"
#define LW_WORDFIELDNICK_CORRECT_GUESSES "correct-guesses"
#define LW_WORDFIELDNICK_INCORRECT_GUESSES "incorrect-guesses"
#define LW_WORDFIELDNICK_TIMESTAMP "timestamp"

typedef enum {
  LW_WORDFIELD_KANJI,
  LW_WORDFIELD_READING,
  LW_WORDFIELD_DEFINITION,
  LW_WORDFIELD_CORRECT_GUESSES,
  LW_WORDFIELD_INCORRECT_GUESSES,
  LW_WORDFIELD_TIMESTAMP,
  TOTAL_LW_WORDFIELDS
} LwWordField;

GType lw_wordfield_get_type (void);
#define LW_TYPE_WORDFIELD (lw_wordfield_get_type ())

struct _LwWord {
  gchar *score;
  gchar *days;
  gint correct_guesses;
  gint incorrect_guesses;
  gint32 timestamp;
  gboolean has_changes;
  gchar *fields[TOTAL_LW_WORDFIELDS];
};

typedef struct _LwWord LwWord;

#define LW_WORD(obj) (LwWord*)obj
#define LW_TYPE_WORD (lw_word_get_type())

LwWord* lw_word_new (void);
LwWord* lw_word_new_from_string (const gchar*);
void lw_word_free (LwWord*);
GType lw_word_get_type (void);

void lw_word_set_kanji (LwWord*, const gchar*);
const gchar* lw_word_get_kanji (LwWord*);

void lw_word_set_reading (LwWord*, const gchar*);
const gchar* lw_word_get_reading (LwWord*);

void lw_word_set_definition (LwWord*, const gchar*);
const gchar* lw_word_get_definition (LwWord*);

gint lw_word_get_correct_guesses (LwWord*);
void lw_word_set_correct_guesses (LwWord*, gint);

gint lw_word_get_incorrect_guesses (LwWord*);
void lw_word_set_incorrect_guesses (LwWord*, gint);

gint lw_word_get_score (LwWord*);
const gchar* lw_word_get_score_as_string (LwWord*);

guint32 lw_word_timestamp_to_hours (gint64);
void lw_word_set_timestamp (LwWord*, gint64);
void lw_word_update_timestamp (LwWord*);
void lw_word_set_last_studied (LwWord*, guint32);
guint32 lw_word_get_last_studied (LwWord*);
const gchar* lw_word_get_last_studied_as_string (LwWord*);

gchar* lw_word_to_string (LwWord*);
void lw_word_update_timestamp (LwWord*);
gint64 lw_word_get_timestamp (LwWord*);
const gchar* lw_word_get_timestamp_as_string (LwWord*);

gboolean lw_word_has_changes (LwWord *word);

LwWord* lw_word_copy (LwWord *word);

G_END_DECLS

#endif
