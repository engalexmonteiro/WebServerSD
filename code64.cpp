/*
 * code64.cpp
 *
 *  Created on: 19 de jul de 2020
 *      Author: User
 */



#include "code64.h"

/**********************************************************************************
*************************BASE 64 CODE/DECODE DO USUARIO E SENHA********************
***********************************************************************************/

static const char b64all[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
"ghijklmnopqrstuvwxyz0123456789+/";


void _encode(uint8_t *dest, const uint8_t *src, int len) {
// Menor que 1, nada a fazer
if (len < 1)
return;

// Dados a serem retornados
int aux[] = { 0, 0, 0, 0 };

// Primeiro elemento: os 6 bits mais significativos do primeiro
// byte
aux[0] = src[0] >> 2;

// Segundo elemento: os 2 bits menos significativos do primeiro e
// os quatro bits mais significativos do segundo byte
aux[1] = (src[0] & 0x03) << 4;

if (len > 1) {
// SE houver um segundo...
aux [1] |= (src[1] & 0xf0) >> 4;

// Terceiro elemento: os quatro bits menos significativos do
// segundo e os dois mais significativos do terceiro byte
aux [2] = (src[1] & 0x0f) << 2;

if (len > 2) {
// Se houver um terceiro...
aux[2] |= src[2] >> 6;

// Quarto elemento: os seis bits menos significatos do
// terceiro byte
aux[3] = src[2] & 0x3f;
}
}

// Codifica agora os valores numéricos para string
dest[0] = b64all[aux[0]];
dest[1] = b64all[aux[1]];
dest[2] = '=';
dest[3] = '=';
if (len > 1) {
dest[2] = b64all[aux[2]];
if (len > 2)
dest[3] = b64all[aux[3]];
}
}


const char *base64_encode(const char *original, int length) {
// Se o tamanho não for informado, consideramos uma string bem
// formada
if (length == 0)
length = strlen(original);

// Inteiro com o tamanho do código a ser gerado
int b64length = ((length + 2) / 3) * 4 + 1;

// Contadores para percorrer as strings
int i=0, j=0;

// Alocando memória para o código
char *b64 = (char *) malloc(sizeof(char) * b64length);
memset(b64, 0, b64length);

	while (i < length) {

	// Codifica um grupo de três bytes...
	_encode((uint8_t *) b64 + j,(const uint8_t *) original + i,(length - i));

	// E segue para o próximo grupo
	i += 3;
	j += 4;
	}

// Retorna o código
return (const char *) b64;
}

/**********************************************************************************
*************************FIM BASE 64 CODE/DECODE DO USUARIO E SENHA****************
***********************************************************************************/


