#ifndef LW_KANJIDICTIONARY_INCLUDED
#define LW_KANJIDICTIONARY_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwKanjiDictionary LwKanjiDictionary;
typedef struct _LwKanjiDictionaryClass LwKanjiDictionaryClass;
typedef struct _LwKanjiDictionaryPrivate LwKanjiDictionaryPrivate;


#define LW_KANJIDICTIONARY_KEY_KANJI "Kanji"
#define LW_KANJIDICTIONARY_KEY_UNICODE_SYMBOL "Unicode Symbol"
#define LW_KANJIDICTIONARY_KEY_USAGE_FREQUENCY "Usage Frequency"
#define LW_KANJIDICTIONARY_KEY_STROKE_COUNT "Stroke Level"
#define LW_KANJIDICTIONARY_KEY_GRADE_LEVEL "Grade Level"
#define LW_KANJIDICTIONARY_KEY_JLPT_LEVEL "JLPT Level"
#define LW_KANJIDICTIONARY_KEY_KUN_READINGS "Kun Reading"
#define LW_KANJIDICTIONARY_KEY_ON_READINGS "On Reading"
#define LW_KANJIDICTIONARY_KEY_MEANINGS "Meanings"

#define LW_TYPE_KANJIDICTIONARY              (lw_kanjidictionary_get_type())
#define LW_KANJIDICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_KANJIDICTIONARY, LwKanjiDictionary))
#define LW_KANJIDICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_KANJIDICTIONARY, LwKanjiDictionaryClass))
#define LW_IS_KANJIDICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_KANJIDICTIONARY))
#define LW_IS_KANJIDICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_KANJIDICTIONARY))
#define LW_KANJIDICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_KANJIDICTIONARY, LwKanjiDictionaryClass))

struct _LwKanjiDictionary {
  LwDictionary object;
};

struct _LwKanjiDictionaryClass {
  LwDictionaryClass parent_class;
};

//Methods
LwDictionary* lw_kanjidictionary_new (const gchar*, LwMorphologyEngine*);
GType lw_kanjidictionary_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

