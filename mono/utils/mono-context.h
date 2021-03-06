/*
 * mono-context.h: plat independent machine state definitions
 *
 *
 * Copyright (c) 2011 Novell, Inc (http://www.novell.com)
 */


#ifndef __MONO_MONO_CONTEXT_H__
#define __MONO_MONO_CONTEXT_H__

#include "mono-compiler.h"
#include "mono-sigcontext.h"
#include "mono-machine.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

/*
 * General notes about mono-context.
 * Each arch defines a MonoContext struct with all GPR regs + IP/PC.
 * IP/PC should be the last element of the struct (this is a mild sgen constraint we could drop if needed)
 * Macros to get/set BP, SP and IP are defined too.
 * MONO_CONTEXT_GET_CURRENT captures the current context as close as possible. One reg might be clobbered
 *  to hold the address of the target MonoContext. It will be a caller save one, so should not be a problem.
 */
#if defined(__i386__)

/*HACK, move this to an eventual mono-signal.c*/
#if defined( __linux__) || defined(__sun) || defined(__APPLE__) || defined(__NetBSD__) || \
       defined(__FreeBSD__) || defined(__OpenBSD__)
#define MONO_SIGNAL_USE_SIGACTION
#endif

#if defined(__native_client__)
#undef MONO_SIGNAL_USE_SIGACTION
#endif

#ifdef HOST_WIN32
/* sigcontext surrogate */
struct sigcontext {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebp;
	unsigned int esp;
	unsigned int esi;
	unsigned int edi;
	unsigned int eip;
};
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
# define SC_EAX sc_eax
# define SC_EBX sc_ebx
# define SC_ECX sc_ecx
# define SC_EDX sc_edx
# define SC_EBP sc_ebp
# define SC_EIP sc_eip
# define SC_ESP sc_esp
# define SC_EDI sc_edi
# define SC_ESI sc_esi
#elif defined(__HAIKU__)
# define SC_EAX regs.eax
# define SC_EBX regs._reserved_2[2]
# define SC_ECX regs.ecx
# define SC_EDX regs.edx
# define SC_EBP regs.ebp
# define SC_EIP regs.eip
# define SC_ESP regs.esp
# define SC_EDI regs._reserved_2[0]
# define SC_ESI regs._reserved_2[1]
#else
# define SC_EAX eax
# define SC_EBX ebx
# define SC_ECX ecx
# define SC_EDX edx
# define SC_EBP ebp
# define SC_EIP eip
# define SC_ESP esp
# define SC_EDI edi
# define SC_ESI esi
#endif

typedef struct {
	mgreg_t eax;
	mgreg_t ebx;
	mgreg_t ecx;
	mgreg_t edx;
	mgreg_t ebp;
	mgreg_t esp;
	mgreg_t esi;
	mgreg_t edi;
	mgreg_t eip;
} MonoContext;

#define MONO_CONTEXT_SET_IP(ctx,ip) do { (ctx)->eip = (mgreg_t)(ip); } while (0); 
#define MONO_CONTEXT_SET_BP(ctx,bp) do { (ctx)->ebp = (mgreg_t)(bp); } while (0); 
#define MONO_CONTEXT_SET_SP(ctx,sp) do { (ctx)->esp = (mgreg_t)(sp); } while (0); 

#define MONO_CONTEXT_GET_IP(ctx) ((gpointer)((ctx)->eip))
#define MONO_CONTEXT_GET_BP(ctx) ((gpointer)((ctx)->ebp))
#define MONO_CONTEXT_GET_SP(ctx) ((gpointer)((ctx)->esp))

/*We set EAX to zero since we are clobering it anyway*/
#define MONO_CONTEXT_GET_CURRENT(ctx) \
	__asm__ __volatile__(   \
	"movl $0x0, 0x00(%0)\n" \
	"mov %%ebx, 0x04(%0)\n" \
	"mov %%ecx, 0x08(%0)\n" \
	"mov %%edx, 0x0c(%0)\n" \
	"mov %%ebp, 0x10(%0)\n" \
	"mov %%esp, 0x14(%0)\n" \
	"mov %%esi, 0x18(%0)\n" \
	"mov %%edi, 0x1c(%0)\n" \
	"call 1f\n"     \
	"1: pop 0x20(%0)\n"     \
	:	\
	: "a" (&(ctx))  \
	: "memory")


#elif defined(__x86_64__) /* defined(__i386__) */


#if !defined( HOST_WIN32 ) && !defined(__native_client__) && !defined(__native_client_codegen__)

#define MONO_SIGNAL_USE_SIGACTION 1

#endif

typedef struct {
	mgreg_t rax;
	mgreg_t rbx;
	mgreg_t rcx;
	mgreg_t rdx;
	mgreg_t rbp;
	mgreg_t rsp;
    mgreg_t rsi;
	mgreg_t rdi;
	mgreg_t r8;
	mgreg_t r9;
	mgreg_t r10;
	mgreg_t r11;
	mgreg_t r12;
	mgreg_t r13;
	mgreg_t r14;
	mgreg_t r15;
	mgreg_t rip;
} MonoContext;

#define MONO_CONTEXT_SET_IP(ctx,ip) do { (ctx)->rip = (mgreg_t)(ip); } while (0); 
#define MONO_CONTEXT_SET_BP(ctx,bp) do { (ctx)->rbp = (mgreg_t)(bp); } while (0); 
#define MONO_CONTEXT_SET_SP(ctx,esp) do { (ctx)->rsp = (mgreg_t)(esp); } while (0); 

#define MONO_CONTEXT_GET_IP(ctx) ((gpointer)((ctx)->rip))
#define MONO_CONTEXT_GET_BP(ctx) ((gpointer)((ctx)->rbp))
#define MONO_CONTEXT_GET_SP(ctx) ((gpointer)((ctx)->rsp))

#define MONO_CONTEXT_GET_CURRENT(ctx)	\
	__asm__ __volatile__(	\
		"movq $0x0,  0x00(%0)\n"	\
		"movq %%rbx, 0x08(%0)\n"	\
		"movq %%rcx, 0x10(%0)\n"	\
		"movq %%rdx, 0x18(%0)\n"	\
		"movq %%rbp, 0x20(%0)\n"	\
		"movq %%rsp, 0x28(%0)\n"	\
		"movq %%rsi, 0x30(%0)\n"	\
		"movq %%rdi, 0x38(%0)\n"	\
		"movq %%r8,  0x40(%0)\n"	\
		"movq %%r9,  0x48(%0)\n"	\
		"movq %%r10, 0x50(%0)\n"	\
		"movq %%r11, 0x58(%0)\n"	\
		"movq %%r12, 0x60(%0)\n"	\
		"movq %%r13, 0x68(%0)\n"	\
		"movq %%r14, 0x70(%0)\n"	\
		"movq %%r15, 0x78(%0)\n"	\
		"leaq (%%rip), %%rdx\n"	\
		"movq %%rdx, 0x80(%0)\n"	\
		: 	\
		: "a" (&(ctx))	\
		: "rdx", "memory")

#elif defined(__arm__) /* defined(__x86_64__) */

typedef struct {
	gulong eip;          // pc 
	gulong esp;          // sp
	gulong regs [16];
	double fregs [8];
} MonoContext;

/* we have the stack pointer, not the base pointer in sigcontext */
#define MONO_CONTEXT_SET_IP(ctx,ip) do { (ctx)->eip = (int)ip; } while (0); 
#define MONO_CONTEXT_SET_BP(ctx,bp) do { (ctx)->regs [ARMREG_FP] = (int)bp; } while (0); 
#define MONO_CONTEXT_SET_SP(ctx,bp) do { (ctx)->esp = (int)bp; } while (0); 

#define MONO_CONTEXT_GET_IP(ctx) ((gpointer)((ctx)->eip))
#define MONO_CONTEXT_GET_BP(ctx) ((gpointer)((ctx)->regs [ARMREG_FP]))
#define MONO_CONTEXT_GET_SP(ctx) ((gpointer)((ctx)->esp))

#else  

#error "Implement mono-context for the current arch"

#endif

void mono_sigctx_to_monoctx (void *sigctx, MonoContext *mctx) MONO_INTERNAL;
void mono_monoctx_to_sigctx (MonoContext *mctx, void *sigctx) MONO_INTERNAL;

#endif /* __MONO_MONO_CONTEXT_H__ */
