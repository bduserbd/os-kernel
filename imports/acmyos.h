#ifndef __ACMYOS_H__
#define __ACMYOS_H__

#include "kernel/include/os-interface.h"

#define ACPI_USE_SYSTEM_CLIBRARY
#define ACPI_USE_DO_WHILE_0
#define ACPI_IGNORE_PACKAGE_RESOLUTION_ERRORS

//#define ACPI_USE_STANDARD_HEADERS

//#define ACPI_DEBUGGER

#define ACPI_DEBUG_OUTPUT
//#define ACPI_DISASSEMBLER

#define ACPI_FLUSH_CPU_CACHE()
#define ACPI_CAST_PTHREAD_T(Pthread) ((ACPI_THREAD_ID) (Pthread))

#if defined(__ia64__)    || (defined(__x86_64__) && !defined(__ILP32__)) ||\
    defined(__aarch64__) || defined(__PPC64__) ||\
    defined(__s390x__)
#define ACPI_MACHINE_WIDTH          64
#define COMPILER_DEPENDENT_INT64    long
#define COMPILER_DEPENDENT_UINT64   unsigned long
#else
#define ACPI_MACHINE_WIDTH          32
#define COMPILER_DEPENDENT_INT64    long long
#define COMPILER_DEPENDENT_UINT64   unsigned long long
#define ACPI_USE_NATIVE_DIVIDE
#define ACPI_USE_NATIVE_MATH64
#endif

#include "acgcc.h"

#endif

