#ifndef LW_WORD_INCLUDED
#define LW_WORD_INCLUDED

G_BEGIN_DECLS

typedef enum {
  LW_WORD_FIELD_KANJI,
  LW_WORD_FIELD_READING,
  LW_WORD_FIELD_DEFINITION,
  LW_WORD_FIELD_CORRECT_GUESSES,
  LW_WORD_FIELD_INCORRECT_GUESSES,
  LW_WORD_FIELD_TIMESTAMP,
  TOTAL_LW_WORD_FIELDS
} LwWordField;

struct _Row {
  gint current_index;
  gint saved_index;
};

struct _LwWord {
  struct _Row row;
  gchar *score;
  gchar *days;
  gint correct_guesses;
  gint incorrect_guesses;
  gint32 timestamp;
  gboolean has_changes;
  gchar *fields[TOTAL_LW_WORD_FIELDS];
};

typedef struct _LwWord LwWord;

#define LW_WORD(obj) (LwWord*)obj

LwWord* lw_word_new (void);
LwWord* lw_word_new_from_string (const gchar*);
void lw_word_free (LwWord*);

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
