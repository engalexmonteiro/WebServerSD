/*
 * SDread.cpp
 *
 *  Created on: 19 de jul de 2020
 *      Author: User
 */

#include "SDread.h"

// SD card chip select pin.
SdFat sd;


boolean iniciar_sd_card() {

	if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) {

		sd.initErrorHalt();

	}

  return true;
}

void write_from_file2(EthernetClient &client, char * filename){

	  int c;

	  SdFile file;
	  SdFile dirFile;

	  if (!dirFile.open("/www", O_RDONLY)) {
	     sd.errorHalt("open root failed");
	   }

	  if (!file.open(&dirFile, String(filename).c_str(), O_RDONLY)) {
	    //sd.errorHalt(String(filename).c_str() );
	    client.println(F("HTTP/1.1 404 FAIL\n"));
	  }
	  Serial.println();

	  char last = 0;

	  // Copy up to 500 characters to Serial.
	  while (file.available())  {
		  client.write(file.read());
	  }

	  file.close();

	  delay(100);

}
