#ifndef LW_EXAMPLEDICTIONARY_INCLUDED
#define LW_EXAMPLEDICTIONARY_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwExampleDictionary LwExampleDictionary;
typedef struct _LwExampleDictionaryClass LwExampleDictionaryClass;
typedef struct _LwExampleDictionaryPrivate LwExampleDictionaryPrivate;

#define LW_EXAMPLEDICTIONARYCOLUMNNAME_PHRASE "Phrase"
#define LW_EXAMPLEDICTIONARYCOLUMNNAME_PHRASE_WITH_READINGS "Phrase With Readings"
#define LW_EXAMPLEDICTIONARYCOLUMNNAME_MEANING "Meaning"
#define LW_EXAMPLEDICTIONARYCOLUMNNAME_ID "ID"

#define LW_EXAMPLEDICTIONARYCOLUMNNICK_PHRASE "phrase"
#define LW_EXAMPLEDICTIONARYCOLUMNNICK_PHRASE_WITH_READINGS "phrase-with-readings"
#define LW_EXAMPLEDICTIONARYCOLUMNNICK_MEANING "meaning"
#define LW_EXAMPLEDICTIONARYCOLUMNNICK_ID "id"

/**
 * LwExampleDictionaryColumnId:
 * @LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE: The example japanese phrase
 * @LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE_WITH_READINGS: The example japanese phrase including readings
 * @LW_EXAMPLEDICTIONARYCOLUMNID_MEANING: The meaning of the japanese phrase when translated
 * @LW_EXAMPLEDICTIONARYCOLUMNID_ID: The Id of the phrase
 * @TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS: Total number of columns
 *
 * Dictionary columns for the #LwExampleDictionary
 */
typedef enum {
  LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE,
  LW_EXAMPLEDICTIONARYCOLUMNID_PHRASE_WITH_READINGS,
  LW_EXAMPLEDICTIONARYCOLUMNID_MEANING,
  LW_EXAMPLEDICTIONARYCOLUMNID_ID,
  TOTAL_LW_EXAMPLEDICTIONARYCOLUMNIDS
} LwExampleDictionaryColumnId;

#define LW_TYPE_EXAMPLEDICTIONARY              (lw_exampledictionary_get_type())
#define LW_EXAMPLEDICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_EXAMPLEDICTIONARY, LwExampleDictionary))
#define LW_EXAMPLEDICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_EXAMPLEDICTIONARY, LwExampleDictionaryClass))
#define LW_IS_EXAMPLEDICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_EXAMPLEDICTIONARY))
#define LW_IS_EXAMPLEDICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_EXAMPLEDICTIONARY))
#define LW_EXAMPLEDICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_EXAMPLEDICTIONARY, LwExampleDictionaryClass))

struct _LwExampleDictionary {
  LwDictionary object;
};

struct _LwExampleDictionaryClass {
  LwDictionaryClass parent_class;
};

//Methods
GType lw_exampledictionary_get_type (void);

GType lw_exampledictionary_columnid_get_type (void);
#define LW_TYPE_EXAMPLEDICTIONARYCOLUMNID (lw_exampledictionarycolumnid_get_type ())

G_END_DECLS

#endif

