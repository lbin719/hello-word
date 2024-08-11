///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2014-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Tiny printf, sprintf and (v)snprintf implementation, optimized for speed on
//        embedded systems with a very limited resources. These routines are thread
//        safe and reentrant!
//        Use this instead of the bloated standard/newlib printf cause these use
//        malloc for printf (and may not be thread safe).
//
//  History:
//  ================================================================
//   2021-06-14:
//   1. porting orginal file
//   2. disable printf and vprintf_ in case of deflecting the ulog module by setting macro PRINTF_FUNC_AVAILABLE to 0
//   3. sprintf is not defined in the file since redirect has been taken place in libs in complier folder. sprintf is modified to sprintf_ in the file
//   4. add test func which can be available by setting macro TEST_PRINTF to 1

//   2021-12-9:
//   1. %m.ng的n指的是有效数字
//      修改前默认是大于1e-4小于1e6使用%f打印，否则使用%e
//      修改后，根据实际的有效数字计算精度比如%1.14g，则小于1e14都按照%f打印
//
//   2. 浮点打印最大支持从小于1e9增大到小于1e17，以便支持double型
//
//   3. 按照规范，当使用%g时，将去除小数部分的无效0，当没有小数部分时，去除小数点
//      any trailing zeros should be removed from the fractional portion of the result
//      and the decimal-point character should be removed if there is no fractional portion remaining
//      when using %g
//
//   4. 使用%g打印0时，按照%f打印
//
//   注意：
//    1. %e打印超大数(超过uint32的)，最后位可能会有误差
//    2. 使用%g时，buffer要足够大——最大有效数字 + 2(小数点和休止符)
//    比如打印double d = 1.12
//    SPRINTF(buffer, "%1.12g", d);
//    buffer的size至少要12 + 2个字节，否则会有越界风险

#define HMI_CORE_SPRINTF_REDIRECT 1

// ///////////////////////////////////////////////////////////////////////////////
#if HMI_CORE_SPRINTF_REDIRECT ||  HMI_CORE_MM_OV_DET

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#if HMI_CORE_MM_OV_DET
#include "cmsis_os2.h"
#include "h_mm.h"
#endif


#define PRINTF_FUNC_AVAILABLE 0 // vprintf_ printf is not defined in the file when set to 0
#if HMI_CORE_SPRINTF_REDIRECT
#define SPRINTF_FUNC_DEFINED_IN_OTHER_FLIE 0 // use sprintf defined in this C file.
#else
#define SPRINTF_FUNC_DEFINED_IN_OTHER_FLIE 1
#endif
#define TEST_PRINTF 0

// to support double float
#define PRINTF_MAX_FLOAT  1e17

// define this globally (e.g. gcc -DPRINTF_INCLUDE_CONFIG_H ...) to include the
// printf_config.h header file
// default: undefined
#ifdef PRINTF_INCLUDE_CONFIG_H
#include "printf_config.h"
#endif


// 'ntoa' conversion buffer size, this must be big enough to hold one converted
// numeric number including padded zeros (dynamically created on stack)
// default: 32 byte
#ifndef PRINTF_NTOA_BUFFER_SIZE
#define PRINTF_NTOA_BUFFER_SIZE    32U
#endif

// 'ftoa' conversion buffer size, this must be big enough to hold one converted
// float number including padded zeros (dynamically created on stack)
// default: 32 byte
#ifndef PRINTF_FTOA_BUFFER_SIZE
#define PRINTF_FTOA_BUFFER_SIZE    32U
#endif

// support for the floating point type (%f)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_FLOAT
#define PRINTF_SUPPORT_FLOAT
#endif

// support for exponential floating point notation (%e/%g)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_SUPPORT_EXPONENTIAL
#endif

// define the default floating point precision
// default: 6 digits
#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
#define PRINTF_DEFAULT_FLOAT_PRECISION  6U
#endif

// define the largest float suitable to print with %f
// default: 1e9
#ifndef PRINTF_MAX_FLOAT
#define PRINTF_MAX_FLOAT  1e9
#endif

// support for the long long types (%llu or %p)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_SUPPORT_LONG_LONG
#endif

// support for the ptrdiff_t type (%t)
// ptrdiff_t is normally defined in <stddef.h> as long or long long type
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_PTRDIFF_T
#define PRINTF_SUPPORT_PTRDIFF_T
#endif

///////////////////////////////////////////////////////////////////////////////

// internal flag definitions
#define FLAGS_ZEROPAD   (1U <<  0U)
#define FLAGS_LEFT      (1U <<  1U)
#define FLAGS_PLUS      (1U <<  2U)
#define FLAGS_SPACE     (1U <<  3U)
#define FLAGS_HASH      (1U <<  4U)
#define FLAGS_UPPERCASE (1U <<  5U)
#define FLAGS_CHAR      (1U <<  6U)
#define FLAGS_SHORT     (1U <<  7U)
#define FLAGS_LONG      (1U <<  8U)
#define FLAGS_LONG_LONG (1U <<  9U)
#define FLAGS_PRECISION (1U << 10U)
#define FLAGS_ADAPT_EXP (1U << 11U)


// import float.h for DBL_MAX
#if defined(PRINTF_SUPPORT_FLOAT)
#include <float.h>
#endif


// output function type
typedef void (*out_fct_type)(char character, void* buffer, size_t idx, size_t maxlen);


// wrapper (used as buffer) for output function type
typedef struct {
  void  (*fct)(char character, void* arg);
  void* arg;
} out_fct_wrap_type;


// internal buffer output
static inline void _out_buffer(char character, void* buffer, size_t idx, size_t maxlen)
{
  if (idx < maxlen) {
    ((char*)buffer)[idx] = character;
  }
}


// internal null output
static inline void _out_null(char character, void* buffer, size_t idx, size_t maxlen)
{
  (void)character; (void)buffer; (void)idx; (void)maxlen;
}


// internal _putchar wrapper
static inline void _out_char(char character, void* buffer, size_t idx, size_t maxlen)
{
  (void)buffer; (void)idx; (void)maxlen;
  if (character) {
#if PRINTF_FUNC_AVAILABLE == 1
    #warning output func must be defined
#else
    (void)character;
#endif
  }
}


// internal output function wrapper
static inline void _out_fct(char character, void* buffer, size_t idx, size_t maxlen)
{
  (void)idx; (void)maxlen;
  if (character) {
    // buffer is the output fct pointer
    ((out_fct_wrap_type*)buffer)->fct(character, ((out_fct_wrap_type*)buffer)->arg);
  }
}


// internal secure strlen
// \return The length of the string (excluding the terminating 0) limited by 'maxsize'
static inline unsigned int _strnlen_s(const char* str, size_t maxsize)
{
  const char* s;
  for (s = str; *s && maxsize--; ++s);
  return (unsigned int)(s - str);
}


// internal test if char is a digit (0-9)
// \return true if char is a digit
static inline bool _is_digit(char ch)
{
  return (ch >= '0') && (ch <= '9');
}


// internal ASCII string to unsigned int conversion
static unsigned int _atoi(const char** str)
{
  unsigned int i = 0U;
  while (_is_digit(**str)) {
    i = i * 10U + (unsigned int)(*((*str)++) - '0');
  }
  return i;
}


// output the specified string in reverse, taking care of any zero-padding
static size_t _out_rev(out_fct_type out, char* buffer, size_t idx, size_t maxlen, const char* buf, size_t len, unsigned int width, unsigned int flags)
{
  const size_t start_idx = idx;

  // pad spaces up to given width
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    for (size_t i = len; i < width; i++) {
      out(' ', buffer, idx++, maxlen);
    }
  }

  // reverse string
  while (len) {
    out(buf[--len], buffer, idx++, maxlen);
  }

  // append pad spaces up to given width
  if (flags & FLAGS_LEFT) {
    while (idx - start_idx < width) {
      out(' ', buffer, idx++, maxlen);
    }
  }

  return idx;
}


// internal itoa format
static size_t _ntoa_format(out_fct_type out, char* buffer, size_t idx, size_t maxlen, char* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags)
{
  // pad leading zeros
  if (!(flags & FLAGS_LEFT)) {
    if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
    while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  // handle hash
  if (flags & FLAGS_HASH) {
    if (!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width))) {
      len--;
      if (len && (base == 16U)) {
        len--;
      }
    }
    if ((base == 16U) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'x';
    }
    else if ((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'X';
    }
    else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'b';
    }
    if (len < PRINTF_NTOA_BUFFER_SIZE) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_NTOA_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}


// internal itoa for 'long' type
static size_t _ntoa_long(out_fct_type out, char* buffer, size_t idx, size_t maxlen, unsigned long value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags)
{
  char buf[PRINTF_NTOA_BUFFER_SIZE];
  size_t len = 0U;

  // no hash for 0 values
  if (!value) {
    flags &= ~FLAGS_HASH;
  }

  // write if precision != 0 and value is != 0
  if (!(flags & FLAGS_PRECISION) || value) {
    do {
      const char digit = (char)(value % base);
      buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
      value /= base;
    } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
  }

  return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}


// internal itoa for 'long long' type
#if defined(PRINTF_SUPPORT_LONG_LONG)
static size_t _ntoa_long_long(out_fct_type out, char* buffer, size_t idx, size_t maxlen, unsigned long long value, bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags)
{
  char buf[PRINTF_NTOA_BUFFER_SIZE];
  size_t len = 0U;

  // no hash for 0 values
  if (!value) {
    flags &= ~FLAGS_HASH;
  }

  // write if precision != 0 and value is != 0
  if (!(flags & FLAGS_PRECISION) || value) {
    do {
      const char digit = (char)(value % base);
      buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
      value /= base;
    } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
  }

  return _ntoa_format(out, buffer, idx, maxlen, buf, len, negative, (unsigned int)base, prec, width, flags);
}
#endif  // PRINTF_SUPPORT_LONG_LONG


#if defined(PRINTF_SUPPORT_FLOAT)

#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// forward declaration so that _ftoa can switch to exp notation for values > PRINTF_MAX_FLOAT
static size_t _etoa(out_fct_type out, char* buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags);
#endif


// internal ftoa for fixed decimal floating point
static size_t _ftoa(out_fct_type out, char* buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags)
{
  char buf[PRINTF_FTOA_BUFFER_SIZE];
  size_t len  = 0U;
  double diff = 0.0;

  // powers of 10
  static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
                                  10000000000,
                                  100000000000,
                                  1000000000000,
                                  10000000000000,
                                  100000000000000,
                                  1000000000000000,
                                  10000000000000000,
                                  100000000000000000};

  // test for special values
  if (value != value)
    return _out_rev(out, buffer, idx, maxlen, "nan", 3, width, flags);
  if (value < -DBL_MAX)
    return _out_rev(out, buffer, idx, maxlen, "fni-", 4, width, flags);
  if (value > DBL_MAX)
    return _out_rev(out, buffer, idx, maxlen, (flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);

  // test for very large values
  // standard printf behavior is to print EVERY whole number digit -- which could be 100s of characters overflowing your buffers == bad
  if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT)) {
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
    return _etoa(out, buffer, idx, maxlen, value, prec, width, flags);
#else
    return 0U;
#endif
  }

  // test for negative
  bool negative = false;
  if (value < 0) {
    negative = true;
    value = 0 - value;
  }

  // set default precision, if not set explicitly
  if (!(flags & FLAGS_PRECISION)) {
    prec = PRINTF_DEFAULT_FLOAT_PRECISION;
  }
  // limit precision to 9, cause a prec >= 18 can lead to overflow errors
  while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 17U)) {
    buf[len++] = '0';
    prec--;
  }

  // int64_t to support double float
  int64_t whole = (int64_t)value;
  double tmp = (value - whole) * pow10[prec];
  uint64_t frac = (uint64_t )tmp;
  diff = tmp - frac;

  if (diff > 0.5) {
    ++frac;
    // handle rollover, e.g. case 0.99 with prec 1 is 1.0
    if (frac >= pow10[prec]) {
      frac = 0;
      ++whole;
    }
  }
  else if (diff < 0.5) {
  }
  else if ((frac == 0U) || (frac & 1U)) {
    // if halfway, round up if odd OR if last digit is 0
    ++frac;
  }

  if (prec == 0U) {
    diff = value - (double)whole;
    if ((diff > 0.5) && (whole & 1)) {
      // exactly 0.5 and ODD, then round up
      // 1.5 -> 2, but 2.5 -> 2
      ++whole;
    }
  }
  else {
    unsigned int count = prec;
    // now do fractional part, as an unsigned number
    while (len < PRINTF_FTOA_BUFFER_SIZE) {
      --count;
      buf[len++] = (char)(48U + (frac % 10U));
      if (!(frac /= 10U)) {
        break;
      }
    }
    // add extra 0s
    while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U)) {
      buf[len++] = '0';
    }
    if (len < PRINTF_FTOA_BUFFER_SIZE) {
      // add decimal
      buf[len++] = '.';
    }
  }

  // do whole part, number is reversed
  while (len < PRINTF_FTOA_BUFFER_SIZE) {
    buf[len++] = (char)(48 + (whole % 10));
    if (!(whole /= 10)) {
      break;
    }
  }

  // pad leading zeros
  if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
    if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_FTOA_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  return _out_rev(out, buffer, idx, maxlen, buf, len, width, flags);
}


#if defined(PRINTF_SUPPORT_EXPONENTIAL)
// internal ftoa variant for exponential floating-point type, contributed by Martijn Jasperse <m.jasperse@gmail.com>
static size_t _etoa(out_fct_type out, char* buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags)
{
  // check for NaN and special values
  if ((value != value) || (value > DBL_MAX) || (value < -DBL_MAX)) {
    return _ftoa(out, buffer, idx, maxlen, value, prec, width, flags);
  }

  // determine the sign
  const bool negative = value < 0;
  if (negative) {
    value = -value;
  }

  // default precision
  if (!(flags & FLAGS_PRECISION)) {
    prec = PRINTF_DEFAULT_FLOAT_PRECISION;
  }

  uint64_t max_le_value = 1;
  unsigned int tmp_prec = prec;

  // "prec" is the number of *significant figures* not decimals
  // 精度是有效位，不是小数位
  // 根据有效位判断最大值，以便是否使用科学计数
  while (tmp_prec)
  {
      max_le_value *= 10;
      tmp_prec--;
  }

  // determine the decimal exponent
  // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
  union {
    uint64_t U;
    double   F;
  } conv;

  conv.F = value;
  int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;           // effectively log2
  conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U);  // drop the exponent so conv.F is now in [1,2)
  // now approximate log10 from the log2 integer part and an expansion of ln around 1.5
  int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
  // now we want to compute 10^expval but we want to be sure it won't overflow
  exp2 = (int)(expval * 3.321928094887362 + 0.5);
  const double z  = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
  const double z2 = z * z;
  conv.U = (uint64_t)(exp2 + 1023) << 52U;
  // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
  conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
  // correct for rounding errors
  if (value < conv.F) {
    expval--;
    conv.F /= 10;
  }

  // the exponent format is "%+03d" and largest value is "307", so set aside 4-5 characters
  unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

  // in "%g" mode, "prec" is the number of *significant figures* not decimals
  if (flags & FLAGS_ADAPT_EXP) {
    // do we want to fall-back to "%f" mode?
    if (((value >= 1e-4) && (value < max_le_value)) || (expval < -307)) { // // 指数小于-307 value为0
      if ((int)prec > expval) {
        prec = (unsigned)((int)prec - expval - 1);
      }
      else {
        prec = 0;
      }

      if (expval < -307) // %g 打印0
      {
          prec = 0;     // 不需要小数
      }

      flags |= FLAGS_PRECISION;   // make sure _ftoa respects precision
      // no characters in exponent
      minwidth = 0U;
      expval   = 0;
    }
    else {
      // we use one sigfig for the whole part
      if ((prec > 0) && (flags & FLAGS_PRECISION)) {
        --prec;
      }
    }
  }

  // will everything fit?
  unsigned int fwidth = width;
  if (width > minwidth) {
    // we didn't fall-back so subtract the characters required for the exponent
    fwidth -= minwidth;
  } else {
    // not enough characters, so go back to default sizing
    fwidth = 0U;
  }
  if ((flags & FLAGS_LEFT) && minwidth) {
    // if we're padding on the right, DON'T pad the floating part
    fwidth = 0U;
  }

  // rescale the float value
  if (expval) {
    value /= conv.F;
  }

  // output the floating part
  const size_t start_idx = idx;
  idx = _ftoa(out, buffer, idx, maxlen, negative ? -value : value, prec, fwidth, flags & ~FLAGS_ADAPT_EXP);

  // any trailing zeros should be removed from the fractional portion of the result
  // and the decimal-point character should removed if there is no fractional portion remaining
  // when using %g
  if (flags & FLAGS_ADAPT_EXP)
  {
      for (int i = idx - 1; i > 0; i--)
      {
          if (buffer[i] == '0')
          {
              buffer[i] = 0;
              idx--;
          }
          else if (buffer[i] == '.')
          {
              buffer[i] = 0;
              idx--;
              break;
          }
          else
          {
              break;
          }
      }
  }

  // output the exponent part
  if (minwidth) {
    // output the exponential symbol
    out((flags & FLAGS_UPPERCASE) ? 'E' : 'e', buffer, idx++, maxlen);
    // output the exponent value
    idx = _ntoa_long(out, buffer, idx, maxlen, (expval < 0) ? -expval : expval, expval < 0, 10, 0, minwidth-1, FLAGS_ZEROPAD | FLAGS_PLUS);
    // might need to right-pad spaces
    if (flags & FLAGS_LEFT) {
      while (idx - start_idx < width) out(' ', buffer, idx++, maxlen);
    }
  }
  return idx;
}
#endif  // PRINTF_SUPPORT_EXPONENTIAL
#endif  // PRINTF_SUPPORT_FLOAT


// internal vsnprintf
static int _vsnprintf(out_fct_type out, char* buffer, const size_t maxlen, const char* format, va_list va)
{
  unsigned int flags, width, precision, n;
  size_t idx = 0U;

  if (!buffer) {
    // use null output function
    out = _out_null;
  }

  while (*format)
  {
    // format specifier?  %[flags][width][.precision][length]
    if (*format != '%') {
      // no
      out(*format, buffer, idx++, maxlen);
      format++;
      continue;
    }
    else {
      // yes, evaluate it
      format++;
    }

    // evaluate flags
    flags = 0U;
    do {
      switch (*format) {
        case '0': flags |= FLAGS_ZEROPAD; format++; n = 1U; break;
        case '-': flags |= FLAGS_LEFT;    format++; n = 1U; break;
        case '+': flags |= FLAGS_PLUS;    format++; n = 1U; break;
        case ' ': flags |= FLAGS_SPACE;   format++; n = 1U; break;
        case '#': flags |= FLAGS_HASH;    format++; n = 1U; break;
        default :                                   n = 0U; break;
      }
    } while (n);

    // evaluate width field
    width = 0U;
    if (_is_digit(*format)) {
      width = _atoi(&format);
    }
    else if (*format == '*') {
      const int w = va_arg(va, int);
      if (w < 0) {
        flags |= FLAGS_LEFT;    // reverse padding
        width = (unsigned int)-w;
      }
      else {
        width = (unsigned int)w;
      }
      format++;
    }

    // evaluate precision field
    precision = 0U;
    if (*format == '.') {
      flags |= FLAGS_PRECISION;
      format++;
      if (_is_digit(*format)) {
        precision = _atoi(&format);
      }
      else if (*format == '*') {
        const int prec = (int)va_arg(va, int);
        precision = prec > 0 ? (unsigned int)prec : 0U;
        format++;
      }
    }

    // evaluate length field
    switch (*format) {
      case 'l' :
        flags |= FLAGS_LONG;
        format++;
        if (*format == 'l') {
          flags |= FLAGS_LONG_LONG;
          format++;
        }
        break;
      case 'h' :
        flags |= FLAGS_SHORT;
        format++;
        if (*format == 'h') {
          flags |= FLAGS_CHAR;
          format++;
        }
        break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
      case 't' :
        flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        format++;
        break;
#endif
      case 'j' :
        flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        format++;
        break;
      case 'z' :
        flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        format++;
        break;
      default :
        break;
    }

    // evaluate specifier
    switch (*format) {
      case 'd' :
      case 'i' :
      case 'u' :
      case 'x' :
      case 'X' :
      case 'o' :
      case 'b' : {
        // set the base
        unsigned int base;
        if (*format == 'x' || *format == 'X') {
          base = 16U;
        }
        else if (*format == 'o') {
          base =  8U;
        }
        else if (*format == 'b') {
          base =  2U;
        }
        else {
          base = 10U;
          flags &= ~FLAGS_HASH;   // no hash for dec format
        }
        // uppercase
        if (*format == 'X') {
          flags |= FLAGS_UPPERCASE;
        }

        // no plus or space flag for u, x, X, o, b
        if ((*format != 'i') && (*format != 'd')) {
          flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
        }

        // ignore '0' flag when precision is given
        if (flags & FLAGS_PRECISION) {
          flags &= ~FLAGS_ZEROPAD;
        }

        // convert the integer
        if ((*format == 'i') || (*format == 'd')) {
          // signed
          if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
            const long long value = va_arg(va, long long);
            idx = _ntoa_long_long(out, buffer, idx, maxlen, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
#endif
          }
          else if (flags & FLAGS_LONG) {
            const long value = va_arg(va, long);
            idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
          }
          else {
            const int value = (flags & FLAGS_CHAR) ? (char)va_arg(va, int) : (flags & FLAGS_SHORT) ? (short int)va_arg(va, int) : va_arg(va, int);
            idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
          }
        }
        else {
          // unsigned
          if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
            idx = _ntoa_long_long(out, buffer, idx, maxlen, va_arg(va, unsigned long long), false, base, precision, width, flags);
#endif
          }
          else if (flags & FLAGS_LONG) {
            idx = _ntoa_long(out, buffer, idx, maxlen, va_arg(va, unsigned long), false, base, precision, width, flags);
          }
          else {
            const unsigned int value = (flags & FLAGS_CHAR) ? (unsigned char)va_arg(va, unsigned int) : (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(va, unsigned int) : va_arg(va, unsigned int);
            idx = _ntoa_long(out, buffer, idx, maxlen, value, false, base, precision, width, flags);
          }
        }
        format++;
        break;
      }
#if defined(PRINTF_SUPPORT_FLOAT)
      case 'f' :
      case 'F' :
        if (*format == 'F') flags |= FLAGS_UPPERCASE;
        idx = _ftoa(out, buffer, idx, maxlen, va_arg(va, double), precision, width, flags);
        format++;
        break;
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
      case 'e':
      case 'E':
      case 'g':
      case 'G':
        if ((*format == 'g')||(*format == 'G')) flags |= FLAGS_ADAPT_EXP;
        if ((*format == 'E')||(*format == 'G')) flags |= FLAGS_UPPERCASE;
        idx = _etoa(out, buffer, idx, maxlen, va_arg(va, double), precision, width, flags);
        format++;
        break;
#endif  // PRINTF_SUPPORT_EXPONENTIAL
#endif  // PRINTF_SUPPORT_FLOAT
      case 'c' : {
        unsigned int l = 1U;
        // pre padding
        if (!(flags & FLAGS_LEFT)) {
          while (l++ < width) {
            out(' ', buffer, idx++, maxlen);
          }
        }
        // char output
        out((char)va_arg(va, int), buffer, idx++, maxlen);
        // post padding
        if (flags & FLAGS_LEFT) {
          while (l++ < width) {
            out(' ', buffer, idx++, maxlen);
          }
        }
        format++;
        break;
      }

      case 's' : {
        const char* p = va_arg(va, char*);
        if (!p) {
          p = "<NULL>";
        }
        unsigned int l = _strnlen_s(p, precision ? precision : (size_t)-1);
        // pre padding
        if (flags & FLAGS_PRECISION) {
          l = (l < precision ? l : precision);
        }
        if (!(flags & FLAGS_LEFT)) {
          while (l++ < width) {
            out(' ', buffer, idx++, maxlen);
          }
        }
        // string output
        while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--)) {
          out(*(p++), buffer, idx++, maxlen);
        }
        // post padding
        if (flags & FLAGS_LEFT) {
          while (l++ < width) {
            out(' ', buffer, idx++, maxlen);
          }
        }
        format++;
        break;
      }

      case 'p' : {
        width = sizeof(void*) * 2U;
        flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
#if defined(PRINTF_SUPPORT_LONG_LONG)
        const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
        if (is_ll) {
          idx = _ntoa_long_long(out, buffer, idx, maxlen, (uintptr_t)va_arg(va, void*), false, 16U, precision, width, flags);
        }
        else {
#endif
          idx = _ntoa_long(out, buffer, idx, maxlen, (unsigned long)((uintptr_t)va_arg(va, void*)), false, 16U, precision, width, flags);
#if defined(PRINTF_SUPPORT_LONG_LONG)
        }
#endif
        format++;
        break;
      }

      case '%' :
        out('%', buffer, idx++, maxlen);
        format++;
        break;

      default :
        out(*format, buffer, idx++, maxlen);
        format++;
        break;
    }
  }

  // termination
  out((char)0, buffer, idx < maxlen ? idx : maxlen - 1U, maxlen);

  // return written chars without terminating \0
  return (int)idx;
}


///////////////////////////////////////////////////////////////////////////////


int printf(const char* format, ...)
{
  (void)format;

#if PRINTF_FUNC_AVAILABLE == 1
  va_list va;
  va_start(va, format);
  char buffer[1];
  const int ret = _vsnprintf(_out_char, buffer, (size_t)-1, format, va);
  va_end(va);
  return ret;
#else
  return 0;
#endif
}


#if SPRINTF_FUNC_DEFINED_IN_OTHER_FLIE == 1
int sprintf_(char* buffer, const char* format, ...)
#else
int sprintf(char* buffer, const char* format, ...)
#endif
{
  va_list va;
  va_start(va, format);
  const int ret = _vsnprintf(_out_buffer, buffer, (size_t)-1, format, va);
  va_end(va);
  return ret;
}



int snprintf(char* buffer, size_t count, const char* format, ...)
{
#if HMI_CORE_MM_OV_DET
    OS_ASSERT(os_heap_mem_validate(buffer, count) == true); // check if overflow when located in heap
#endif
    va_list va;
    va_start(va, format);
    const int ret = _vsnprintf(_out_buffer, buffer, count, format, va);
    va_end(va);
    return ret;
}


int vprintf_(const char* format, va_list va)
{
  (void)format;
  (void)va;

#if PRINTF_FUNC_AVAILABLE == 1
  char buffer[1];
  return _vsnprintf(_out_char, buffer, (size_t)-1, format, va);
#else
  return 0;
#endif
}


int vsnprintf(char* buffer, size_t count, const char* format, va_list va)
{
#if HMI_CORE_MM_OV_DET
    OS_ASSERT(os_heap_mem_validate(buffer, count) == true); // check if overflow when located in heap
#endif
    return _vsnprintf(_out_buffer, buffer, count, format, va);
}


int fctprintf(void (*out)(char character, void* arg), void* arg, const char* format, ...)
{
  va_list va;
  va_start(va, format);
  const out_fct_wrap_type out_fct_wrap = { out, arg };
  const int ret = _vsnprintf(_out_fct, (char*)(uintptr_t)&out_fct_wrap, (size_t)-1, format, va);
  va_end(va);
  return ret;
}

#if  TEST_PRINTF == 1
extern void ulog_console_raw_printf(const char *fmt, ...);
#define PRINTF_LOG   ulog_console_raw_printf
#if SPRINTF_FUNC_DEFINED_IN_OTHER_FLIE == 1
#define  SPRINTF sprintf_
#else
#define  SPRINTF sprintf
#endif


char test_printf_buf[100];
void test_printf(void)
{
    char *ptr = "Hello world!";
    char *np = 0;
    int m = 5;
    unsigned int bs = sizeof(int)*8;
    int mi;

    mi = (1 << (bs-1)) + 1;
    PRINTF_LOG("%s\n", ptr);
    PRINTF_LOG("printf test start\n");
    PRINTF_LOG("%s is null pointer\n", np);
    PRINTF_LOG("%d = 5\n", m);
    SPRINTF(test_printf_buf, "%d = - max int\n", mi);
    PRINTF_LOG("%s", test_printf_buf);
    PRINTF_LOG("char %c = 'a'\n", 'a');
    PRINTF_LOG("hex %x = ff\n", 0xff);
    PRINTF_LOG("hex %02x = 00\n", 0);
    PRINTF_LOG("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
    PRINTF_LOG("\n");
    PRINTF_LOG("%d %s(s) with %%\n", 0, "message");
    SPRINTF(test_printf_buf, "justif: \"%-10s\"\n", "left"); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "justif: \"%10s\"\n", "right"); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, " 3: %04d zero padded\n", 3); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, " 3: %-4d left justif.\n", 3); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, " 3: %4d right justif.\n", 3); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "-3: %04d zero padded\n", -3); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "-3: %-4d left justif.\n", -3); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "-3: %4d right justif.\n", -3); PRINTF_LOG("%s", test_printf_buf);


    // 浮点测试
    // output format of float
    SPRINTF(test_printf_buf, "float output 0 is %f\n", (float)0); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 0.1 is %f\n", (float)0.1); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output -0.1 is %f\n", (float)-0.1); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 1.2f -12.1234 is %1.2f\n", (float)-12.1234); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 1.2f 12.1234 is %1.2f\n", (float)12.1234); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 3.2f -12.1234 is %3.2f\n", (float)-12.1234); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 3.2f 12.1234 is %3.2f\n", (float)12.1234); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 8.3f 3.14159 is %8.3f\n", (float)3.14159); PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf, "float output 9.5f 3.14159 is %8.5f\n", (float)3.141592); PRINTF_LOG("%s", test_printf_buf);


    int i = 30122121;
    long i2 = 309095024l;
    short i3 = 30;
    unsigned i4 = 2123453;
    SPRINTF(test_printf_buf,"%d,%o,%x,%X,%ld,%hd,%u/n", i, i, i, i, i2, i3, i4);    //如果是：%l,%h，则输不出结果
    PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf,"%d,%ld/n", i, i2);    //试验不出%ld和%d之间的差别，因为long是4bytes
    PRINTF_LOG("%s", test_printf_buf);
    SPRINTF(test_printf_buf,"%hd,%hd/n/n/n", i, i3);    //试验了%hd和%d之间的差别，因为short是2bytes
    PRINTF_LOG("%s", test_printf_buf);
    //for string and char
    char ch1 = 'd';
    unsigned char ch2 = 160;
    char *str = "Hello everyone!";
    SPRINTF(test_printf_buf,"%c,%u,%s/n/n/n", ch1, ch2, str);    //unsigned char超过128的没有字符对应
    PRINTF_LOG("%s", test_printf_buf);

    //for float and double,unsigned and signed can not be used with double and float
    float fl = 2.566545445F;    //or 2.566545445f
    double dl = 265.5651445;
    long double dl2 = 2147483648.5654441454;
    //%g没有e格式，默认6位包括小数点前面的数,
    //%f没有e格式，默认6位仅只小数点后面包含6位
    //%e采用e格式，默认6位为转化后的小数点后面的6位
    PRINTF_LOG("test float/r/n");
    SPRINTF(test_printf_buf,"%f,%lf,%llf/n", fl, fl, dl2);
    PRINTF_LOG("%s", test_printf_buf);
    PRINTF_LOG("%f,%e,%g,%.7f/n", fl, dl, dl, dl);
    PRINTF_LOG("%f,%E,%G,%f/n", fl, dl, dl, dl);    //%F is wrong
    PRINTF_LOG("%.8f,%.10e/n", fl, dl);
    PRINTF_LOG("%.8e,%.10f/n/n/n", fl, dl);
    //for point
    PRINTF_LOG("test point/r/n");
    int *iP = &i;
    char *iP1 = str;
    void *iP2 = (void *)0x12345678;    //dangerous!
    PRINTF_LOG("%p,%p,%p/n/n/n", iP, iP1, iP2);

    //其他知识：负号，表示左对齐（默认是右对齐）；%6.3，6表示宽度，3表示精度
    char *s = "Hello world!";
    PRINTF_LOG(":%s: /n:%10s: /n:%.10s: /n:%-10s: /n:%.15s: /n:%-15s: /n:%15.10s: /n:%-15.10s:/n/n/n",
           s,
           s, s, s, s, s, s, s);
    double ddd = 563.908556444;
    PRINTF_LOG(":%g: /n:%10g: /n:%.10g: /n:%-10g: /n:%.15g: /n:%-15g: /n:%15.10g: /n:%-15.10g:/n/n/n",
           ddd,
           ddd, ddd, ddd, ddd, ddd, ddd, ddd);
    //还有一个特殊的格式%*.* ,这两个星号的值分别由第二个和第三个参数的值指定
    PRINTF_LOG("%.*s /n", 8, "abcdefgggggg");
    PRINTF_LOG("%*.*f /n", 3, 3, 1.25456f);

    // 测试%g %1.2g %1.15g %1.17g%
    double d = 123456;
    SPRINTF(test_printf_buf, "%g", d);
    PRINTF_LOG("g: %s\r\n", test_printf_buf);
    d = 12345.06;
    SPRINTF(test_printf_buf, "%g", d);
    PRINTF_LOG("g: %s\r\n", test_printf_buf);
    d = -12345.06;
    SPRINTF(test_printf_buf, "%g", d);
    PRINTF_LOG("g: %s\r\n", test_printf_buf);
    d = -12345.06;
    SPRINTF(test_printf_buf, "%1.2g", d);
    PRINTF_LOG("1.2g: %s\r\n", test_printf_buf);

    d = 123456789012345;
    SPRINTF(test_printf_buf, "%1.15g", d);
    PRINTF_LOG("1.15g: %s\r\n", test_printf_buf);

    d = 1234567890123456;
    SPRINTF(test_printf_buf, "%1.15g", d);
    PRINTF_LOG("1.15g: %s\r\n", test_printf_buf);

    d = 12345678901234567;
    SPRINTF(test_printf_buf, "%1.17g", d);
    PRINTF_LOG("1.15g: %s\r\n", test_printf_buf);

    d = 123456789012345678;
    SPRINTF(test_printf_buf, "%1.17g", d);
    PRINTF_LOG("1.17g: %s\r\n", test_printf_buf);

    d = 1234567890123456.78;
    SPRINTF(test_printf_buf, "%1.17g", d);
    PRINTF_LOG("1.17g: %s\r\n", test_printf_buf);

    return;
}
#endif

#else // redirect printf to reduce code size
int printf(const char *format, ...)
{
    return 0;
}

#endif
