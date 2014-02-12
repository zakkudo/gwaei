#ifndef LGW_DICTIONARYLISTBOX_INCLUDED
#define LGW_DICTIONARYLISTBOX_INCLUDED

G_BEGIN_DECLS


//Boilerplate
typedef struct _LgwDictionaryListBox LgwDictionaryListBox;
typedef struct _LgwDictionaryListBoxClass LgwDictionaryListBoxClass;
typedef struct _LgwDictionaryListBoxPrivate LgwDictionaryListBoxPrivate;
typedef struct _LgwDictionaryListBoxClassPrivate LgwDictionaryListBoxClassPrivate;

#define LGW_TYPE_DICTIONARYLISTBOX              (lgw_dictionarylistbox_get_type())
#define LGW_DICTIONARYLISTBOX(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_DICTIONARYLISTBOX, LgwDictionaryListBox))
#define LGW_DICTIONARYLISTBOX_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_DICTIONARYLISTBOX, LgwDictionaryListBoxClass))
#define LGW_IS_DICTIONARYLISTBOX(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_DICTIONARYLISTBOX))
#define LGW_IS_DICTIONARYLISTBOX_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_DICTIONARYLISTBOX))
#define LGW_DICTIONARYLISTBOX_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_DICTIONARYLISTBOX, LgwDictionaryListBoxClass))

struct _LgwDictionaryListBox {
  GtkBox box;
  LgwDictionaryListBoxPrivate *priv;
};

struct _LgwDictionaryListBoxClass {
  GtkBoxClass parent_class;
  LgwDictionaryListBoxClassPrivate *priv;
};

//Methods

GtkWidget* lgw_dictionarylistbox_new (void);
GType lgw_dictionarylistbox_get_type (void) G_GNUC_CONST;

void lgw_dictionarylistbox_set_dictionaryliststore (LgwDictionaryListBox *dictionary_list_box, LgwDictionaryListStore *dictionary_list_store);
LgwDictionaryListStore* lgw_dictionarylistbox_get_dictionaryliststore (LgwDictionaryListBox *view);

void lgw_dictionarylistbox_set_editable (LgwDictionaryListBox *dictionary_list_box, gboolean editable);
gboolean lgw_dictionarylistbox_is_editable (LgwDictionaryListBox *dictionary_list_box);


G_END_DECLS

#endif
