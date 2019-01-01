#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"

#ifdef K_BITS_32
#define ACPI_MACHINE_WIDTH	32
#elif K_BITS_64
#define ACPI_MACHINE_WIDTH	64
#endif

#if 0
#define COMPILER_DEPENDENT_INT64	k_int64_t
#define COMPILER_DEPENDENT_UINT64	k_uint64_t
#endif
#define ACPI_SYSTEM_XFACE

#include "imports/acpica/source/include/platform/acgcc.h"
#include "imports/acpica/source/include/platform/aclinux.h"
#include "imports/acpica/source/include/platform/acenv.h"

#include "imports/acpica/source/include/actypes.h"
#include "imports/acpica/source/include/acexcep.h"
#include "imports/acpica/source/include/actbl.h"
#include "imports/acpica/source/include/acpiosxf.h"

/* Environmental and ACPI Tables. */

ACPI_STATUS AcpiOsInitialize(void)
{
	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void)
{
	return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void)
{
	return k_acpi.physical_rsdp;
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *PredefinedName,
		ACPI_STRING *NewValue)
{
	*NewValue = NULL;

	return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewValue)
{
	*NewValue = NULL;

	return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *ExistingTable,
		ACPI_PHYSICAL_ADDRESS *NewAddress, UINT32 *NewTableLength)
{
	*NewAddress = 0;

	return AE_OK;
}

/* Memory Management. */

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE Length)
{
	/* I guess you want to map only ACPI stuff, which is already mapped. */
	return k_p2v((void *)PhysicalAddress);
}

void AcpiOsUnmapMemory(void *LogicalAddress, ACPI_SIZE Length)
{

}

ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress,
		ACPI_PHYSICAL_ADDRESS *PhysicalAddress)
{
	unsigned long physical_address;

	if (!LogicalAddress || !PhysicalAddress)
		return AE_BAD_PARAMETER;

	physical_address = k_v2p_l((unsigned long)LogicalAddress);
	if (!physical_address)
		return AE_ERROR;

	*PhysicalAddress = physical_address;

	return AE_OK;
}

void *AcpiOsAllocate(ACPI_SIZE Size)
{
	return k_malloc(Size);
}

void AcpiOsFree(void *Memory)
{
	k_free(Memory);
}

static BOOLEAN acpi_os_accessible(void *ptr, unsigned long length)
{
	unsigned long a, b;

	a = (unsigned long)ptr & 0xfff;
	b = (a + length) & 0xfff;

	do {
		if (!k_v2p_l(a))
			return FALSE;

		a += 0x1000;
	} while (a != b);

	return TRUE;
}

BOOLEAN AcpiOsReadable(void *Memory, ACPI_SIZE Length)
{
	return acpi_os_accessible(Memory, Length);
}

BOOLEAN AcpiOsWritable(void *Memory, ACPI_SIZE Length)
{
	return acpi_os_accessible(Memory, Length);
}

/* Multithreading and Scheduling Services. */
ACPI_THREAD_ID AcpiOsGetThreadId(void)
{
	return 0x12345678;
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function,
		void *Context)
{

}

void AcpiOsSleep(UINT64 Milliseconds)
{

}

void AcpiOsStall(UINT32 Microseconds)
{

}

void AcpiOsWaitEventsComplete(void)
{

}

/* Mutual Exclusion and Synchronization. */

#if 0
ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX *OutHandle)
{

}

void AcpiOsDeleteMutex(ACPI_MUTEX Handle)
{

}

ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX Handle, UINT16 Timeout)
{

}

void AcpiOsReleaseMutex(ACPI_MUTEX Handle)
{

}
#endif

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits,
		ACPI_SEMAPHORE *OutHandle)
{

}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle)
{

}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout)
{

}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units)
{

}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle)
{

}

void AcpiOsDeleteLock(ACPI_HANDLE Handle)
{

}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle)
{

}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags)
{

}

/* Interrupt Handling. */

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptLevel, ACPI_OSD_HANDLER Handler,
		void *Context)
{

}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 InterruptLevel, ACPI_OSD_HANDLER Handler)
{

}

/* Memory Access and Memory Mapped I/O. */

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 *Value, UINT32 Width)
{
	void *ptr;

	ptr = k_p2v((void *)Address);
	if (!ptr)
		return AE_NOT_EXIST;

	switch (Width) {
	case 8:
		*Value = *(k_uint8_t *)ptr;
		break;

	case 16:
		*Value = *(k_uint16_t *)ptr;
		break;

	case 32:
		*Value = *(k_uint32_t *)ptr;
		break;

	case 64:
		*Value = *(k_uint64_t *)ptr;
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 Value, UINT32 Width)
{
	void *ptr;

	ptr = k_p2v((void *)Address);
	if (!ptr)
		return AE_NOT_EXIST;

	switch (Width) {
	case 8:
		*(k_uint8_t *)ptr = (k_uint8_t)Value;
		break;

	case 16:
		*(k_uint16_t *)ptr = (k_uint16_t)Value;
		break;

	case 32:
		*(k_uint32_t *)ptr = (k_uint32_t)Value;
		break;

	case 64:
		*(k_uint64_t *)ptr = Value;
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/* Port Input/Output. */

