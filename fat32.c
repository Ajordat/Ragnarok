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
	debug(GET_NAME(entry));
	debug("\nName2: ");
	debug(entry.name);
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


int getNextCluster(int fs, BootSector fat, int *index, uint32_t *cluster) {
	uint32_t next_cluster;
	off_t address, offset;


	offset = getBase(fs);
	address = GO_TO_FAT_CLUSTER(fat, *cluster);
	lseek(fs, address, SEEK_SET);
	read(fs, &next_cluster, sizeof(uint32_t));
//	next_cluster &= 0x0FFFFFFF;
	debugvh("Current cluster long", (uint32_t) *cluster);
	debugvh("\nNext cluster long", next_cluster);
	debugln();
	if (next_cluster > 0xFFFFFF7) {
		recoverBase(fs, offset);
		return 0;
	}
	address = GET_CLUSTER_ADDRESS(fat, next_cluster);
	lseek(fs, address, SEEK_SET);
	*index = -1;
	*cluster = next_cluster;
	return 1;
}

void printFile(int fs, BootSector fat, DirectoryEntry entry) {
	long total;
	uint32_t index;
	int cont;
	uint32_t current_cluster;
	uint8_t byte;

	current_cluster = entry.address;
	lseek(fs, GET_CLUSTER_ADDRESS(fat, current_cluster), SEEK_SET);

	for (total = 0, index = 0; total < entry.size; total++, index++) {
		read(fs, &byte, sizeof(char));
		write(STDOUT, &byte, sizeof(char));

		if (index == entry.size - 1) {
			cont = getNextCluster(fs, fat, (int *) &index, &current_cluster);
			if (!cont) {
				println();
				return;
			}
		}
	}
	println();
}

DirectoryEntry extractEntry(int fs, BootSector fat, int *index, uint32_t *cluster) {
	DirectoryEntry entry;
	off_t base;
	uint16_t addr_low, date;
	uint8_t byte, count;
	int i, cluster_size;

	cluster_size = GET_CLUSTER_SIZE(fat);
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
			lseek(fs, base + DIR_ENTRY_SIZE, SEEK_SET);

			(*index)++;
			debugv("Index", *index);
			debugln();
			if (*index == cluster_size) {
				getNextCluster(fs, fat, index, cluster);
				*index = 0;
			}
			base = getBase(fs);
		} while ((count & 0x0F) != 0x01);

	}
//	printMemory(fs, 32);

	memset(entry.name, '\0', FILENAME_LENGTH + 1);
	read(fs, &entry.name, FILENAME_LENGTH * sizeof(uint8_t));
	memset(&entry.attribute, '\0', sizeof(uint8_t));
	read(fs, &entry.attribute, sizeof(uint8_t));

	lseek(fs, base + 0x0E, SEEK_SET);
	read(fs, &date, sizeof(uint16_t));
	entry.creation.tm_hour = (date >> 11) & 0x1F;
	entry.creation.tm_min = (date >> 5) & 0x3F;
	entry.creation.tm_sec = (date & 0x1F) * 2;
	read(fs, &date, sizeof(uint16_t));
	entry.creation.tm_year = ((date >> 9) & 0x7F) + 1980;
	entry.creation.tm_mon = (date >> 5) & 0x0F;
	entry.creation.tm_mday = date & 0x1F;

	lseek(fs, base + 0x14, SEEK_SET);
	read(fs, &entry.address, sizeof(uint16_t));
	entry.address <<= 16;
	lseek(fs, base + 0x1A, SEEK_SET);
	read(fs, &addr_low, sizeof(uint16_t));
	entry.address |= addr_low;
	read(fs, &entry.size, sizeof(uint32_t));

	return entry;
}


int showCluster(int fs, BootSector fat, uint32_t cluster) {
	uint64_t address;
	int index, found, cluster_size, cont;
	uint8_t next;
	DirectoryEntry entry;
	off_t base;

	cluster_size = GET_CLUSTER_SIZE(fat);
	address = GET_CLUSTER_ADDRESS(fat, cluster);

	debugvh("address", (uint32_t) address);
	debugln();
	lseek(fs, address, SEEK_SET);

	read(fs, &next, sizeof(uint8_t));
#if DEBUG
	print("Next_prev: ");
	printByte(next);
	printv("\nDepth_prev", (uint64_t) depth);
	println();
#endif

	for (index = 0; next & 0x3F && index < cluster_size; index++) {        // Don't check two highest bits

		lseek(fs, -sizeof(uint8_t), SEEK_CUR);
		if (next == 0xE5 || next == 0x05 || next == 0x2F) {
			debugvh("Unused", next);
			lseek(fs, DIR_ENTRY_SIZE, SEEK_CUR);
			read(fs, &next, sizeof(uint8_t));
			debugvh("\nNext", next);
			debugln();
			if (!next || index == cluster_size - 1) {
				cont = getNextCluster(fs, fat, &index, &cluster);
				if (!cont)
					return 0;
				read(fs, &next, sizeof(uint8_t));
			}
			continue;
		} else {
			debugvh("Used", next);
			debugln();
		}

		entry = extractEntry(fs, fat, &index, &cluster);

		read(fs, &next, sizeof(uint8_t));


		if (entry.attribute & 0xC0)
			return 0;

		if (!(entry.attribute & 0x3F)) {
			continue;
		}

		printCluster(entry);

		if (list && (HIDDEN_FILES || GET_NAME(entry)[0] != '.')) {
			if (entry.attribute & 0x08) {
				print("-- ");
				print(GET_NAME(entry));
				println();
			} else
				listFile(GET_NAME(entry));
		}

		debugv("Size", entry.size);
		debugln();
//		if (search && !strcmp(GET_NAME(entry), file_fat32)) {
//			char aux[LENGTH];
//			printv("Size", entry.size);
//			print("\nCreation: ");
//			print(getFat32Date(aux, entry.creation));
//			println();
//			return 1;
//		}

#if DEBUG
		getchar();
#endif

		if ((entry.attribute & 0x10 && !(entry.attribute & 0xC0)) &&
			((HIDDEN_FILES && !SAME_DIR_FAT32(entry.name) && !LAST_DIR_FAT32(entry.name)) ||
			 (!HIDDEN_FILES && GET_NAME(entry)[0] != '.'))) {
			base = getBase(fs);
			depth++;
			found = showCluster(fs, fat, entry.address);
			if (found)
				return 1;
			depth--;
			recoverBase(fs, base);
		} else if (!(entry.attribute & 0xDC) && show && !strcmp(GET_NAME(entry), file_fat32)) {
//			base = getBase(fs);
//			lseek(fs, GET_CLUSTER_ADDRESS(fat, cluster), SEEK_SET);
//			printMemory(fs, entry.size);
//			recoverBase(fs, base);
			printFile(fs, fat, entry);

			return 1;
		}

#if DEBUG
		print("Next: ");
		printByte(next);
		printv("\nDepth", (uint64_t) depth);
		printv("\nIndex", (uint64_t) index);
		println();
#endif

		if (!next || index == cluster_size - 1) {
			cont = getNextCluster(fs, fat, &index, &cluster);
			if (!cont)
				return 0;
			read(fs, &next, sizeof(uint8_t));
		}
	}

	return 0;
}


void searchFat32(int fs, const char *file) {
	BootSector fat;
	int found;

	memset(file_fat32, '\0', LONGNAME_LEN + 1);
	strcpy(file_fat32, file);

	fat = extractFat32(fs);

//	printFat32(fat);
	fat.fat_begin_lba = fat.reserved_sectors;
	fat.cluster_begin_lba = fat.reserved_sectors + fat.n_fats * fat.sectors_per_fat;
	debugvh("Cluster begin", (uint32_t) fat.cluster_begin_lba);
	debugvh("\nFAT begin", (uint32_t) fat.fat_begin_lba);
	debugln();

	depth = 0;
	found = showCluster(fs, fat, fat.root_cluster);
	if (!found && search) {
		print("File not found\n");
	}
}

