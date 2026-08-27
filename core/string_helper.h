#pragma once

#include <stdint.h>

int STRING_snprintf(char* str, uint32_t capacity, const char* fmt, ...);
int STRING_memcopy(char* destination, uint32_t capacity, const char* source, uint32_t length);

uint32_t STRING_Length(const char* s);
uint8_t STRING_Equals(const char* a, const char* b);
uint8_t STRING_StartsWith(const char* s, const char* prefix);

uint8_t STRING_Append(char* destination, uint32_t capacity, uint32_t* length, const char* source);
uint8_t STRING_AppendLength(char* destination, uint32_t capacity, uint32_t* destination_length, const char* source, const uint32_t source_length);

uint8_t STRING_ParseUnsigned(const char** s, uint32_t* value);
int STRING_S_To_UInt(const char* s, uint32_t* i);
