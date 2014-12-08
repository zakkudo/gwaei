#ifndef LW_ROW_INCLUDED
#define LW_ROW_INCLUDED

G_BEGIN_DECLS

struct _Row {
  gint current_index;
  gint saved_index;
};

typedef struct _Row Row;

G_END_DECLS

#endif
