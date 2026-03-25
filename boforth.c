#ifdef _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#endif
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "types.h"
#include "utils.h"
#include "str.h"

#define SPUSH(st, s) *((st)->p++) = s
#define SPOP(st) ((st)->p == (st)->arr ? (die("stack underflow"), (st)->arr[0]) : (*--(st)->p))
#define SSIZE(st) usz size = st.p - st.arr;

#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__); fflush(stdout)
#else
#define DBG(...)
#endif


typedef enum { Int, Float, Str, Call, Pointer } FType;
char *ftype_names[] = { "Int", "Float", "Str", "Call" };

typedef void (*Func)(void);

typedef union {
	i32  i;
	f32  f;
  u8   *s;
  u8   *c;
  void *p;
  Func fn;
} Val;

typedef struct {
  FType type;
  Val   v;
} TVal;

typedef struct {
  Val *arr;
  Val *p;
} Stack;

typedef struct {
  u8 *name;
  Val *body;  
} Word;

typedef struct {
  Val *arr;
  Val *head;
  Val *tail;
  int  size;
} Queue;

typedef void (*Emit)(Val);

typedef struct {
  u8  *val;
  u32  line;
} Token;


// func declarations
static u8** tokenize(u8 *s);
static void parsecode(u8 **ts, Emit emit);
static TVal parsetoken(u8 *t);
static void qpush(Queue *q, Val v);
static void qfpush(Queue *q, Val v);
static Val qfpop(Queue *q);
#ifdef DEBUG
static u8* getfnname(Func fn);
#endif
static void addvar(u8 *name);
// vars
static Val *emit_buf;

#include "cfg.h"

// func impls

#ifdef DEBUG
u8 *
getfnname(Func fn)
{
  Word *w = module.words;
  while (w->name) {
    if (w->body[0].fn == fn) return w->name;
    w++;
  }
  return (u8 *)"?";
}
#endif

void
addvar(u8 *name)
{
  Val *mem = emalloc(sizeof(Val));
  mem->f = 0;
  Word *w = module.words;
  while (w->name) w++;
  w->name = name;
  w->body = emalloc(3 * sizeof(Val));
  w->body[0].fn = bf_push_ptr;
  w->body[1].p  = mem;
  w->body[2].fn = NULL;
}

void
qpush(Queue *q, Val v)
{
  *q->tail++ = v;
  if (q->tail >= q->arr + q->size) q->tail = q->arr;
}

void
qfpush(Queue *q, Val v)
{
  if (q->head == q->arr) q->head = q->arr + q->size;
  --q->head;
  if (q->head == q->tail) die("queue overflow");
  *q->head = v;
}

Val
qfpop(Queue *q)
{
  Val v = *q->head++;
  if (q->head >= q->arr + q->size) q->head = q->arr;
  return v;
}

// заменяет пробелы на \0 и создаёт список указателей
u8 **
tokenize(u8 *s)
{
  static u8 *ts[MAX_WORDS];
  u32 n = 0;

  while (*(s = skip_gap(s))) {
    ts[n++] = s;
    s = find_end(s);

    if (*s) {
      *s = '\0';
      s++;
    }

    if (n >= MAX_WORDS - 1) break;
  }

  ts[n] = NULL;
  return ts;
}

TVal
parsetoken(u8 *t)
{
  TVal tv;
  if (*t == '"') {
    tv.type = Str;
    t++;
    tv.v.s = t;
    STREND(t);
    t--;
    if (*t == '"') *t = '\0';
    return tv;
  }
  u8 *p = t;
  tv.type = Int;
  if (*p == '-') p++;
  while (*p) {
    if (((*p == '.') && (tv.type = Float))
        || (!(*p >= '0' && *p <= '9') && (tv.type = Call))) break;
    p++;
  }
  if (*t == '.') tv.type = Call;
  if (*t == '-' && p == t + 1) tv.type = Call;
  if      (tv.type == Int)   tv.v.f = (f32)stoi(t);
  else if (tv.type == Float) tv.v.f = stof(t);
  else                       tv.v.s = t;
  return tv;
}

static void
emit_queue(Val v)
{
  qpush(module.calls, v);
}

static void
emit_body(Val v)
{
  *emit_buf++ = v;
}

void
addword(u8 **p)
{
  Word *w = module.words;
  while (w->name) w++;
  w->name = *p++;
  emit_buf = w->body = emalloc(MAX_BODY * sizeof(Val));
  parsecode(p, emit_body);
  emit_buf->fn = NULL;
}

void
parsecode(u8 **ts, Emit emit)
{
  
  for (; *ts && !streq(*ts, ";"); ts++) {
    TVal tv = parsetoken(*ts);
    DBG("t: %s T=%d\n", *ts, tv.type);
    switch (tv.type) {
      case Int:
      case Float: {
        Val v;
        v.fn = bf_push;
        emit(v);
        emit(tv.v);
        break;
      }
      case Str: {
        Val v;
        v.fn = bf_push_str;
        emit(v);
        emit(tv.v);
        break;
      }
      case Call: {
        if (streq(*ts, ":")) {
          ts++;
          addword(ts);
          while (*ts && !streq(*ts, ";")) ts++;
          break;
        } if (streq(*ts, "variable") || streq(*ts, "var")) {
          ts++;
          addvar(*ts);
          break;
        }

        Word *w = module.words;
        while (w->name) {
          if (streq(w->name, *ts)) {
            Val *b = w->body;
            while (b->fn) { emit(*b); b++; }
            break;
          }
          w++;
        }
        if (!w->name) { printf("%s", *ts); die("unknown word"); }
        
        break;
      }
      case Pointer:
        break;
    }
  }
}

void
executor(Module *m)
{
  while (1) {
    if (m->calls->head == m->calls->tail) { usleep(1000); continue; }
    Val v = qfpop(m->calls);
    if (!v.fn) die("expected func");
#ifdef DEBUG
    u8* n = getfnname(v.fn); 
#endif
    DBG("FN:%s\n", n);
    v.fn();
  }
}

int
main(int argc, char **argv)
{
  if (argc < 2) return 1;

  u8 *s = read_file(argv[1]);
  if (!s) die("file not found");

  
  u8** ts = tokenize(s);
  
  init_module();
  if (!module.calls || !module.calls->arr) die("init failed");
  
  parsecode(ts, emit_queue);
  executor(&module);

  free(s);
}
