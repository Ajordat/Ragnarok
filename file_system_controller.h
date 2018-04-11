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

#define EXT_MAGIC_NUMBER_OFFSET		(1024+0x38)
#define EXT_MAGIC_NUMBER			0xEF53


enum Format {EXT2, EXT3, EXT4, FAT16, FAT32, UNKNOWN};

enum Format getFormat(int fs);



#endif //RAGNAROK_FILE_SYSTEM_CONTROLLER_H