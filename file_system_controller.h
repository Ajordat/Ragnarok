//
// Created by alexj on 11/4/2018.
//

#ifndef _FILE_SYSTEM_CONTROLLER_H_
#define _FILE_SYSTEM_CONTROLLER_H_

#include <stdint-gcc.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "ext4.h"
#include "fat32.h"


#define EXT_MAGIC_NUMBER_OFFSET			(1024+0x38)
#define EXT_FEATURE_COMPAT_OFFSET		(1024+0x5C)
#define EXT_FEATURE_RO_COMPAT_OFFSET	(1024+0x64)
#define EXT_MAGIC_NUMBER				0xEF53


enum Format {
	EXT2, EXT3, EXT4, FAT12_16, FAT32, UNKNOWN
};


char *getFormatName(enum Format format);

enum Format getFormat(int fs);

void infoCommand(char *filename);





#endif //RAGNAROK_FILE_SYSTEM_CONTROLLER_H
