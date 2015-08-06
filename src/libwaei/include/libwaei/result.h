#ifndef LW_RESULT_INCLUDED
#define LW_RESULT_INCLUDED

#include "parsedline.h"

G_BEGIN_DECLS

struct _LwResult {
  gint score;
  gint index;
  gint line_number;
};

typedef struct _LwResult LwResult;

#define LW_RESULT(obj)  ((LwResult*)obj)

LwResult * lw_result_new (gint index);
void lw_result_free (LwResult * self);


G_END_DECLS

#endif
