

#define MAX_WORDS  1024
#define MAX_VARS   1024
#define MAX_CALLS  4096
#define MAX_BODY   256
#define STACK_SIZE 256


typedef struct {
  Word   *words;
  u32    words_size;
  Queue  *calls;
  Stack  stack;
  Stack loop_buf;
} Module;

static Module module;
static void executor(Module *m);

#include "words.h"

static Word builtins[] = {
  { "kwpush", (Val[]){ {.fn=bf_push}, {.fn=NULL} } },
  { "kwpushstr", (Val[]){ {.fn=bf_push_str}, {.fn=NULL} } },
  { "dup",    (Val[]){ {.fn=bf_dup }, {.fn=NULL} } },
  { "bye",    (Val[]){ {.fn=bf_bye }, {.fn=NULL} } },
  { ".",      (Val[]){ {.fn=bf_puts}, {.fn=NULL} } },
  { "+",      (Val[]){ {.fn=bf_add }, {.fn=NULL} } },
  { "-",      (Val[]){ {.fn=bf_sub}, {.fn=NULL} } },
  { "*",    (Val[]){ {.fn=bf_mul },  {.fn=NULL} } },
  { "/",    (Val[]){ {.fn=bf_div },  {.fn=NULL} } },
  { "mod",  (Val[]){ {.fn=bf_mod },  {.fn=NULL} } },
  { "=",    (Val[]){ {.fn=bf_eq  },  {.fn=NULL} } },
  { "<",    (Val[]){ {.fn=bf_lt  },  {.fn=NULL} } },
  { ">",    (Val[]){ {.fn=bf_gt  },  {.fn=NULL} } },
  { "drop", (Val[]){ {.fn=bf_drop},  {.fn=NULL} } },
  { "swap", (Val[]){ {.fn=bf_swap},  {.fn=NULL} } },
  { "over", (Val[]){ {.fn=bf_over},  {.fn=NULL} } },
  { "not",  (Val[]){ {.fn=bf_not },  {.fn=NULL} } },
  { "if",     (Val[]){ {.fn=bf_if  }, {.fn=NULL} } },
  { "then",   (Val[]){ {.fn=bf_then}, {.fn=NULL} } },
  { "do",     (Val[]){ {.fn=bf_do  }, {.fn=NULL} } },
  { "loop",   (Val[]){ {.fn=bf_loop}, {.fn=NULL} } },
  { "begin",  (Val[]){ {.fn=bf_begin}, {.fn=NULL} } },
  { "until",  (Val[]){ {.fn=bf_until}, {.fn=NULL} } },
  { ".s",     (Val[]){ {.fn=bf_dot_s}, {.fn=NULL} } },
  { "and",    (Val[]){ {.fn=bf_and   }, {.fn=NULL} } },
  { "or",     (Val[]){ {.fn=bf_or    }, {.fn=NULL} } },
  { "xor",    (Val[]){ {.fn=bf_xor   }, {.fn=NULL} } },
  { "lshift", (Val[]){ {.fn=bf_lshift}, {.fn=NULL} } },
  { "rshift", (Val[]){ {.fn=bf_rshift}, {.fn=NULL} } },
  { "rot",    (Val[]){ {.fn=bf_rot   }, {.fn=NULL} } },
  { "nip",    (Val[]){ {.fn=bf_nip   }, {.fn=NULL} } },
  { "tuck",   (Val[]){ {.fn=bf_tuck  }, {.fn=NULL} } },
  { "2dup",   (Val[]){ {.fn=bf_2dup  }, {.fn=NULL} } },
  { "2drop",  (Val[]){ {.fn=bf_2drop }, {.fn=NULL} } },
  { "abs",    (Val[]){ {.fn=bf_abs   }, {.fn=NULL} } },
  { "min",    (Val[]){ {.fn=bf_min   }, {.fn=NULL} } },
  { "max",    (Val[]){ {.fn=bf_max   }, {.fn=NULL} } },
  { "negate", (Val[]){ {.fn=bf_negate}, {.fn=NULL} } },
  { "cr",     (Val[]){ {.fn=bf_cr    }, {.fn=NULL} } },
  { "space",  (Val[]){ {.fn=bf_space }, {.fn=NULL} } },
  { "emit",   (Val[]){ {.fn=bf_emit  }, {.fn=NULL} } },
  { "puts", (Val[]){ {.fn=bf_puts_str}, {.fn=NULL} } },
  { "@",   (Val[]){ {.fn=bf_fetch}, {.fn=NULL} } },
  { "!",   (Val[]){ {.fn=bf_store}, {.fn=NULL} } },
  { "key", (Val[]){ {.fn=bf_key}, {.fn=NULL} } },
  { NULL,    NULL                                  },
};

static void init_module(void) {
  module.words      = emalloc(MAX_WORDS * sizeof(Word));
  
  Word *b = builtins;
  Word *w = module.words;
  while (b->name)
    *w++ = *b++;

  module.calls = emalloc(sizeof(Queue));
  module.calls->arr = emalloc(MAX_CALLS * sizeof(Val));
  module.calls->tail = module.calls->arr;
  module.calls->head = module.calls->arr;
  module.calls->size = MAX_CALLS;

  module.stack.arr = emalloc(STACK_SIZE * sizeof(Val));
  module.stack.p   = module.stack.arr;
  
  module.loop_buf.arr = emalloc(STACK_SIZE * sizeof(Val));
  module.loop_buf.p   = module.stack.arr;
}
