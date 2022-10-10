#ifndef _STDARG_H_
#define _STDARG_H_

typedef __builtin_va_list va_list;

#define va_start(arg, last) (__builtin_va_start(arg, last))
#define va_arg(arg, type) (__builtin_va_arg(arg, type))
#define va_end(arg) (__builtin_va_end(arg))

#endif
