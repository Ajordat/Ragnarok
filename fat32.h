//
// Created by alexj on 11/4/2018.
//

#ifndef RAGNAROK_FAT32_H
#define RAGNAROK_FAT32_H


#include <stdint-gcc.h>

#include "utils.h"


typedef struct {
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t n_fats;
	uint32_t max_root_entries;
	uint32_t sectors_per_fat;
	char label[12];
}BootSector;

typedef struct {
	BootSector boot_sector;
}ReservedSection;


ReservedSection extractFat32(int fs);

void printFat32(ReservedSection fat);


#endif //RAGNAROK_FAT32_H
