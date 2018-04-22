//
// Created by alexj on 10/4/2018.
//

#include <time.h>
#include "ext4.h"


SuperBlockExt4 extractExt4(int fs) {
	SuperBlockExt4 ext;

	lseek(fs, SUPER_BLOCK_BASE, SEEK_SET);
	read(fs, &ext.inode.total_count, sizeof(uint32_t));			//0x00
	read(fs, &ext.block.total_count, sizeof(uint32_t));			//0x04
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.free_blocks_count, sizeof(uint32_t));	//0x0C
	read(fs, &ext.inode.free_inodes_count, sizeof(uint32_t));	//0x10
	read(fs, &ext.block.first_free_block, sizeof(uint32_t));	//0x14
	read(fs, &ext.block.size, sizeof(uint32_t));				//0x18
	ext.block.size = (uint32_t) (1024 << ext.block.size);
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.block_group, sizeof(uint32_t));			//0x20
	read(fs, &ext.block.clusters_per_group, sizeof(uint32_t));	//0x24
	read(fs, &ext.inode.inodes_per_group, sizeof(uint32_t));	//0x28
	read(fs, &ext.volume.last_mount, sizeof(uint32_t));			//0x2C
	read(fs, &ext.volume.last_written, sizeof(uint32_t));		//0x30
	lseek(fs, LAST_CHECK_OFFSET, SEEK_SET);
	read(fs, &ext.volume.last_check, sizeof(uint32_t));			//0x40
	lseek(fs, FIRST_FREE_INODE_OFFSET, SEEK_SET);
	read(fs, &ext.inode.first_free_inode, sizeof(uint32_t));	//0x54
	read(fs, &ext.inode.size, sizeof(uint16_t));				//0x58
	lseek(fs, VOLUME_NAME_OFFSET, SEEK_SET);
	memset(ext.volume.name, '\0', 17 * sizeof(uint8_t));
	read(fs, &ext.volume.name, 16 * sizeof(uint8_t));			//0x78
	lseek(fs, GDT_ENTRIES_OFFSET, SEEK_SET);
	read(fs, &ext.block.reserved_count, sizeof(uint16_t));		//0xCE
	lseek(fs, DESC_SIZE_OFFSET, SEEK_SET);
	read(fs, &ext.block.desc_size, sizeof(uint16_t));			//0xFE

	return ext;
}

void printExt4(SuperBlockExt4 ext) {
	char aux[LENGTH];

	print("---- Filesystem Information ----\n\n");
	print("Filesystem: EXT4\n\n");

	print("INODE INFO");
	printv("\nInode size: ", ext.inode.size);
	printv("\nNumber of inodes: ", ext.inode.total_count);
	printv("\nFirst inode: ", ext.inode.first_free_inode);
	printv("\nInodes per group: ", ext.inode.inodes_per_group);
	printv("\nFree inodes: ", ext.inode.free_inodes_count);

	print("\n\nBLOCK INFO");
	printv("\nBlock size: ", ext.block.size);
//	printv("\nBlock desc size: ", ext.block.desc_size);
	printv("\nReserved blocks: ", ext.block.reserved_count);
	printv("\nFree blocks: ", ext.block.free_blocks_count);
	printv("\nTotal blocks: ", ext.block.total_count);
	printv("\nFirst block: ", ext.block.first_free_block);
	printv("\nBlocks per group: ", ext.block.block_group);
	printv("\nFrags per group: ", ext.block.clusters_per_group);

	print("\n\nVOLUME INFO");
	print("\nVolume name: ");print(ext.volume.name);
	print("\nLast check: ");print(getDate(aux, ext.volume.last_check));
	print("\nLast mount: ");print(getDate(aux, ext.volume.last_mount));
	print("\nLast written: ");print(getDate(aux, ext.volume.last_written));
	print("\n");
}

int showLinearDirectory(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode){
	uint32_t aux_32;
	uint16_t directory_length;
	uint8_t aux_8, name_length, type;
	off_t offset;
	int i;


	offset = lseek(fs, 0, SEEK_CUR);

	read(fs, &aux_32, sizeof(uint32_t));
	printf("Inode: 0x%X\n", aux_32);
	read(fs, &directory_length, sizeof(uint16_t));
	printf("Length of directory entry: 0x%X\n", directory_length);
	read(fs, &name_length, sizeof(uint8_t));
	printf("Filename length: 0x%X\n", name_length);
	read(fs, &type, sizeof(uint8_t));
	printf("File Type: 0x%X\n", type);
	print("Name: ");

	for(i = 0; i < name_length; i++){
		read(fs, &aux_8, sizeof(uint8_t));
		printc(aux_8);
	}

	println();


	if (type == 0x02 && inode != aux_32){
		showInode(fs, ext, group, aux_32);
	}
	printv("Offset: ", (uint64_t) (offset + directory_length));
	println();

	lseek(fs, offset+directory_length, SEEK_SET);
	read(fs, &aux_32, sizeof(uint32_t));
	lseek(fs, offset+directory_length, SEEK_SET);
	return aux_32;
}

void showExtentTree(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode){
	uint16_t entries, depth;
	uint32_t aux_32;
	int valid_entries = 1;
	Extent_node extentNode;


	read(fs, &entries, sizeof(uint16_t));
	printf("Entries: 0x%X\n", entries);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &depth, sizeof(uint16_t));
	printf("Depth: 0x%X\n", depth);

	while(entries--){
		print("   NODE\n");
		lseek(fs, 0x4, SEEK_CUR);	//0x0A
		read(fs, &extentNode.file_block_number, sizeof(uint32_t));
		printf("File block number: 0x%X\n", extentNode.file_block_number);
		read(fs, &extentNode.number_of_blocks, sizeof(uint16_t));
		printf("Number of blocks: 0x%X\n", extentNode.number_of_blocks);
		read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
		extentNode.file_block_addr = (extentNode.file_block_addr & 0xFFFF) << 32;
		printf("Start high: 0x%X\n", (unsigned int) extentNode.file_block_addr);
		read(fs, &aux_32, sizeof(uint32_t));
		printf("Start low: 0x%X\n", aux_32);
		extentNode.file_block_addr |= aux_32;
		printf("Addr: 0x%X\n", (unsigned int) extentNode.file_block_addr);
		// END ENTRIES

		lseek(fs, ext.block.size*extentNode.file_block_addr, SEEK_SET);

		while(valid_entries)
			valid_entries = showLinearDirectory(fs, ext, group, inode);

	}
}

void showInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode){
	uint16_t aux_16;

	print("----------------------------\n");

	lseek(fs, ext.block.size * group.inode_table_offset + ext.inode.size * (inode-1), SEEK_SET);
	printf("@inode[%d]: %lu\n", inode, ext.block.size * group.inode_table_offset + ext.inode.size * (inode-1));

	print("\n-- INODE TABLE --");
	read(fs, &aux_16, sizeof(uint16_t));
	printf("\ni_mode: 0x%X\n", aux_16);

	print(" - EXTENT TREE -\n   HEADER\n");
	lseek(fs, 0x26, SEEK_CUR);


	read(fs, &aux_16, sizeof(uint16_t));
	printf("Magic number: 0x%X\n", aux_16);

	if(aux_16 != INODE_MAGIC_NUMBER)
		return;

	showExtentTree(fs, ext, group, inode);

}

void searchExt4(int fs, const char *file) {
	SuperBlockExt4 ext;
	uint32_t aux_32;
	uint16_t aux_16;
	GroupDesc group;
	int i;

	(void)(file);	//Avoid -Wunused-parameter warning

	ext = extractExt4(fs);
	group.size = ext.block.desc_size;

	/**
	 * Extraction of block group descriptor.
	 */
	lseek(fs, SUPER_BLOCK_BASE + ext.block.size, SEEK_SET);
	read(fs, &group.block_bitmap_offset, sizeof(uint32_t));		//0x00
	group.block_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_bitmap_offset, sizeof(uint32_t));		//0x04
	group.inode_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_table_offset, sizeof(uint32_t));		//0x08
	group.inode_table_offset &= 0xFFFFFFFF;

//	lseek(fs, SUPER_BLOCK_BASE + ext.block.size + 0x20, SEEK_SET);
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.block_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_table_offset |= ((uint64_t)aux_32) << 32;

	print("-- BLOCK GROUP DESCRIPTOR --\n");
	printv("Block bitmap offset: ", group.block_bitmap_offset);
	printv("\nInode bitmap offset: ", group.inode_bitmap_offset);
	printv("\nInode table offset: ", group.inode_table_offset);
	println();

	/**
	 * Show values of the rest of the block group descriptor.
	 */
	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));		//0x0C - 0x12
		printf("Data %d: 0x%X\n", i+3, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));			//0x14
	printf("Data %d: 0x%X\n", i+3, aux_32);
	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));		//0x18 - 0x1E
		printf("Data %d: 0x%X\n", i+8, aux_16);
	}

	for (i = 0; i < 3; i++) {
		read(fs, &aux_32, sizeof(uint32_t));		//0x20 - 0x28
		printf("Data %d: 0x%X\n", i, aux_32);
	}
	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));		//0x2C - 0x32
		printf("Data %d: 0x%X\n", i+3, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));			//0x34
	printf("Data %d: 0x%X\n", i+3, aux_32);
	for (i = 0; i < 2; i++) {
		read(fs, &aux_16, sizeof(uint16_t));		//0x38 - 0x3A
		printf("Data %d: 0x%X\n", i+8, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));			//0x3C
	printf("Data %d: 0x%X\n", i+8, aux_32);


	showInode(fs, ext, group, 2);

}
