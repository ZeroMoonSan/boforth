#pragma once

#define STREND(s) while (*s) s++;

static u8*
skip_gap(u8 *s)
{
  while (*s && *s <= ' ') s++;
  if (*s == '\\') {
    while (*s && *s != '\n') s++;
    return skip_gap(s);
  }
  if (*s == '(') {
    while (*s && *s != ')') s++;
    if (*s) s++;
    return skip_gap(s);
  }
  return s;
}

static u8*
find_end(u8 *s)
{
  if (*s == '"') {
    s++;
    while (*s && *s != '"') s++;
    if (*s) s++;
    return s;
  }
  while (*s && *s > ' ') s++;
  return s;
}

static i32
stoi(u8 *t)
{
  i32 sign = 1;
  if (*t == '-') { sign = -1; t++; }
  i32 n = 0;
  while (*t >= '0' && *t <= '9')
    n = n * 10 + (*t++ - '0');
  return n * sign;
}

static f32
stof(u8 *t)
{
  int neg = 0;
  if (*t == '-') { neg = 1; t++; }
  f32 n = (f32)stoi(t);
  while (*t && *t != '.') t++;
  if (!*t) return neg ? -n : n;
  t++;
  f32 d = 0.1f;
  while (*t >= '0' && *t <= '9') {
    n += (*t++ - '0') * d;
    d *= 0.1f;
  }
  return neg ? -n : n;
}

bool
streq(u8 *a, u8 *b)
{
  while (*a && *b)
    if (*a++ != *b++) return 0;
  return *a == *b;
}
