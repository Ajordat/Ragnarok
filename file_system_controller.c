//
// Created by alexj on 11/4/2018.
//

#include "file_system_controller.h"


enum Format getFormat(int fs){
	uint16_t magic_signature;
	uint32_t feature_compat;
	char aux[LENGTH];
	enum Format format = UNKNOWN;

	//EXT
	lseek(fs, EXT_MAGIC_NUMBER_OFFSET, SEEK_SET);
	read(fs, &magic_signature, sizeof(uint16_t));

	if (magic_signature == EXT_MAGIC_NUMBER){

		lseek(fs, 1024+0x5C, SEEK_SET);
		read(fs, &feature_compat, sizeof(uint32_t));
		if (feature_compat & 0x08){
			debug("EXT4\n");
			format = EXT4;
		} else if (feature_compat & 0x04){	//TODO: No funciona, potser s'ha de buscar a una altra posició
			debug("EXT3\n");
			format = EXT3;
		} else {
			debug("EXT2\n");
			format = EXT2;
		}

		sprintf(aux, "Magic number: 0x%X\nFeature compat: 0x%X\n", magic_signature, feature_compat);
		debug(aux);
	} else {
		//TODO: FAT16 i FAT32

	}

	return format;
}