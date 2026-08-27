#include "../core/string_helper.h"

#include <stdarg.h>

uint8_t STRING_ParseUnsigned(const char** s, uint32_t* value) {
	if (s == 0 || *s == 0 || value == 0) return 0;

	const char* cursor = *s;
	if (*cursor < '0' || *cursor > '9') return 0;

	uint32_t parsed = 0;
	while (*cursor >= '0' && *cursor <= '9') {
		uint32_t digit = (uint32_t)(*cursor - '0');
		if (parsed > (((uint32_t)~0U - digit) / 10U)) return 0;
		parsed = parsed * 10U + digit;
		cursor++;
	}

	*s = cursor;
	*value = parsed;
	return 1;
}

int STRING_S_To_UInt(const char* s, uint32_t* i) {
	if (s == 0 || i == 0) return 0;

	const char* cursor = s;
	uint32_t parsed;
	if (!STRING_ParseUnsigned(&cursor, &parsed) || *cursor != '\0') return 0;

	*i = parsed;
	return 1;
}

static void STRING_PutChar(char** out, uint32_t* remaining, int* length, char c) {
	if (*remaining > 1U) {
		**out = c;
		*out += 1;
		*remaining -= 1U;
	}

	*length += 1;
}

static void STRING_PutString(char** out, uint32_t* remaining, int* length, const char* s) {
	while (*s != '\0') {
		STRING_PutChar(out, remaining, length, *s);
		s++;
	}
}

static void STRING_PutUnsigned(char** out, uint32_t* remaining, int* length, uint64_t value, uint32_t base, uint8_t uppercase) {
	char buffer[32];
	uint32_t index = 0;
	const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

	if (value == 0) {
		STRING_PutChar(out, remaining, length, '0');
		return;
	}

	while (value != 0) {
		buffer[index] = digits[value % base];
		value /= base;
		index++;
	}

	while (index > 0) {
		index--;
		STRING_PutChar(out, remaining, length, buffer[index]);
	}
}

static void STRING_PutSigned(char** out, uint32_t* remaining, int* length, int64_t value) {
	uint64_t magnitude;

	if (value < 0) {
		STRING_PutChar(out, remaining, length, '-');
		magnitude = (uint64_t)(-(value + 1)) + 1U;
	} else {
		magnitude = (uint64_t)value;
	}

	STRING_PutUnsigned(out, remaining, length, magnitude, 10U, 0);
}

int STRING_snprintf(char* str, uint32_t capacity, const char* fmt, ...) {
	char* out = str;
	uint32_t remaining = capacity;
	int length = 0;
	va_list args;
	va_start(args, fmt);

	for (const char* p = fmt; *p; ++p) {
		if (*p != '%') {
			STRING_PutChar(&out, &remaining, &length, *p);
			continue;
		}

		if (*(p + 1) == '%') {
			STRING_PutChar(&out, &remaining, &length, '%');
			++p;
			continue;
		}

		if (*(p + 1) == '\0') {
			STRING_PutChar(&out, &remaining, &length, '%');
			continue;
		}

		enum { LEN_NONE, LEN_L, LEN_LL } len = LEN_NONE;
		++p;
		if (*p == 'l') {
			if (*(p + 1) == 'l') { len = LEN_LL; p += 1; }
			else { len = LEN_L; }
			++p;
		}
		if (*p == '\0') {
			STRING_PutChar(&out, &remaining, &length, '%');
			STRING_PutChar(&out, &remaining, &length, 'l');
			if (len == LEN_LL) STRING_PutChar(&out, &remaining, &length, 'l');
			break;
		}

		switch (*p) {
		case 'c': {
			int c = va_arg(args, int);
			STRING_PutChar(&out, &remaining, &length, (char)c);
			break;
		}
		case 's': {
			const char* s = va_arg(args, const char*);
			if (!s) s = "(null)";
			STRING_PutString(&out, &remaining, &length, s);
			break;
		}
		case 'd': case 'i': {
			if (len == LEN_LL) STRING_PutSigned(&out, &remaining, &length, va_arg(args, long long));
			else if (len == LEN_L) STRING_PutSigned(&out, &remaining, &length, va_arg(args, long));
			else STRING_PutSigned(&out, &remaining, &length, va_arg(args, int));
			break;
		}
		case 'u': {
			if (len == LEN_LL) STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned long long), 10U, 0);
			else if (len == LEN_L) STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned long), 10U, 0);
			else STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned int), 10U, 0);
			break;
		}
		case 'x': case 'X': {
			uint8_t uppercase = (*p == 'X');
			if (len == LEN_LL) STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned long long), 16U, uppercase);
			else if (len == LEN_L) STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned long), 16U, uppercase);
			else STRING_PutUnsigned(&out, &remaining, &length, va_arg(args, unsigned int), 16U, uppercase);
			break;
		}
		case 'p': {
			uintptr_t pv = (uintptr_t)va_arg(args, void*);
			STRING_PutString(&out, &remaining, &length, "0x");
			STRING_PutUnsigned(&out, &remaining, &length, pv, 16U, 0);
			break;
		}
		default:
			STRING_PutChar(&out, &remaining, &length, '%');
			STRING_PutChar(&out, &remaining, &length, *p);
			break;
		}
	}

	if (capacity > 0U) *out = '\0';
	va_end(args);
	return length;
}

int STRING_memcopy(char* destination, uint32_t capacity, const char* source, uint32_t length) {
	if (destination == 0 || source == 0) return 0;
	if (length == 0) return 0;
	
	for (uint32_t i = 0; i < (capacity < length ? capacity : length); i++) {
		destination[i] = source[i];
	}
	
	return 1;
}

uint32_t STRING_Length(const char* s) {
	if (s == 0) return 0;

	uint32_t length = 0;
	while (s[length] != '\0') length++;
	return length;
}

uint8_t STRING_Equals(const char* a, const char* b) {
	if (a == 0 || b == 0) return 0;

	while (*a != '\0' && *b != '\0') {
		if (*a != *b) return 0;

		a++;
		b++;
	}

	return *a == *b;
}

uint8_t STRING_StartsWith(const char* s, const char* prefix) {
	if (s == 0 || prefix == 0) return 0;

	while (*prefix != '\0') {
		if (*s != *prefix) return 0;
		s++;
		prefix++;
	}

	return 1;
}

uint8_t STRING_Append(char* destination, uint32_t capacity, uint32_t* length, const char* source) {
	if (destination == 0 || length == 0 || source == 0 || capacity == 0) return 0;

	uint32_t source_length = 0;
	while (source[source_length] != '\0') {
		if (source_length >= capacity - *length - 1U) return 0;
		source_length++;
	}

	for (uint32_t i = 0; i < source_length; i++) {
		destination[*length + i] = source[i];
	}

	*length += source_length;
	destination[*length] = '\0';
	return 1;
}

uint8_t STRING_AppendLength(char* destination, uint32_t capacity, uint32_t* destination_length, const char* source, const uint32_t source_length) {
	if (destination == 0 || destination_length == 0 || source == 0 || capacity == 0) return 0;
	if (capacity < *destination_length + source_length) return 0;

	for (uint32_t i = 0; i < source_length; i++) {
		destination[*destination_length + i] = source[i];
	}

	*destination_length += source_length;
	destination[*destination_length] = '\0';
	return 1;
}
