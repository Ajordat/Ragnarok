//
// Created by alexj on 11/4/2018.
//

#ifndef RAGNAROK_FAT32_H
#define RAGNAROK_FAT32_H


#include <stdint-gcc.h>

#include "utils.h"

#define SAME_DIR_FAT32(name)    (!strcmp((name), ".          "))
#define LAST_DIR_FAT32(name)    (!strcmp((name), "..         "))
#define GET_NAME(entry)            ((entry).is_longname ? (entry).long_name:(entry).name)
#define GET_CLUSTER_ADDRESS(fat,cluster)	(((fat).cluster_begin_lba + ((cluster) - 2) * (fat).sectors_per_cluster) * (fat).sector_size)
#define GO_TO_FAT_CLUSTER(fat, cluster)		((fat).fat_begin_lba * (fat).sector_size + (cluster) * 4)
#define GET_CLUSTER_SIZE(fat)	((fat).sector_size * (fat).sectors_per_cluster / DIR_ENTRY_SIZE)

#define SYSTEM_NAME_OFFSET            0x03
#define SECTORS_PER_FAT_OFFSET        0x24
#define ROOT_CLUSTER_OFFSET        0x2C
#define VOLUME_LABEL_OFFSET            0x47
#define MAGIC_SIGNATURE_OFFSET        0x1FE

#define DIR_ENTRY_SIZE            32
#define SYSTEM_NAME_LENGTH        8
#define FILENAME_LENGTH                11
#define LABEL_LENGTH            12
#define LONGNAME_LEN        LENGTH


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
	char label[LABEL_LENGTH];
} BootSector;

typedef struct {
	char name[FILENAME_LENGTH + 1];
	char long_name[LONGNAME_LEN];
	uint8_t attribute;
	uint8_t is_longname;
	uint32_t address;
	uint32_t size;
	struct tm creation;
} DirectoryEntry;


char file_fat32[LONGNAME_LEN];


BootSector extractFat32(int fs);

void printFat32(BootSector fat);

void searchFat32(int fs, const char *file);


#endif //RAGNAROK_FAT32_H
