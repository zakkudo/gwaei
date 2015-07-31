#ifndef LW_RESULT_INCLUDED
#define LW_RESULT_INCLUDED

#include "parsedline.h"

G_BEGIN_DECLS

struct _LwResult {
  LwParsedLine * line;
  gint score;
  gint index;
  GTree * numbers;
};

typedef struct _LwResult LwResult;

#define LW_RESULT(obj)  ((LwResult*)obj)

LwResult * lw_result_new (LwParsedLine * line);
void lw_result_free (LwResult * self);


gint lw_result_compare_score_func (LwResult * a, LwResult * b);

G_END_DECLS

#endif
