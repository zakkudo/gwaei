#ifndef LGW_SEARCHENTRY_INCLUDED
#define LGW_SEARCHENTRY_INCLUDED

G_BEGIN_DECLS

//Boilerplate
typedef struct _LgwSearchEntry LgwSearchEntry;
typedef struct _LgwSearchEntryClass LgwSearchEntryClass;
typedef struct _LgwSearchEntryPrivate LgwSearchEntryPrivate;
typedef struct _LgwSearchEntryClassPrivate LgwSearchEntryClassPrivate;

#define LGW_TYPE_SEARCHENTRY              (lgw_searchentry_get_type())
#define LGW_SEARCHENTRY(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), LGW_TYPE_SEARCHENTRY, LgwSearchEntry))
#define LGW_SEARCHENTRY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), LGW_TYPE_SEARCHENTRY, LgwSearchEntryClass))
#define LGW_IS_SEARCHENTRY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), LGW_TYPE_SEARCHENTRY))
#define LGW_IS_SEARCHENTRY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), LGW_TYPE_SEARCHENTRY))
#define LGW_SEARCHENTRY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), LGW_TYPE_SEARCHENTRY, LgwSearchEntryClass))

struct _LgwSearchEntry {
  GtkBox box;
  LgwSearchEntryPrivate *priv;
};

struct _LgwSearchEntryClass {
  GtkBoxClass parent_class;
  LgwSearchEntryClassPrivate *priv;
};

GtkWidget* lgw_searchentry_new (void);
GType lgw_searchentry_get_type (void) G_GNUC_CONST;

GtkEntry* lgw_searchentry_get_entry (LgwSearchEntry *search_entry);

void lgw_searchentry_set_text (LgwSearchEntry *search_entry, const gchar *TEXT);
void lgw_searchentry_insert_text (LgwSearchEntry *search_entry, const gchar *TEXT);

G_END_DECLS

#endif
