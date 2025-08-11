#pragma once
#ifndef __TEENY_SHA1_H__
#define __TEENY_SHA1_H__

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

int sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes);

#ifdef __cplusplus
}
#endif

#endif
