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
	debug(entry.is_longname ? entry.long_name : entry.name);
	debug("\nLongname: ");
	debug(entry.is_longname ? "Yes" : "No");
	debugvh("\nAttribute", entry.attribute);
	debugvh("\nAddress", entry.address);
	debugv("\nSize", entry.size);
	debugln();
}


off_t moveNextNamePosition(int fs, int index, off_t base) {

	switch (index) {
		case 4:
			return lseek(fs, base + 0x0E, SEEK_SET);
		case 10:
			return lseek(fs, base + 0x1C, SEEK_SET);
		case 12:
			return 0;
		default:
			return lseek(fs, 0x01, SEEK_CUR);
	}
}


DirectoryEntry extractEntry(int fs) {
	DirectoryEntry entry;
	off_t base;
	uint16_t addr_low;
	uint8_t byte, count;
	int i;

	base = getBase(fs);

	lseek(fs, 0x0B, SEEK_CUR);
	read(fs, &entry.attribute, sizeof(uint8_t));
	recoverBase(fs, base);

	entry.is_longname = (uint8_t) (entry.attribute == 0x0F);

	if (entry.is_longname) {

		memset(entry.long_name, '\0', LENGTH);
		do {
//			printMemory(fs, 32);
			read(fs, &count, sizeof(uint8_t));
			debugvh("Count", count);
			debugln();
			for (i = 0;; i++) {
				read(fs, &byte, sizeof(uint8_t));
				entry.long_name[((count & 0x0F) - 1) * 13 + i] = byte;
				if (!byte)
					break;
				if (!moveNextNamePosition(fs, i, base))
					break;
			}
			lseek(fs, base + 32, SEEK_SET);
			base = getBase(fs);
		} while ((count & 0x0F) != 0x01);

	}

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

	return entry;
}


void showCluster(int fs, BootSector fat, uint32_t cluster) {
	uint64_t address;
	uint32_t index;
	uint8_t next;
	DirectoryEntry entry;
	off_t base;

	address = (fat.cluster_begin_lba + (cluster - 2) * fat.sectors_per_cluster) *
			  fat.sector_size;

	debugvh("address", (uint32_t) address);
	debugln();
	lseek(fs, address, SEEK_SET);

	read(fs, &next, sizeof(uint8_t));
//	print("Next: ");
//	printByte(next);
//	printv("\nDepth", (uint64_t) depth);
//	println();

	for (index = 0; next & 0x3F; index++) {
		if (next == 0xE5) {
			debug("Unused\n");
			break;
		}
		lseek(fs, -sizeof(uint8_t), SEEK_CUR);

		entry = extractEntry(fs);

		read(fs, &next, sizeof(uint8_t));

		if (entry.attribute & 0xC0)
			break;

		printCluster(entry);

		if (list) {
			listFile(entry.is_longname ? entry.long_name : entry.name);
		}

//		getchar();


		if ((entry.attribute & 0x10) && !SAME_DIR_FAT32(entry.name) && !LAST_DIR_FAT32(entry.name)) {
//			if (entry.is_longname) {
//				if (entry.long_name[0] == '.')
//					continue;
//			} else {
//				if (entry.name[0] == '.')
//					continue;
//			}
			base = getBase(fs);
			depth++;
			showCluster(fs, fat, entry.address);
			depth--;
			recoverBase(fs, base);
		}
//		print("Next: ");
//		printByte(next);
//		printv("\nDepth", (uint64_t) depth);
//		println();

	}

//	uint32_t next_cluster;
//	base = getBase(fs);
//	lseek(fs, (fat.fat_begin_lba + (cluster - 2) * fat.sectors_per_cluster) *
//			  fat.sector_size, SEEK_SET);
//	read(fs, &next_cluster, sizeof(uint32_t));
//	debugvh("Next cluster", next_cluster);
//	debugln();
//	recoverBase(fs, base);
//	getchar();
//	if (next_cluster < 0xFFFFFF8 && next_cluster >= 2){
//		showCluster(fs, fat, next_cluster);
//	}
}


void searchFat32(int fs, const char *file) {
	BootSector fat;

	(void) file;

	fat = extractFat32(fs);

	fat.fat_begin_lba = fat.reserved_sectors;
	fat.cluster_begin_lba = fat.reserved_sectors + fat.n_fats * fat.sectors_per_fat;
	debugvh("LBA", (uint32_t) fat.cluster_begin_lba);
	debugln();

	depth = 0;
	showCluster(fs, fat, fat.root_cluster);
}

