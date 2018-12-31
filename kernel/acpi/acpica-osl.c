#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"

#ifdef K_BITS_32
#define ACPI_MACHINE_WIDTH	32
#elif K_BITS_64
#define ACPI_MACHINE_WIDTH	64
#endif

#define COMPILER_DEPENDENT_INT64	k_int64_t
#define COMPILER_DEPENDENT_UINT64	k_uint64_t
#define ACPI_SYSTEM_XFACE

#include "imports/acpica/source/include/actypes.h"
#include "imports/acpica/source/include/acexcep.h"
#include "imports/acpica/source/include/actbl.h"

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

