/*
 * SDread.h
 *
 *  Created on: 19 de jul de 2020
 *      Author: User
 */

#ifndef SDREAD_H_
#define SDREAD_H_

#include <Arduino.h>
#include <Ethernet.h>
#include <SdFat.h>
#include <FreeStack.h>

#define SD_CS_PIN 4

extern SdFat sd;

boolean iniciar_sd_card();

void write_from_file2(EthernetClient &client, char * filename);

#endif /* SDREAD_H_ */
