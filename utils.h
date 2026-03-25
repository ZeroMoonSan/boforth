#pragma once

u8 *
read_file(const char *path)
{
  FILE *f;
  i64 sz;
  u8 *buf;

  if (!path || !(f = fopen(path, "rb"))) 
    return NULL;

  fseek(f, 0, SEEK_END);
  sz = ftell(f);
  rewind(f);

  if (sz < 0) {
    fclose(f);
    return NULL;
  }

  buf = malloc((usz)sz + 1);
  
  if (!buf || fread(buf, 1, (usz)sz, f) != (usz)sz) {
    free(buf);
    fclose(f);
    return NULL;
  }

  fclose(f);
  buf[sz] = '\0'; 
  return buf;
}

void
die(const char *msg)
{
  fprintf(stderr, "\033[31m%s\033[0m\n", msg);
  exit(1);
}

void *
emalloc(size_t size)
{
  void *p;
  if (!(p = malloc(size)))
    die("malloc failed");
  return p;
}

void *
ecalloc(size_t nmemb, size_t size)
{
  void *p;
  if (!(p = calloc(nmemb, size)))
      die("calloc failed");
  return p;
}
