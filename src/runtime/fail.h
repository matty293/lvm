/**----------------------------------------------------------------------
  The Lazy Virtual Machine.

  Daan Leijen.

  Copyright 2001, Daan Leijen. This file is distributed under the terms
  of the GNU Library General Public License. This file is based on the
  original Objective Caml source copyrighted by INRIA Rocquencourt.
----------------------------------------------------------------------**/

/***********************************************************************/
/*                                                                     */
/*                           Objective Caml                            */
/*                                                                     */
/*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         */
/*                                                                     */
/*  Copyright 1996 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the GNU Library General Public License.         */
/*                                                                     */
/***********************************************************************/

/* $Id$ */

#ifndef _fail_h
#define _fail_h

#include <setjmp.h>
#include <signal.h>
#include "misc.h"
#include "mlvalues.h"
#include "roots.h"

/*----------------------------------------------------------------------
 exception handling
----------------------------------------------------------------------*/
#ifdef POSIX_SIGNALS
struct longjmp_buffer {
  sigjmp_buf buf;
};
#else
struct longjmp_buffer {
  jmp_buf buf;
};
#define sigsetjmp(buf,save) setjmp(buf)
#define siglongjmp(buf,val) longjmp(buf,val)
#endif


struct exception_frame {
  struct exception_frame*   _prev;
  struct caml__roots_block* _local_roots;
  struct longjmp_buffer     _jmp;
  value                     _exn;
};

#define Setup_exception_handler(frame,thread,exn,handler) \
                 { if (thread != NULL) { \
                      frame._prev = thread->exn_frame; \
                      thread->exn_frame = &frame; \
                   } else { \
                      frame._prev = global_exn_frame; \
                      global_exn_frame  = &frame;     \
                   } \
                   frame._exn = 0; \
                   frame._local_roots = local_roots;   \
                   if (sigsetjmp(frame._jmp.buf, 0)) { \
                      exn = frame._exn; \
                      local_roots = frame._local_roots; \
                      handler; \
                   } \
                 }

#define Restore_exception_handler(frame,thread) \
                 { if (thread != NULL) { \
                      thread->exn_frame = frame._prev; \
                   } else { \
                      global_exn_frame = frame._prev; \
                   } \
                 }

struct exception_frame* global_exn_frame;

/*----------------------------------------------------------------------
   exception tags
----------------------------------------------------------------------*/
enum exn_tag {
  Exn_async_heap_overflow,
  Exn_async_stack_overflow,
  Exn_async_signal,
  Exn_runtime,
  Exn_arithmetic,
  Exn_system,
  Exn_invalid_arg,
  Exn_assert,
  Exn_not_found,
  Exn_user,
  Exn_count
};

enum exn_runtime {
  Exn_failed_pattern,
  Exn_blackhole,
  Exn_out_of_bounds,
  Exn_exit,
  Exn_invalid_opcode,
  Exn_load_error,
  Exn_runtime_error,
  Exn_runtime_count
};

enum exn_system {
  Exn_eof,
  Exn_system_blocked_io,
  Exn_system_error,
  Exn_system_count
};


enum exn_arithmetic {
  /* IEEE 754 floating point exceptions (and sticky tags) */
  Fpe_invalid,
  Fpe_zerodivide,
  Fpe_overflow,
  Fpe_underflow,
  Fpe_inexact,
  Fpe_denormal,

  /* integer arithmetic, [Int_underflow] is used for negative overflows */
  Int_zerodivide,
  Int_overflow,
  Int_underflow,

  /* other floating point exceptions. [Fpe_error] is a general floating point error */
  Fpe_error,
  Fpe_unemulated,
  Fpe_sqrtneg,
  Fpe_stackoverflow,
  Fpe_stackunderflow,

  Fpe_count
};


bool is_async_exception( enum exn_tag tag );

enum exn_field {
  Field_exn_val1,
  Field_exn_val2,
};


/*----------------------------------------------------------------------
  raise exceptions
----------------------------------------------------------------------*/
void fatal_uncaught_exception( value exn ) Noreturn;

void raise_invalid_argument (const char *) Noreturn;
void raise_user(const char *, ...) Noreturn;
void raise_internal( const char*, ... ) Noreturn;

void raise_out_of_memory (unsigned long size) Noreturn;
void raise_stack_overflow (unsigned long size) Noreturn;
void raise_signal( int sig) Noreturn;
void raise_sys_error(int err, const char* msg) Noreturn;
void raise_sys_blocked_io (void) Noreturn;
void raise_invalid_opcode( long opcode ) Noreturn;
void raise_module(const char* name, const char* msg, ...) Noreturn;

void raise_exn( enum exn_tag exn ) Noreturn;
void raise_exn_str( enum exn_tag exn, const char* msg ) Noreturn;

void raise_arithmetic_exn( enum exn_arithmetic tag ) Noreturn;
void raise_runtime_exn_1( enum exn_runtime, value v ) Noreturn;

#endif /* _fail_h */
