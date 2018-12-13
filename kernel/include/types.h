#ifndef TYPES_H
#define TYPES_H

#define NULL ((void *)0)

#define true	1
#define false	0

typedef int	bool;

/* Signed types */
typedef signed char		s8;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long long	s64;

typedef s8	k_int8_t;
typedef s16	k_int16_t;
typedef s32	k_int32_t;
typedef s64	k_int64_t;

/* Unsigned types */
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef u8	k_uint8_t;
typedef u16	k_uint16_t;
typedef u32	k_uint32_t;
typedef u64	k_uint64_t;

/* Unsigned types for relative low level things */
typedef s8	__s8;
typedef s16	__s16;
typedef s32	__s32;
typedef s64	__s64;
typedef signed	__s;

/* Unsigned types for relative low level things */
typedef u8	__u8;
typedef u16	__u16;
typedef u32	__u32;
typedef u64	__u64;
typedef unsigned	__u;

/* Various */
typedef u16	k_wchar_t;
typedef u32	k_size_t;
typedef u32	k_addr_t;
typedef s32	k_ssize_t;

#define K_OFFSETOF(st, m)	((k_size_t)&(((st *)0)->m))

#define K_OFFSET_FROM(address, offset)	((unsigned long)address + offset)

#define K_ALIGN_UP(n, align)	\
	(((n) + (typeof(n))(align) - 1) & ~((typeof(n))(align) - 1))

#define K_MIN(a, b)	((a) > (b) ? (b) : (a))
#define K_MAX(a, b)	((a) > (b) ? (a) : (b))

#define K_KB(n)	(n << 10)
#define K_MB(n)	(n << 20)

static inline k_uint16_t swap16(k_uint16_t v)
{
	return ((v & 0xff) << 8) | (v >> 8);
}

static inline k_uint32_t swap32(k_uint32_t v)
{
	return  ((v & 0xff) << 24)		|
		(((v >> 8) & 0xff) << 16)	|
		(((v >> 16) & 0xff) << 8)	|
		(v >> 24);
}

static inline k_uint64_t swap64(k_uint64_t v)
{
	return  ((v & 0xff) << 56)		|
		(((v >> 8) & 0xff) << 48)	|
		(((v >> 16) & 0xff) << 40)	|
		(((v >> 24) & 0xff) << 32)	|
		(((v >> 32) & 0xff) << 24)	|
		(((v >> 40) & 0xff) << 16)	|
		(((v >> 48) & 0xff) << 8)	|
		((v >> 58) & 0xff);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define k_le16_to_cpu(v)	(v)
#define k_le32_to_cpu(v)	(v)
#define k_le64_to_cpu(v)	(v)
#define k_be16_to_cpu(v)	swap16(v)
#define k_be32_to_cpu(v)	swap32(v)
#define k_be64_to_cpu(v)	swap64(v)

#define k_cpu_to_le16(v)	(v)
#define k_cpu_to_le32(v)	(v)
#define k_cpu_to_le64(v)	(v)
#define k_cpu_to_be16(v)	swap16(v)
#define k_cpu_to_be32(v)	swap32(v)
#define k_cpu_to_be64(v)	swap64(v)

#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define k_le16_to_cpu(v)	swap16(v)
#define k_le32_to_cpu(v)	swap32(v)
#define k_le64_to_cpu(v)	swap64(v)
#define k_be16_to_cpu(v)	(v)
#define k_be32_to_cpu(v)	(v)
#define k_be64_to_cpu(v)	(v)

#define k_cpu_to_le16(v)	swap16(v)
#define k_cpu_to_le32(v)	swap32(v)
#define k_cpu_to_le64(v)	swap64(v)
#define k_cpu_to_be16(v)	(v)
#define k_cpu_to_be32(v)	(v)
#define k_cpu_to_be64(v)	(v)

#endif

#endif

