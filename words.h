
static void 
bf_puts(void) 
{
  Val v = SPOP(&module.stack);
  if (v.f == (f32)(i32)v.f) printf("%d", (i32)v.f);
  else printf("%f", v.f);
  fflush(stdout);
}

static void 
bf_add(void) 
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f + b.f;
  SPUSH(&module.stack, r);
}

static void
bf_sub(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f - b.f;
  SPUSH(&module.stack, r);
}

static void 
bf_dup(void) 
{
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, a);
  SPUSH(&module.stack, a);
}

static void
bf_mul(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f * b.f;
  SPUSH(&module.stack, r);
}

static void
bf_div(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  if (b.f == 0) die("division by zero");
  Val r; r.f = a.f / b.f;
  SPUSH(&module.stack, r);
}

static void
bf_mod(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  if (b.f == 0) die("division by zero");
  Val r; r.f = (f32)((int)a.f % (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_eq(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f == b.f ? 1 : 0;
  SPUSH(&module.stack, r);
}

static void
bf_lt(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f < b.f ? 1 : 0;
  SPUSH(&module.stack, r);
}

static void
bf_gt(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f > b.f ? 1 : 0;
  SPUSH(&module.stack, r);
}

static void
bf_drop(void)
{
  SPOP(&module.stack);
}

static void
bf_swap(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, b);
  SPUSH(&module.stack, a);
}

static void
bf_over(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, a);
  SPUSH(&module.stack, b);
  SPUSH(&module.stack, a);
}

static void
bf_not(void)
{
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f == 0 ? 1 : 0;
  SPUSH(&module.stack, r);
}

static void
bf_push(void)
{
  Val v = qfpop(module.calls);
  DBG("%d\n", (u32)v.f);
  SPUSH(&module.stack, v);
}

static void
bf_push_str(void)
{
  Val v = qfpop(module.calls);
  SPUSH(&module.stack, v);
}

static void
bf_push_ptr(void)
{
  Val v = qfpop(module.calls);
  SPUSH(&module.stack, v);
}

static void
bf_bye(void)
{
  exit(0);
}

static void
bf_then(void) {}

static void
bf_if(void)
{
  Val v = SPOP(&module.stack);
  if (v.f <= 0) {
    while (module.calls->head != module.calls->tail) {
      Val x = qfpop(module.calls);
      if (x.fn == bf_then) break;
    }
  }
}

static void
bf_loop(void) {}

static void
bf_do(void)
{
  Val a = SPOP(&module.stack);
  Val b = SPOP(&module.stack);
  int c = (int)(a.f - b.f);
  Val buf[MAX_BODY];
  Val *p = buf;
  while (module.calls->head != module.calls->tail) {
    Val v = qfpop(module.calls);
    if (v.fn == bf_loop) break;
    if (p >= buf + MAX_BODY) die("body overflow");
    *p++ = v;
  }
  for (Val *q = buf; q < p; q++)
    DBG("buf: %s\n", getfnname(q->fn));
  for (int i = 0; i < c; i++) {
    Val *q = p - 1;
    while (q >= buf) {
      DBG("q=%p buf=%p val=%g\n", (void*)q, (void*)buf, q->f);
      qfpush(module.calls, *q--);
    }
  }
}

static void
bf_dot_s(void)
{
  Val *p = module.stack.arr;
  while (p < module.stack.p)
    printf("%g ", p++->f);
  printf("\n");
}

static void bf_until(void);

static void
bf_begin(void)
{
  module.loop_buf.p = module.calls->head;
}

static void
bf_until(void)
{
  Val v = SPOP(&module.stack);
  DBG("until v=%g\n", v.f);
  if (v.f <= 0)
    module.calls->head = module.loop_buf.p;
}

static void
bf_and(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = (f32)((int)a.f & (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_or(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = (f32)((int)a.f | (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_xor(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = (f32)((int)a.f ^ (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_lshift(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = (f32)((int)a.f << (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_rshift(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = (f32)((int)a.f >> (int)b.f);
  SPUSH(&module.stack, r);
}

static void
bf_rot(void)
{
  Val c = SPOP(&module.stack);
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, b);
  SPUSH(&module.stack, c);
  SPUSH(&module.stack, a);
}

static void
bf_nip(void)
{
  Val b = SPOP(&module.stack);
  SPOP(&module.stack);
  SPUSH(&module.stack, b);
}

static void
bf_tuck(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, b);
  SPUSH(&module.stack, a);
  SPUSH(&module.stack, b);
}

static void
bf_2dup(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  SPUSH(&module.stack, a);
  SPUSH(&module.stack, b);
  SPUSH(&module.stack, a);
  SPUSH(&module.stack, b);
}

static void
bf_2drop(void)
{
  SPOP(&module.stack);
  SPOP(&module.stack);
}

static void
bf_abs(void)
{
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f < 0 ? -a.f : a.f;
  SPUSH(&module.stack, r);
}

static void
bf_min(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f < b.f ? a.f : b.f;
  SPUSH(&module.stack, r);
}

static void
bf_max(void)
{
  Val b = SPOP(&module.stack);
  Val a = SPOP(&module.stack);
  Val r; r.f = a.f > b.f ? a.f : b.f;
  SPUSH(&module.stack, r);
}

static void
bf_negate(void)
{
  Val a = SPOP(&module.stack);
  Val r; r.f = -a.f;
  SPUSH(&module.stack, r);
}

static void
bf_cr(void)
{
  printf("\n");
}

static void
bf_space(void)
{
  printf(" ");
}

static void
bf_emit(void)
{
  Val a = SPOP(&module.stack);
  printf("%c", (char)a.f);
}

static void
bf_puts_str(void)
{
  Val a = SPOP(&module.stack);
  u8 *s = a.s;
  while (*s && *s != '"') printf("%c", *s++);
}

static void
bf_fetch(void)
{
  Val *addr = (Val *)SPOP(&module.stack).p;
  SPUSH(&module.stack, *addr);
}

static void
bf_store(void)
{
  Val *addr = (Val *)SPOP(&module.stack).p;
  *addr = SPOP(&module.stack);
}

static void
bf_key(void)
{
  Val v;
  v.f = (f32)getchar();
  SPUSH(&module.stack, v);
}

