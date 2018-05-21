//
// Created by alexj on 11/4/2018.
//

#include <sys/fcntl.h>
#include "file_system_controller.h"

char *getFormatName(enum Format format) {
	switch (format) {
		case EXT2:
			return "EXT2";
		case EXT3:
			return "EXT3";
		case EXT4:
			return "EXT4";
		case FAT12_16:
			return "FAT12/16";
		case FAT32:
			return "FAT32";
		default:
			return "UNKNOWN";
	}
}

enum Format getFormat(int fs) {
	uint16_t magic_signature, feature_compat, feature_incompat, feature_ro_compat;
	uint16_t root_entries_fat16, total_sec_16b;
	uint32_t total_sec_32b;
	char aux[LENGTH];
	enum Format format = UNKNOWN;

	lseek(fs, EXT_MAGIC_NUMBER_OFFSET, SEEK_SET);
	read(fs, &magic_signature, sizeof(uint16_t));

	/**
	 * To be detected as an EXT volume, it must have the value of 0xEF53 at address 0x0438, which is the magic signature for EXT volumes.
	 */
	if (magic_signature == EXT_MAGIC_NUMBER) {

		lseek(fs, EXT_FEATURE_COMPAT_OFFSET, SEEK_SET);
		read(fs, &feature_compat, sizeof(uint32_t));
		read(fs, &feature_incompat, sizeof(uint32_t));
		lseek(fs, EXT_FEATURE_RO_COMPAT_OFFSET, SEEK_SET);
		read(fs, &feature_ro_compat, sizeof(uint32_t));

		/**
		 * To be detected as EXT3/4, the EXT volume must support extended attributes. If it doesn't, the volume is
		 * labeled as EXT2.
		 *
		 * To distinguish between EXT3 and EXT4, the flag RO_COMPAT_DIR_NLINK of field s_feature_ro_compat is checked.
		 * This flag indicates if the 32000 subdirectory limit no longer applies, which is a limitation of ext3.
		 */
		if (feature_compat & 0x08) {            //Supports extended attributes
			if (feature_ro_compat & 0x20) {        //Avoids ext3 limit on subdirectories
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

		sprintf(aux, "Magic number: 0x%X\nFeature compat: 0x%X\nFeature incompat: 0x%X\nFeature ro compat: 0x%X\n",
				magic_signature,
				feature_compat, feature_incompat, feature_ro_compat);
		debug(aux);
	} else {
		lseek(fs, 0x11, SEEK_SET);
		read(fs, &root_entries_fat16, sizeof(uint16_t));    //Root entries for fat12/16, must be 0 in fat32
		read(fs, &total_sec_16b,
			 sizeof(uint16_t));            //Count of sectors in volume for fat12/16 if fits, must be 0 otherwise or in fat32
		lseek(fs, 0x20, SEEK_SET);
		read(fs, &total_sec_32b,
			 sizeof(uint32_t));            //Count of sectors in volume for fat32 or fat12/16 if doesn't fit in total_sec_16b

		/**
		 * To be detected as fat12/16, the field BPB_RootEntCnt is checked. This field contains the count of 32-byte directory entries in
		 * the root directory. In fat12/16 it must have a value whereas in fat32 it must be 0.
		 *
		 * The fields BPB_TotSec16 and BPB_TotSec32 are also checked to distinguish between fat16, fat32 and an unknown file. This fields
		 * hold the total count of all sectors in all four regions of the volume.
		 *
		 * There are these situations in fat12/16:
		 * 	-	The count of sectors is less than 65536: BPB_TotSec16 holds the value and BPB_TotSec32 is 0.
		 * 	-	The count of sectors is equal of higher than 65536: BPB_TotSec32 holds the value and BPB_TotSec16 is 0.
		 *
		 * To confirm it's a fat12/16 volume, one of BPB_TotSec16 of BPB_TotSec32 must be 0 and the other must be non-zero.
		 *
		 * To be detected as fat32, BPB_RootEntCnt and BPB_TotSec32 must be non-zero and BPB_TotSec16 must be zero.
		 *
		 * If the volume doesn't fit these requirements, it's marked as unknown.
		 */
		if (root_entries_fat16 && !(total_sec_32b && total_sec_16b) && (total_sec_32b != total_sec_16b)) {
			format = FAT12_16;
		} else if (!root_entries_fat16 && total_sec_32b && !total_sec_16b) {
			format = FAT32;
		}

		sprintf(aux, "Fat field: 0x%X\nTotal sec high: 0x%X\nTotal sec low: 0x%X\n", root_entries_fat16,
				total_sec_32b, total_sec_16b);
		debug(aux);
	}

	return format;
}

void infoCommand(char *filesystem) {
	enum Format format;

	int fs = open(filesystem, O_RDONLY);
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
			close(fs);
			exit(EXIT_FAILURE);
	}
	close(fs);

}

void searchCommand(char *file, char *filesystem) {
	enum Format format;

	int fs = open(filesystem, O_RDONLY);
	if (fs <= 0) {
		print("The file doesn't exist.\n");
		return;
	}

	format = getFormat(fs);

	switch (format) {
		case EXT4:
			searchOnExt4(fs, file);
			break;
		case FAT32:
			searchFat32(fs, file);
			break;
		default:
			print("File system not recognized (");
			print(getFormatName(format));
			print(").\n");
			close(fs);
			exit(EXIT_FAILURE);
	}
	close(fs);
}

void actionCommand(enum Action action, char *file, char *filesystem, uint32_t time) {
	enum Format format;


	int fs = open(filesystem, O_RDWR);
	if (fs <= 0) {
		print("The file doesn't exist.\n");
		return;
	}

	format = getFormat(fs);

	switch (format) {
		case EXT4:
			if (action == HIDE || action == SHOW_HIDDEN) {
				print("Activating or deactivating the hidden file feature is not available on EXT4.\n");
				close(fs);
				exit(EXIT_FAILURE);
			}
			actionOnExt4(action, fs, file, time);
			break;
		case FAT32:

			break;
		default:
			print("File system not recognized (");
			print(getFormatName(format));
			print(").\n");
			close(fs);
			exit(EXIT_FAILURE);
	}
	close(fs);
}
