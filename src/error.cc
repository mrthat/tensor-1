
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern bool              human_readable;
extern verbosity::type_t noisiness;
extern char              *tool_name;
extern bool              tracing;
extern bool              verbose;

void
verror(uint levels, char const *format, va_list args)
{
  bool show;
  
  show = verbose || !(levels & (level::information | level::debug));
  if (show) {
    if (!(levels & (level::progress | level::trace))) {
      fprintf(stderr, "%s: ", tool_name);
    }
    if (levels & level::information) {
      fprintf(stderr, "INFORMATION: ");
    } else if (levels & level::debug) {
      fprintf(stderr, "DEBUG: ");
    } else if (levels & level::warning) {
      fprintf(stderr, "WARNING: ");
    } else if (levels & level::error) {
      fprintf(stderr, "ERROR: ");
    }
    vfprintf(stderr, format, args);
  }
  if (levels & level::fatal) {
    exit(levels);
  }
}

void
vmessage(char const *format, va_list args)
{
  vfprintf(stdout, format, args);
}

void
message(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  vmessage(format, args);
  va_end(args);
}

void
progress(char const *format, ...)
{
  va_list args;
  
  if (!human_readable) {
    return;
  }
  
  va_start(args, format);
  verror(level::progress, format, args);
  va_end(args);
}

void
information(char const *format, ...)
{
  va_list args;

  va_start(args, format);
  verror(level::information, format, args);
  va_end(args);
}

void
trace(char const *format, ...)
{
  va_list args;
  
  if (!tracing) {
    return;
  }
  
  va_start(args, format);
  verror(level::trace, format, args);
  va_end(args);
}

void
debug(char const *format, ...)
{
  va_list args;

  va_start(args, format);
  verror(level::debug, format, args);
  va_end(args);
}

void
debug(verbosity::type_t noise, char const *format, ...)
{
  va_list args;
  
  if (noise <= noisiness) {
    va_start(args, format);
    verror(level::debug, format, args);
    va_end(args);
  }
}

void
superfluous(char const *format, ...)
{
  va_list args;
  
  if (noisiness < verbosity::max) {
    return;
  }
  
  va_start(args, format);
  verror(level::debug, format, args);
  va_end(args);
}

void
warning(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(level::warning, format, args);
  va_end(args);
}

void
error(uint levels, char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(levels, format, args);
  va_end(args);
}

void
die(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  verror(level::error|level::fatal, format, args);
  va_end(args);
}

