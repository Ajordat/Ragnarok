//
// Created by alexj on 10/4/2018.
//

#include <time.h>
#include "ext4.h"


SuperBlockExt4 extractExt4(int fs) {
	SuperBlockExt4 ext;

	lseek(fs, SUPER_BLOCK_BASE, SEEK_SET);
	read(fs, &ext.inode.total_count, sizeof(uint32_t));            //0x00
	read(fs, &ext.block.total_count, sizeof(uint32_t));            //0x04
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.free_blocks_count, sizeof(uint32_t));    //0x0C
	read(fs, &ext.inode.free_inodes_count, sizeof(uint32_t));    //0x10
	read(fs, &ext.block.first_free_block, sizeof(uint32_t));    //0x14
	read(fs, &ext.block.size, sizeof(uint32_t));                //0x18
	ext.block.size = (uint32_t) (1024 << ext.block.size);
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.block_group, sizeof(uint32_t));            //0x20
	read(fs, &ext.block.clusters_per_group, sizeof(uint32_t));    //0x24
	read(fs, &ext.inode.inodes_per_group, sizeof(uint32_t));    //0x28
	read(fs, &ext.volume.last_mount, sizeof(uint32_t));            //0x2C
	read(fs, &ext.volume.last_written, sizeof(uint32_t));        //0x30
	lseek(fs, LAST_CHECK_OFFSET, SEEK_SET);
	read(fs, &ext.volume.last_check, sizeof(uint32_t));            //0x40
	lseek(fs, FIRST_FREE_INODE_OFFSET, SEEK_SET);
	read(fs, &ext.inode.first_free_inode, sizeof(uint32_t));    //0x54
	read(fs, &ext.inode.size, sizeof(uint16_t));                //0x58
	lseek(fs, VOLUME_NAME_OFFSET, SEEK_SET);
	memset(ext.volume.name, '\0', 17 * sizeof(uint8_t));
	read(fs, &ext.volume.name, 16 * sizeof(uint8_t));            //0x78
	lseek(fs, GDT_ENTRIES_OFFSET, SEEK_SET);
	read(fs, &ext.block.reserved_count, sizeof(uint16_t));        //0xCE
	lseek(fs, DESC_SIZE_OFFSET, SEEK_SET);
	read(fs, &ext.block.desc_size, sizeof(uint16_t));            //0xFE

	return ext;
}

void printExt4(SuperBlockExt4 ext) {
	char aux[LENGTH];

	print("---- Filesystem Information ----\n\n");
	print("Filesystem: EXT4\n\n");

	print("INODE INFO");
	printv("\nInode size", ext.inode.size);
	printv("\nNumber of inodes", ext.inode.total_count);
	printv("\nFirst inode", ext.inode.first_free_inode);
	printv("\nInodes per group", ext.inode.inodes_per_group);
	printv("\nFree inodes", ext.inode.free_inodes_count);
	println();

	print("\nBLOCK INFO");
	printv("\nBlock size", ext.block.size);
//	printv("\nBlock desc size: ", ext.block.desc_size);
	printv("\nReserved blocks", ext.block.reserved_count);
	printv("\nFree blocks", ext.block.free_blocks_count);
	printv("\nTotal blocks", ext.block.total_count);
	printv("\nFirst block", ext.block.first_free_block);
	printv("\nBlocks per group", ext.block.block_group);
	printv("\nFrags per group", ext.block.clusters_per_group);
	println();

	print("\nVOLUME INFO");
	print("\nVolume name: ");
	print(ext.volume.name);
	print("\nLast check: ");
	print(getDate(aux, ext.volume.last_check));
	print("\nLast mount: ");
	print(getDate(aux, ext.volume.last_mount));
	print("\nLast written: ");
	print(getDate(aux, ext.volume.last_written));
	println();
}

void printFile(char *name) {
	int i;

	if (depth)
		print("|");
	for (i = 0; i < depth; i++) {
		print("  ");
	}
	print("|-");
	print(name);
	println();
}

int showLinearDirectory(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode) {
	uint32_t next_inode;
	uint16_t directory_length;
	uint8_t name_length, type;
	off_t offset;
	int i;
	char name[EXT4_NAME_LEN + 1];


	memset(name, '\0', EXT4_NAME_LEN + 1);
	offset = lseek(fs, 0, SEEK_CUR);

	read(fs, &next_inode, sizeof(uint32_t));
	debugvh("Inode", next_inode);
	read(fs, &directory_length, sizeof(uint16_t));
	debugvh("\nLength of directory entry", directory_length);
	read(fs, &name_length, sizeof(uint8_t));
	debugvh("\nFilename length", name_length);
	read(fs, &type, sizeof(uint8_t));
	debugvh("\nFile Type", type);
	debug("\nName: ");

	for (i = 0; i < name_length; i++) {
		read(fs, &name[i], sizeof(uint8_t));
	}
	debug(name);
	debug("\n");


	if (show)
		printFile(name);

	if (type == 0x02 && inode != next_inode && (strcmp(name, "..") != 0)) {
//		getchar();
		depth++;
		showInode(fs, ext, group, next_inode);
		depth--;
	}
	debugvh("Offset: ", (uint32_t) (offset + directory_length));
	debugln();

	lseek(fs, offset + directory_length, SEEK_SET);
	read(fs, &next_inode, sizeof(uint32_t));
	lseek(fs, offset + directory_length, SEEK_SET);
	return next_inode;
}

void showExtentTree(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode) {
	uint16_t entries, depth;
	uint32_t aux_32;
	int valid_entries = 1;
	Extent_node extentNode;


	read(fs, &entries, sizeof(uint16_t));
	debugvh("Entries", entries);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &depth, sizeof(uint16_t));
	debugvh("\nDepth", depth);
	debugln();

	while (entries--) {
		debug("   NODE\n");
		lseek(fs, 0x4, SEEK_CUR);    //0x0A
		read(fs, &extentNode.file_block_number, sizeof(uint32_t));
		debugvh("File block number", extentNode.file_block_number);
		read(fs, &extentNode.number_of_blocks, sizeof(uint16_t));
		debugvh("\nNumber of blocks", extentNode.number_of_blocks);
		read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
		extentNode.file_block_addr = (extentNode.file_block_addr & 0xFFFF) << 32;
		debugvh("\nStart high", (unsigned int) extentNode.file_block_addr);
		read(fs, &aux_32, sizeof(uint32_t));
		debugvh("\nStart low", aux_32);
		extentNode.file_block_addr |= aux_32;
		debugvh("\nAddr", (unsigned int) extentNode.file_block_addr);
		debugln();
		// END ENTRIES

		lseek(fs, ext.block.size * extentNode.file_block_addr, SEEK_SET);

		while (valid_entries)
			valid_entries = showLinearDirectory(fs, ext, group, inode);
	}
}

void showInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode) {
	uint16_t aux_16;
	char aux[LENGTH];

	debug("----------------------------\n");

	lseek(fs, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1), SEEK_SET);
	sprintf(aux, "@inode[%d]: %lu\n", inode, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1));
	debug(aux);

	debug("\n-- INODE TABLE --");
	read(fs, &aux_16, sizeof(uint16_t));
	debugvh("\ni_mode", aux_16);

	debug(" - EXTENT TREE -\n   HEADER\n");
	lseek(fs, 0x26, SEEK_CUR);


	read(fs, &aux_16, sizeof(uint16_t));
	debugvh("Magic number", aux_16);


	if (aux_16 != INODE_MAGIC_NUMBER)
		return;

	showExtentTree(fs, ext, group, inode);

}

void searchExt4(int fs, const char *file) {
	SuperBlockExt4 ext;
	GroupDesc group;

	(void) file;

	ext = extractExt4(fs);
	group.size = ext.block.desc_size;

	/**
	 * Extraction of block group descriptor.
	 */
	lseek(fs, SUPER_BLOCK_BASE + ext.block.size, SEEK_SET);
	read(fs, &group.block_bitmap_offset, sizeof(uint32_t));        //0x00
	group.block_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_bitmap_offset, sizeof(uint32_t));        //0x04
	group.inode_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_table_offset, sizeof(uint32_t));        //0x08
	group.inode_table_offset &= 0xFFFFFFFF;

//	lseek(fs, SUPER_BLOCK_BASE + ext.block.size + 0x20, SEEK_SET);
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.block_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_table_offset |= ((uint64_t)aux_32) << 32;

	debug("-- BLOCK GROUP DESCRIPTOR --\n");
	debugv("Block bitmap offset", group.block_bitmap_offset);
	debugv("\nInode bitmap offset", group.inode_bitmap_offset);
	debugv("\nInode table offset", group.inode_table_offset);
	debugln();

	/**
	 * Show values of the rest of the block group descriptor.
	 */
//	uint32_t aux_32;
//	uint16_t aux_16;
//	int i;
//	for (i = 0; i < 4; i++) {
//		read(fs, &aux_16, sizeof(uint16_t));        //0x0C - 0x12
//		printf("Data %d: 0x%X\n", i + 3, aux_16);
//	}
//	read(fs, &aux_32, sizeof(uint32_t));            //0x14
//	printf("Data %d: 0x%X\n", i + 3, aux_32);
//	for (i = 0; i < 4; i++) {
//		read(fs, &aux_16, sizeof(uint16_t));        //0x18 - 0x1E
//		printf("Data %d: 0x%X\n", i + 8, aux_16);
//	}
//
//	for (i = 0; i < 3; i++) {
//		read(fs, &aux_32, sizeof(uint32_t));        //0x20 - 0x28
//		printf("Data %d: 0x%X\n", i, aux_32);
//	}
//	for (i = 0; i < 4; i++) {
//		read(fs, &aux_16, sizeof(uint16_t));        //0x2C - 0x32
//		printf("Data %d: 0x%X\n", i + 3, aux_16);
//	}
//	read(fs, &aux_32, sizeof(uint32_t));            //0x34
//	printf("Data %d: 0x%X\n", i + 3, aux_32);
//	for (i = 0; i < 2; i++) {
//		read(fs, &aux_16, sizeof(uint16_t));        //0x38 - 0x3A
//		printf("Data %d: 0x%X\n", i + 8, aux_16);
//	}
//	read(fs, &aux_32, sizeof(uint32_t));            //0x3C
//	printf("Data %d: 0x%X\n", i + 8, aux_32);

	depth = 0;
	showInode(fs, ext, group, 2);
}
