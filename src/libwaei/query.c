

/*
  DOESNT WORK.  CONTINUE TO USE & for and in regular expressions

  BEFORE: word:日本語 another word reading:ふりがな (definition:swimming) fish strokes:3
  AFTER: word:日本語&&another word&&reading:ふりがな&definition:swimming&&fish&strokes:4

  BEFORE: definition:swimming fish
  AFTER: definition:swimming fish

  BEFORE: one(two)three
  AFTER: one(two)three

  BEFORE: (one||two)three
  AFTER: (one||two)three

  BEFORE: (one&&two)three
  AFTER: (one&&two)&&three

  BEFORE: (one&&two)||three
  AFTER:

  BEFORE: one&&two||three
  AFTER: one&&(two||three)

  BEFORE: one&&(two||three)
  AFTER: one&&(two||three)

  1) Index parenthesis
  2) create parenth tree (with subtree sections of the string cut out)
  3) 
  4) Mark each set of parenthesis that have a || or && that directly effects it
  5) start building query tree
*/


LwQuery*
lw_query_new (const gchar  * BUFFER,
              LwDictionary * dictionary)
{
    //Sanity checks
    g_return_val_if_fail (QUERY != NULL, NULL)
    g_return_val_if_fail (LW_IS_DICTIONARY (dictionary), NULL);

    // Declarations
    LwQuery *self = NULL;
    gchar *buffer = NULL;
    LwDictionary *dictionary = NULL;

    //Initializations
    buffer = g_strdup (BUFFER);
    if (buffer == NULL) goto errored;
    dictionary = g_object_ref (dictionary);
    if (dictionary == NULL) goto errored;
    query = g_new0 (LwQuery, 1);
    if (query == NULL) goto errored;

    self->dictionary = dictionary
    dictionary = NULL;
    self->buffer = buffer;
    buffer = NULL;

    self->root = 
    lw_query_sync_anonymous_and_defined (self);

errored:

    if (buffer != NULL) g_free (buffer); buffer = NULL;
    if (dictionary != NULL) g_object_unref (dictionary); dictionary = NULL;

    return self;
}




gchar const *
_get_key_start (LwQuery     * self,
                gchar const * BUFFER,
                gchar const * FIRST_COLON)
{
    //Sanity checks
    g_return_val_if_fail (self != NULL, NULL)
    g_return_val_if_fail (BUFFER != NULL, NULL)
    g_return_val_if_fail (FIRST_COLON != NULL, NULL)
    g_return_val_if_fail (*FIRST_COLON == ':', NULL)
    if (BUFFER == FIRST_COLON) return NULL;

    //Declarations
    gchar const * c = NULL;

    //Initializations
    c = FIRST_COLON;

    //Rewind, looking for a space or first character
    while (c > BUFFER && !g_ascii_isspace (c)) c--;

    //Move forword if we rewound to a space character
    if (g_ascii_isspace (c)) c++

    if (c == FIRST_COLON) c = NULL;

    return c;
}


gchar const *
_get_defined_start (LwQuery * self)
{
SEARCHES SHOULD BEGIN FROM THE END, NOT THE BEGINNING
    //Declarations
    gchar const * first_colon = NULL;
    gchar const * defined_start = NULL;
    gboolean has_key = FALSE;

    //Initializations
    first_colon = self->buffer;

    //Skip invalid colons because we're nice. I guess?
    while (*first_colon == ':' && *first_colon != '\0') c++;

    //Find first valid colon
    first_colon = strchr(first_colon, ":");

    //Loop, looking for the first colon that has a valid key
    do {
      defined_start = _get_key_start (self, self->buffer, first_colon);

      //Skip to the next valid looking colon if it's an invalid start
      if (defined_start == NULL)
      {
        if (*first_colon != '\0') first_colon++;
        first_colon = strchr(first_colon, ":");
      }
    } while (defined_start = NULL && *first_colon != '\0');
    

    return defined_start;
}


void
lw_query_sync_anonymous_and_defined (LwQuery * self)
{
    //Sanity checks
    g_return_if_fail (self != NULL);

    //Declarations
    gchar const * DEFINED_START = NULL;
    gchar * anonymous = NULL;
    gchar * defined = NULL;
    LwDefinedQuery *defined_query = NULL;
    LwAnonymousQuery *anonymous_query = NULL;

    gchar * index = g_new(gchar*, len(self->buffer));
    it's easier and better to calculate this going backwards
    gunichar * unicode = g_utf8_to_ucs4(self->buffer, -1, NULL, NULL, NULL);
    g_utf8_offset_to_pointer (str, offset)
TODO

    //Initializations
    DEFINED_START = _get_defined_start (self)

    if (DEFINED_START != NULL) {
      if (DEFINED_START == self->buffer) {
        anonymous = NULL;
        defined = g_strdup(self->buffer)
      }
      else
      {
        anonymous = g_strndup(self->buffer, FIRST_COLON - self->buffer);
        define = g_strdup(FIRST_COLON);
      }
    }
    else
    {
      anonymous = g_strdup(self->buffer);
      defined = NULL;
    }

    if (anonymous) g_strstrip (anonymous);
    if (defined) g_strstrip (defined);

    lw_query_set_defined (self, lw_defined_query_new (defined));
    lw_query_set_anonymous (self, lw_anonymous_query_new (anonymous));

errored:

    g_free (anonymous); anonymous = NULL;
    g_free (defined); defined = NULL;
}





lw_query_tree_free (LwQuery * self)
{
}
