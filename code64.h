/*
 * code64.h
 *
 *  Created on: 19 de jul de 2020
 *      Author: User
 */

#ifndef CODE64_H_
#define CODE64_H_

#include <Arduino.h>

const char *base64_encode(const char *original, int length);

void _encode(uint8_t *dest, const uint8_t *src, int len);

#endif /* CODE64_H_ */
