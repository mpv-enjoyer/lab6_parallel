#ifndef DEBUG_OUTPUT_HPP
#define DEBUG_OUTPUT_HPP

#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <iostream>
#if __cplusplus >= 202302L
 #include <format>
 #include <print>
#endif // __cplusplus >= 202302L

int debug_printf(const char* fmt, ...);
int debug_puts(const char* str);
int debug_fputs(const char* str, FILE* stream);
int debug_putc(int ch, FILE* stream);
int debug_fputc(int ch, FILE* stream);
#if __cplusplus >= 202302L
 template <class... Args>
 void debug_println(std::format_string<Args...> fmt, Args&&... args);
 template <class... Args>
 void debug_println(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args);
 template <class... Args>
 void debug_print(std::format_string<Args...> fmt, Args&&... args);
 template <class... Args>
 void debug_print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args);
#endif // __cplusplus >= 202302L

#ifdef DO_DEBUG
 int debug_printf(const char* fmt, ...)
 {
     va_list arg;
     va_start(arg, fmt);
     int result = vprintf(fmt, arg);
     va_end(arg);
     return result;
 }
 int debug_puts(const char* str)
 {
     return puts(str);
 }
 int debug_fputs(const char* str, FILE* stream)
 {
     return fputs(str, stream);
 }
 int debug_putc(int ch, FILE* stream)
 {
     return putc(ch, stream);
 }
 int debug_fputc(int ch, FILE* stream)
 {
     return fputc(ch, stream);
 }
 #define debug_assert(x) assert(x)

 #define DEBUG_PRINTF debug_printf
 #define DEBUG_PUTS debug_puts
 #define DEBUG_FPUTS debug_fputs
 #define DEBUG_PUTC debug_putc
 #define DEBUG_FPUTC debug_fputc
 #define DEBUG_ASSERT assert
 
 #if __cplusplus >= 202302L
  template <class... Args>
  void debug_println(std::format_string<Args...> fmt, Args&&... args)
  {
   std::println(fmt, std::forward<Args>(args)...);
  }
  template <class... Args>
  void debug_println(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
  {
   std::println(stream, fmt, std::forward<Args>(args)...);
  }
  template <class... Args>
  void debug_print(std::format_string<Args...> fmt, Args&&... args)
  {
   std::print(fmt, std::forward<Args>(args)...);
  }
  template <class... Args>
  void debug_print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
  {
   std::print(stream, fmt, std::forward<Args>(args)...);
  }

  #define DEBUG_PRINT debug_print
  #define DEBUG_PRINTLN debug_println
 #endif // __cplusplus >= 202302L

 std::ostream& debug_cout = std::cout;

#else // DO_DEBUG

 int debug_printf(const char* fmt, ...)
 {
     return 0;
 }
 int debug_puts(const char* str)
 {
     return 0;
 }
 int debug_fputs(const char* str, FILE* stream)
 {
     return 0;
 }
 int debug_putc(int ch, FILE* stream)
 {
     return ch;
 }
 int debug_fputc(int ch, FILE* stream)
 {
     return ch;
 }
 #define debug_assert(x) do {(x);} while (0)

 #define DEBUG_PRINTF(...) do {} while (0)
 #define DEBUG_PUTS(...) do {} while (0)
 #define DEBUG_FPUTS(...) do {} while (0)
 #define DEBUG_PUTC(...) do {} while (0)
 #define DEBUG_FPUTC(...) do {} while (0)
 #define DEBUG_ASSERT(...) do {} while (0)

 #if __cplusplus >= 202302L
  template <class... Args>
  void debug_println(std::format_string<Args...> fmt, Args&&... args)
  { }
  template <class... Args>
  void debug_println(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
  { }
  template <class... Args>
  void debug_print(std::format_string<Args...> fmt, Args&&... args)
  { }
  template <class... Args>
  void debug_print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
  { }
  #define DEBUG_PRINTLN(...) do {} while (0)
  #define DEBUG_PRINT(...) do {} while (0)
 #endif // __cplusplus >= 202302L

 // Thanks, Ulrich Eckhardt: https://groups.google.com/g/comp.lang.c++.moderated/c/ggl_2Ii3aVM/m/r3slzSfZ6XkJ
 std::ostream debug_cout(0);

#endif // DO_DEBUG

#endif // DEBUG_OUTPUT_HPP