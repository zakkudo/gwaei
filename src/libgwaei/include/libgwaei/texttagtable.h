#ifndef LGW_TEXTTAGTABLE_INCLUDED
#define LGW_TEXTTAGTABLE_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _LgwTextTagTable LgwTextTagTable;
typedef struct _LgwTextTagTableClass LgwTextTagTableClass;
typedef struct _LgwTextTagTablePrivate LgwTextTagTablePrivate;
typedef struct _LgwTextTagTableClassPrivate LgwTextTagTableClassPrivate;

#define LGW_TYPE_TEXTTAGTABLE              (lgw_texttagtable_get_type())
#define LGW_TEXTTAGTABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTable))
#define LGW_TEXTTAGTABLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTableClass))
#define LGW_IS_TEXTTAGTABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_TEXTTAGTABLE))
#define LGW_IS_TEXTTAGTABLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_TEXTTAGTABLE))
#define LGW_TEXTTAGTABLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTableClass))

struct _LgwTextTagTable {
  GtkTextTagTable tag_table;
  LgwTextTagTablePrivate *priv;
};

struct _LgwTextTagTableClass {
  GtkTextTagTableClass parent_class;
  LgwTextTagTableClassPrivate *priv;
};

//Methods
GtkTextTagTable* lgw_texttagtable_new (LwPreferences *preferences);
GType lgw_texttagtable_get_type (void) G_GNUC_CONST;

void lgw_texttagtable_set_preferences (LgwTextTagTable *tag_table, LwPreferences *preferences);
LwPreferences* lgw_texttagtable_get_preferences (LgwTextTagTable *tag_table);

G_END_DECLS

#endif
