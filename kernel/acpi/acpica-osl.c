#include "kernel/include/acpi/acpica.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/io/io.h"
#include "kernel/include/pci/pci.h"
#include "kernel/include/video/print.h"

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
	k_printf("%lx\n", k_acpi.physical_rsdp);
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

ACPI_STATUS AcpiOsCreateCache(char *CacheName, UINT16 ObjectSize, UINT16 MaxDepth,
		ACPI_CACHE_T **ReturnCache)
{

}

ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T *Cache)
{

}

ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T *Cache)
{

}

void *AcpiOsAcquireObject(ACPI_CACHE_T *Cache)
{

}

ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T *Cache, void *Object)
{

}

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

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS Address, UINT32 *Value, UINT32 Width)
{
	switch (Width) {
	case 8:
		*Value = k_inb(Address);
		break;

	case 16:
		*Value = k_inw(Address);
		break;

	case 32:
		*Value = k_inl(Address);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS Address, UINT32 Value, UINT32 Width)
{
	switch (Width) {
	case 8:
		k_outb((k_uint8_t)Value, Address);
		break;

	case 16:
		k_outw((k_uint16_t)Value, Address);
		break;

	case 32:
		k_outl(Value, Address);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/* PCI Configuration Space Access. */

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *PciId, UINT32 Register, UINT64 *Value,
		UINT32 Width)
{
	switch (Width) {
	case 8:
		*Value = k_pci_read_config_byte(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register);
		break;

	case 16:
		*Value = k_pci_read_config_word(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register);
		break;

	case 32:
		*Value = k_pci_read_config_long(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *PciId, UINT32 Register, UINT64 Value,
		UINT32 Width)
{
	switch (Width) {
	case 8:
		k_pci_write_config_byte(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register, (k_uint8_t)Value);
		break;

	case 16:
		k_pci_write_config_word(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register, (k_uint16_t)Value);
		break;

	case 32:
		k_pci_write_config_long(PciId->Bus, PciId->Device, PciId->Function,
				(k_uint8_t)Register, (k_uint32_t)Value);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/* Formatted Output. */

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char *Format, ...)
{
	va_list Args;

	va_start(Args, Format);
	k_vprintf(Format, Args);

	va_end(Args);
}

void AcpiOsVprintf(const char *Format, va_list Args)
{
	k_vprintf(Format, Args);
}

void AcpiOsRedirectOutput(void *Destination)
{

}

/* System ACPI Table Access. */

/* Miscellaneous. */

UINT64 AcpiOsGetTimer(void)
{

}

ACPI_STATUS AcpiOsSignal(UINT32 Function, void *Info)
{

}

ACPI_STATUS AcpiOsGetLine(char *Buffer, UINT32 BufferLength, UINT32 *BytesRead)
{

}

ACPI_STATUS AcpiOsEnterSleep(UINT8 SleepState, UINT32 RegaValue, UINT32 RegbValue)
{
	return AE_NOT_IMPLEMENTED;
}

