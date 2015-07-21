#ifndef LW_RADICALSDICTIONARY_INCLUDED
#define LW_RADICALSDICTIONARY_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwRadicalsDictionary LwRadicalsDictionary;
typedef struct _LwRadicalsDictionaryClass LwRadicalsDictionaryClass;
typedef struct _LwRadicalsDictionaryPrivate LwRadicalsDictionaryPrivate;

#define LW_RADICALSDICTIONARYCOLUMNNAME_KANJI "Kanji"
#define LW_RADICALSDICTIONARYCOLUMNNAME_RADICALS "Radicals"

#define LW_RADICALSDICTIONARYCOLUMNNICK_KANJI "kanji"
#define LW_RADICALSDICTIONARYCOLUMNNICK_RADICALS "radicals"

/**
 * LwRadicalsDictionaryColumnId:
 * @LW_RADICALSDICTIONARYCOLUMNID_KANJI: The kanji
 * @LW_RADICALSDICTIONARYCOLUMNID_RADICALS: The radicals of the kanji
 * @TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS: Total number of columns
 *
 * Dictionary columns for the #LwRadicalsDictionary
 */
typedef enum {
  LW_RADICALSDICTIONARYCOLUMNID_KANJI,
  LW_RADICALSDICTIONARYCOLUMNID_RADICALS,
  TOTAL_LW_RADICALSDICTIONARYCOLUMNIDS
} LwRadicalsDictionaryColumnId;

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
GType lw_radicalsdictionary_get_type (void);

GType lw_radicalsdictionary_get_columnid_type (void);
#define LW_TYPE_RADICALSDICTIONARYCOLUMNID (lw_radicalsdictionary_get_columnid_type ())

G_END_DECLS

#endif

