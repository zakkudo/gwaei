#ifndef LGW_ACTION_GROUP_INCLUDED
#define LGW_ACTION_GROUP_INCLUDED

G_BEGIN_DECLS

struct _LgwActionGroupData {
  GActionEntry const * entries;
  gint length;
};

struct _LgwActionGroup {
  GHashTable *entries;
  GtkWidget *widget;
  gboolean changed;
};

typedef struct _LgwActionGroupData LgwActionGroupData;
typedef struct _LgwActionGroup LgwActionGroup;

#define LGW_ACTIONGROUP(obj) ((LgwActionGroup*)(obj))
#define LGW_ACTIONGROUPDATA(obj) ((LgwActionGroupData*)(obj))

LgwActionGroup* lgw_actiongroup_new (GtkWidget *widget);
void lgw_actiongroup_free (LgwActionGroup *self);

void lgw_actiongroup_add_entries (LgwActionGroup *self, ...);
void lgw_actiongroup_remove_entries (LgwActionGroup *self, ...);

void lgw_actiongroup_add_to_map (LgwActionGroup *self, GActionMap *map);
void lgw_actiongroup_remove_from_map (LgwActionGroup *self, GActionMap *map);

G_END_DECLS

#endif

