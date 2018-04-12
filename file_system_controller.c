//
// Created by alexj on 11/4/2018.
//

#include <sys/fcntl.h>
#include <stdlib.h>
#include "file_system_controller.h"

char *getFormatName(enum Format format) {
	switch (format) {
		case EXT2:
			return "EXT2";
		case EXT3:
			return "EXT3";
		case EXT4:
			return "EXT4";
		case FAT16:
			return "FAT16";
		case FAT32:
			return "FAT32";
		default:
			return "UNKNOWN";
	}
}

enum Format getFormat(int fs) {
	uint16_t magic_signature, feature_compat, feature_ro_compat;
	char aux[LENGTH];
	enum Format format = UNKNOWN;

	//EXT
	lseek(fs, EXT_MAGIC_NUMBER_OFFSET, SEEK_SET);
	read(fs, &magic_signature, sizeof(uint16_t));

	if (magic_signature == EXT_MAGIC_NUMBER) {

		lseek(fs, 1024 + 0x5C, SEEK_SET);
		read(fs, &feature_compat, sizeof(uint32_t));
		lseek(fs, 0x04, SEEK_CUR);
		read(fs, &feature_ro_compat, sizeof(uint32_t));

		if (feature_compat & 0x08) {			//Supports extended attributes
			if (feature_ro_compat & 0x20) {		//Avoids ext3 limit on subdirectories
				debug("EXT4\n");
				format = EXT4;
			} else {
				debug("EXT3\n");
				format = EXT3;
			}
		} else {
			debug("EXT2\n");
			format = EXT2;
		}

		sprintf(aux, "Magic number: 0x%X\nFeature compat: 0x%X\nFeature ro compat: 0x%X\n", magic_signature,
				feature_compat, feature_ro_compat);
		debug(aux);
	} else {
		//TODO: FAT16 i FAT32
		format = FAT32;
	}

	return format;
}

void infoCommand(char *filename) {
	enum Format format;

	int fs = open(filename, O_RDONLY);
	if (fs <= 0) {
		print("The file doesn't exist.\n");
		return;
	}

	format = getFormat(fs);

	switch (format) {
		case EXT4:
			printExt4(extractExt4(fs));
			break;
		case FAT32:
			printFat32(extractFat32(fs));
			break;
		default:
			print("File system not recognized (");
			print(getFormatName(format));
			print(").\n");
			break;
	}
	close(fs);

}