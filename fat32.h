//
// Created by alexj on 11/4/2018.
//

#ifndef RAGNAROK_FAT32_H
#define RAGNAROK_FAT32_H


#include <stdint-gcc.h>

#include "utils.h"


#define SYSTEM_NAME_OFFSET            0x03
#define SECTORS_PER_FAT_OFFSET        0x24
#define ROOT_CLUSTER_OFFSET        0x2C
#define VOLUME_LABEL_OFFSET            0x47
#define MAGIC_SIGNATURE_OFFSET        0x1FE

#define SYSTEM_NAME_LENGTH        8
#define FILENAME_LENGTH                11


typedef struct {
	char system_name[SYSTEM_NAME_LENGTH + 1];
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t n_fats;
	uint32_t root_cluster;
	uint32_t sectors_per_fat;
	uint64_t fat_begin_lba;
	uint64_t cluster_begin_lba;
	char label[12];
} BootSector;

typedef struct {
	char name[FILENAME_LENGTH + 1];
	uint8_t attribute;
	uint32_t address;
	uint32_t size;
} DirectoryEntry;


BootSector extractFat32(int fs);

void printFat32(BootSector fat);

void searchFat32(int fs, const char *file);


#endif //RAGNAROK_FAT32_H
