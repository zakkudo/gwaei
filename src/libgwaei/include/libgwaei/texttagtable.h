#ifndef LGW_TEXTTAGTABLE_INCLUDED
#define LGW_TEXTTAGTABLE_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _LgwTextTagTable LgwTextTagTable;
typedef struct _LgwTextTagTableClass LgwTextTagTableClass;
typedef struct _LgwTextTagTablePrivate LgwTextTagTablePrivate;

#define LGW_TYPE_TEXTTAGTABLE              (lgw_texttagtable_get_type())
#define LGW_TEXTTAGTABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTable))
#define LGW_TEXTTAGTABLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTableClass))
#define LGW_IS_TEXTTAGTABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_TEXTTAGTABLE))
#define LGW_IS_TEXTTAGTABLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_TEXTTAGTABLE))
#define LGW_TEXTTAGTABLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_TEXTTAGTABLE, LgwTextTagTableClass))

struct _LgwTextTagTable {
  GtkTextTagTable tagtable;
  LgwTextTagTablePrivate *priv;
};

struct _LgwTextTagTableClass {
  GtkTextTagTableClass parent_class;
};

//Methods
GtkTextTagTable* lgw_texttagtable_new (void);
GType lgw_texttagtable_get_type (void) G_GNUC_CONST;

void lgw_texttagtable_set_preferences (LgwTextTagTable *tagtable, LwPreferences *preferences);

G_END_DECLS

#endif
