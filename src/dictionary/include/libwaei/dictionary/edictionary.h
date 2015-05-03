#ifndef LW_EDICTIONARY_INCLUDED
#define LW_EDICTIONARY_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LwEDictionary LwEDictionary;
typedef struct _LwEDictionaryClass LwEDictionaryClass;
typedef struct _LwEDictionaryPrivate LwEDictionaryPrivate;

#define LW_EDICTIONARYCOLUMNNAME_WORD "Word"
#define LW_EDICTIONARYCOLUMNNAME_READING "Reading"
#define LW_EDICTIONARYCOLUMNNAME_DEFINITION "Definition"
#define LW_EDICTIONARYCOLUMNNAME_CLASSIFICATION "Classification"
#define LW_EDICTIONARYCOLUMNNAME_POPULAR "Popular"

#define LW_EDICTIONARYCOLUMNNICK_WORD "word"
#define LW_EDICTIONARYCOLUMNNICK_READING "reading"
#define LW_EDICTIONARYCOLUMNNICK_DEFINITION "definition"
#define LW_EDICTIONARYCOLUMNNICK_CLASSIFICATION "classification"
#define LW_EDICTIONARYCOLUMNNICK_POPULAR "popular"

typedef enum {
  LW_EDICTIONARYCOLUMNID_WORD,
  LW_EDICTIONARYCOLUMNID_READING,
  LW_EDICTIONARYCOLUMNID_DEFINITION,
  LW_EDICTIONARYCOLUMNID_CLASSIFICATION,
  LW_EDICTIONARYCOLUMNID_POPULAR,
  TOTAL_LW_EDICTIONARYCOLUMNIDS
} LwEdictionaryColumnId;

GType lw_edictionary_columnid_get_type (void);
#define LW_TYPE_EDICTIONARYCOLUMNID (lw_edictionarycolumnid_get_type ())


#define LW_TYPE_EDICTIONARY              (lw_edictionary_get_type())
#define LW_EDICTIONARY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LW_TYPE_EDICTIONARY, LwEDictionary))
#define LW_EDICTIONARY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LW_TYPE_EDICTIONARY, LwEDictionaryClass))
#define LW_IS_EDICTIONARY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LW_TYPE_EDICTIONARY))
#define LW_IS_EDICTIONARY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_EDICTIONARY))
#define LW_EDICTIONARY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LW_TYPE_EDICTIONARY, LwEDictionaryClass))

struct _LwEDictionary {
  LwDictionary object;
};

struct _LwEDictionaryClass {
  LwDictionaryClass parent_class;
};


//Methods
LwDictionary* lw_edictionary_new (const gchar*);
GType lw_edictionary_get_type (void) G_GNUC_CONST;


G_END_DECLS

#endif

