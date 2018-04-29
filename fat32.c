//
// Created by alexj on 11/4/2018.
//

#include <unistd.h>
#include <memory.h>
#include "fat32.h"


BootSector extractFat32(int fs) {
	BootSector fat;
	uint16_t aux_16;

	lseek(fs, SYSTEM_NAME_OFFSET, SEEK_SET);
	memset(fat.system_name, '\0', 9 * sizeof(uint8_t));        //0x03
	read(fs, &fat.system_name, 8 * sizeof(uint8_t));
	read(fs, &fat.sector_size, sizeof(uint16_t));            //0x0B
	read(fs, &fat.sectors_per_cluster, sizeof(uint8_t));    //0x0D
	read(fs, &fat.reserved_sectors, sizeof(uint16_t));        //0x0E
	read(fs, &fat.n_fats, sizeof(uint8_t));                    //0x10
	lseek(fs, SECTORS_PER_FAT_OFFSET, SEEK_SET);
	read(fs, &fat.sectors_per_fat, sizeof(uint32_t));        //0x24
	lseek(fs, ROOT_CLUSTER_OFFSET, SEEK_SET);
	read(fs, &fat.root_cluster, sizeof(uint32_t));        //0x2C
	lseek(fs, VOLUME_LABEL_OFFSET, SEEK_SET);
	memset(fat.label, '\0', 12 * sizeof(uint8_t));            //0x47
	read(fs, &fat.label, 11 * sizeof(uint8_t));

	lseek(fs, MAGIC_SIGNATURE_OFFSET, SEEK_SET);                        //0x1BE
	read(fs, &aux_16, sizeof(uint16_t));
	debugvh("Magic signature", aux_16);
	debugln();

	return fat;
}


void printFat32(BootSector fat) {

	print("---- Filesystem Information ----\n\n");
	print("Filesystem: FAT32\n");

	print("\nSystem name: ");
	print(fat.system_name);
	printv("\nSector size: ", fat.sector_size);
	printv("\nSectors per cluster: ", fat.sectors_per_cluster);
	printv("\nReserved sectors: ", fat.reserved_sectors);
	printv("\nNumber of FATs: ", fat.n_fats);
	printv("\nMaximum root entries: ", fat.root_cluster);
	printv("\nSectors per FAT: ", fat.sectors_per_fat);
	print("\nLabel: ");
	print(fat.label);
	print("\n");
}


void printCluster(DirectoryEntry entry) {
	debug("Name: ");
	debug(entry.name);
	debugvh("\nAttribute", entry.attribute);
	debugvh("\nAddress", entry.address);
	debugv("\nSize", entry.size);
	debugln();
}


uint32_t getNextCluster(int fs, BootSector fat, uint32_t cluster) {
	off_t base;

	base = lseek(fs, 0, SEEK_CUR);
	lseek(fs, fat.fat_begin_lba + cluster * 32, SEEK_SET);
	read(fs, &cluster, sizeof(uint32_t));
	lseek(fs, base, SEEK_SET);
	return cluster;
}


void showCluster(int fs, BootSector fat, uint32_t cluster) {
	uint64_t address;
	uint32_t next_cluster, index;
	uint16_t addr_low;
	uint8_t byte;
	DirectoryEntry entry;
	off_t base;

	address = (fat.cluster_begin_lba + (cluster - 2) * fat.sectors_per_cluster) *
			  fat.sector_size;

	debugvh("address", (uint32_t) address);
	debugln();
	lseek(fs, address, SEEK_SET);

//	int i;
//	for (i = 0; i < 32; i++){
//		read(fs, &byte, sizeof(uint8_t));
//		debug("Cluster: ");
//		printByte(byte);
//		debugln();
//	}

	read(fs, &byte, sizeof(uint8_t));
	for (index = 0; byte; index++) {
		print("Byte: ");
		printByte(byte);
		println();
		if (byte == 0xE5)
			print("Unused\n");
		base = lseek(fs, -sizeof(uint8_t), SEEK_CUR);
		memset(entry.name, '\0', FILENAME_LENGTH + 1);
		read(fs, &entry.name, FILENAME_LENGTH * sizeof(uint8_t));
		read(fs, &entry.attribute, sizeof(uint8_t));
		lseek(fs, base + 0x14, SEEK_SET);
		read(fs, &entry.address, sizeof(uint16_t));
		entry.address <<= 16;
		lseek(fs, base + 0x1A, SEEK_SET);
		read(fs, &addr_low, sizeof(uint16_t));
		entry.address |= addr_low;
		read(fs, &entry.size, sizeof(uint32_t));
		read(fs, &byte, sizeof(uint8_t));

		printCluster(entry);

		if (entry.attribute & 0x10) {
			next_cluster = getNextCluster(fs, fat, cluster - 2 + index);
			debugvh("Next cluster", next_cluster);
			debugln();

			base = getBase(fs);
			showCluster(fs, fat, next_cluster);
			recoverBase(fs, base);
		}


	}

}


void searchFat32(int fs, const char *file) {
	BootSector fat;

	(void) file;

	fat = extractFat32(fs);

	fat.fat_begin_lba = fat.reserved_sectors;
	fat.cluster_begin_lba = fat.reserved_sectors + fat.n_fats * fat.sectors_per_fat;
	debugvh("LBA", (uint32_t) fat.cluster_begin_lba);
	debugln();

	showCluster(fs, fat, fat.root_cluster);

}

