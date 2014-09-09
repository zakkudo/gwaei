#ifndef LW_RADICALSDICTIONARY_INCLUDED
#define LW_RADICALSDICTIONARY_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwRadicalsDictionary LwRadicalsDictionary;
typedef struct _LwRadicalsDictionaryClass LwRadicalsDictionaryClass;
typedef struct _LwRadicalsDictionaryPrivate LwRadicalsDictionaryPrivate;

#define LW_RADICALSDICTIONARYTOKENNAME_KANJI "Kanji"
#define LW_RADICALSDICTIONARYTOKENNAME_RADICALS "Radicals"

#define LW_RADICALSDICTIONARYTOKENNICK_KANJI "kanji"
#define LW_RADICALSDICTIONARYTOKENNICK_RADICALS "radicals"

typedef enum {
  LW_RADICALSDICTIONARYTOKENID_KANJI,
  LW_RADICALSDICTIONARYTOKENID_RADICALS,
  TOTAL_LW_RADICALSDICTIONARYTOKENIDS
} LwRadicalsDictionaryTokenId;

#define LW_TYPE_RADICALSDICTIONARY              (lw_radicalsdictionary_get_type())
#define LW_RADICALSDICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_RADICALSDICTIONARY, LwRadicalsDictionary))
#define LW_RADICALSDICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_RADICALSDICTIONARY, LwRadicalsDictionaryClass))
#define LW_IS_RADICALSDICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_RADICALSDICTIONARY))
#define LW_IS_RADICALSDICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_RADICALSDICTIONARY))
#define LW_RADICALSDICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_RADICALSDICTIONARY, LwRadicalsDictionaryClass))

struct _LwRadicalsDictionary {
  LwDictionary object;
};

struct _LwRadicalsDictionaryClass {
  LwDictionaryClass parent_class;
};

//Methods
LwDictionary* lw_radicalsdictionary_new (const gchar*, LwMorphologyEngine*);
GType lw_radicalsdictionary_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

