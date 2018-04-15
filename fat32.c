//
// Created by alexj on 11/4/2018.
//

#include <unistd.h>
#include <memory.h>
#include "fat32.h"


ReservedSection extractFat32(int fs) {
	ReservedSection fat;

	lseek(fs, SYSTEM_NAME_OFFSET, SEEK_SET);
	memset(fat.boot_sector.system_name, '\0', 9 * sizeof(uint8_t));		//0x03
	read(fs, &fat.boot_sector.system_name, 8 * sizeof(uint8_t));
	read(fs, &fat.boot_sector.sector_size, sizeof(uint16_t));			//0x0B
	read(fs, &fat.boot_sector.sectors_per_cluster, sizeof(uint8_t));	//0x0D
	read(fs, &fat.boot_sector.reserved_sectors, sizeof(uint16_t));		//0x0E
	read(fs, &fat.boot_sector.n_fats, sizeof(uint8_t));					//0x10
	lseek(fs, SECTORS_PER_FAT_OFFSET, SEEK_SET);
	read(fs, &fat.boot_sector.sectors_per_fat, sizeof(uint32_t));		//0x24
	lseek(fs, MAX_ROOT_ENTRIES_OFFSET, SEEK_SET);
	read(fs, &fat.boot_sector.max_root_entries, sizeof(uint32_t));		//0x2C
	lseek(fs, VOLUME_LABEL_OFFSET, SEEK_SET);
	memset(fat.boot_sector.label, '\0', 12 * sizeof(uint8_t));			//0x47
	read(fs, &fat.boot_sector.label, 11 * sizeof(uint8_t));

	return fat;
}

void printFat32(ReservedSection fat) {

	print("---- Filesystem Information ----\n\n");
	print("Filesystem: FAT32\n");

	print("\nSystem name: ");print(fat.boot_sector.system_name);
	printv("\nSector size: ", fat.boot_sector.sector_size);
	printv("\nSectors per cluster: ", fat.boot_sector.sectors_per_cluster);
	printv("\nReserved sectors: ", fat.boot_sector.reserved_sectors);
	printv("\nNumber of FATs: ", fat.boot_sector.n_fats);
	printv("\nMaximum root entries: ", fat.boot_sector.max_root_entries);
	printv("\nSectors per FAT: ", fat.boot_sector.sectors_per_fat);
	print("\nLabel: ");print(fat.boot_sector.label);
	print("\n");
}