#ifndef TYPE_H
#define TYPE_H

#include <stddef.h>
#include <stdint.h>

#define IF_TYPE(var, type, statement) if (var == type) { statement; }
#define DO_FOR_EACH_TYPE(var, \
	void_statement, \
	u8_statement, u16_statement, u32_statement, \
	u64_statement, i8_statement, i16_statement, \
	i32_statement, i64_statement, ptr_statement \
) \
switch (var) { case TYPE_VOID: void_statement; break; case TYPE_U8: u8_statement; break; case TYPE_U16: u16_statement; break; case TYPE_U32: u32_statement; break; case TYPE_U64: u64_statement; break; case TYPE_I8: i8_statement; break; case TYPE_I16: i16_statement; break; case TYPE_I32: i32_statement; break; case TYPE_I64: i64_statement; break; case TYPE_PTR: ptr_statement; break; default: break; }

typedef void * ptr_t;
typedef uintmax_t value_t;
typedef char * arithptr_t;

typedef enum {
	TYPE_VOID = 0,
	TYPE_U8 = 1,
	TYPE_U16 = 2,
	TYPE_U32 = 3,
	TYPE_U64 = 4,
	TYPE_I8 = 5,
	TYPE_I16 = 6,
	TYPE_I32 = 7,
	TYPE_I64 = 8,
	TYPE_PTR = 9
} type_t;

char * type_get_printf_formatter(type_t type);
char * type_get_name(type_t type);
size_t type_sizeof(type_t type);
value_t type_ptr_dereference(type_t type, ptr_t ptr);
ptr_t type_ptr_value_add(ptr_t ptr, value_t value);
ptr_t type_ptr_value_sub(ptr_t ptr, value_t value);

#endif
