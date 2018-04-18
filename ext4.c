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

void searchExt4(int fs, char *file) {
	SuperBlockExt4 ext;
	uint32_t aux_32;
	uint16_t aux_16;
	GroupDesc group;
	int i;

	ext = extractExt4(fs);
	group.size = ext.block.desc_size;

	lseek(fs, SUPER_BLOCK_BASE + ext.block.size, SEEK_SET);
	read(fs, &group.block_bitmap_offset, sizeof(uint32_t));
	group.block_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_bitmap_offset, sizeof(uint32_t));
	group.inode_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_table_offset, sizeof(uint32_t));
	group.inode_table_offset &= 0xFFFFFFFF;

//	lseek(fs, SUPER_BLOCK_BASE + ext.block.size + 0x20, SEEK_SET);
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.block_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_bitmap_offset |= ((uint64_t)aux_32) << 32;
//	read(fs, &aux_32, sizeof(uint32_t));
//	group.inode_table_offset |= ((uint64_t)aux_32) << 32;

	printf("Block bitmap offset: %lu\n", group.block_bitmap_offset);
	printf("Inode bitmap offset: %lu\n", group.inode_bitmap_offset);
	printf("Inode table offset: %lu\n", group.inode_table_offset);

	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Data %d: 0x%X\n", i+3, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Data %d: 0x%X\n", i+3, aux_32);
	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Data %d: 0x%X\n", i+8, aux_16);
	}

	for (i = 0; i < 3; i++) {
		read(fs, &aux_32, sizeof(uint32_t));
		printf("Data %d: 0x%X\n", i, aux_32);
	}
	for (i = 0; i < 4; i++) {
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Data %d: 0x%X\n", i+3, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Data %d: 0x%X\n", i+3, aux_32);
	for (i = 0; i < 2; i++) {
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Data %d: 0x%X\n", i+8, aux_16);
	}
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Data %d: 0x%X\n", i+8, aux_32);

	lseek(fs, SUPER_BLOCK_BASE + ext.block.size + ext.block.size * group.inode_table_offset + ext.inode.size * (2-1), SEEK_SET);
	printf("addr: %d + %d*%lu\n", SUPER_BLOCK_BASE, ext.block.size, group.inode_table_offset);
	/*for (i = 0; i < ext.inode.size/2; i++){
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Inode %d: 0x%X\n", i, aux_16);
	}*/
	read(fs, &aux_16, sizeof(uint16_t));
	printf("\ni_mode: 0x%X\n", aux_16);
	lseek(fs, 0x26, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Magic number: 0x%X\n", aux_16);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Entries: 0x%X\n", aux_16);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Depth: 0x%X\n", aux_16);
	lseek(fs, 0xA, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Start high: 0x%X\n", aux_16);
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Start low: 0x%X\n", aux_32);
	lseek(fs, SUPER_BLOCK_BASE + ext.block.size * aux_32, SEEK_SET);

	uint8_t aux_8;
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Inode: 0x%X\n", aux_32);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Length of directory entry: 0x%X\n", aux_16);
	read(fs, &aux_8, sizeof(uint8_t));
	printf("Filename length: 0x%X\n", aux_8);
	read(fs, &aux_8, sizeof(uint8_t));
	printf("File Type: 0x%X\nName: ", aux_8);

	for(i = 0; i < aux_8; i++){
		read(fs, &aux_16, sizeof(uint8_t));
		aux_16 &= 0x0FF;
		printf("%c", aux_16);
	}
	printf("\nInodes per group: %d\n", ext.block.total_count);
/*
	int inode;
	for(inode = 1; inode < aux_32+1; inode++){
		lseek(fs, SUPER_BLOCK_BASE + *//*ext.block.size +*//* ext.block.size * group.inode_table_offset + ext.inode.size * (inode-1), SEEK_SET);
		lseek(fs, 0x28, SEEK_CUR);
		read(fs, &aux_16, sizeof(uint16_t));
		printf("Magic number: %d - 0x%X\n", inode, aux_16);
	}*/


	printf("\n---\nSize %lu\n",SUPER_BLOCK_BASE + /*ext.block.size +*/ ext.block.size * group.inode_table_offset + ext.inode.size * (aux_32-1));
	printv("Base: ", SUPER_BLOCK_BASE);
	printv("\nBlock_size: ", ext.block.size);
	printv("\nInode_size: ", ext.inode.size);
	printv("\nInode: ", aux_32);
	printf("\nInode_table_offset: %lu\n---", group.inode_table_offset);

	lseek(fs, SUPER_BLOCK_BASE + /*ext.block.size + */ext.block.size * group.inode_table_offset + ext.inode.size * (aux_32-1), SEEK_SET);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("\ni_mode: 0x%X\n", aux_16);
	lseek(fs, 0x2,SEEK_CUR);
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Size: %d\n", aux_32);
	lseek(fs, 0x20, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Magic number: 0x%X\n", aux_16);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Entries: 0x%X\n", aux_16);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Depth: 0x%X\n", aux_16);
	lseek(fs, 0xA, SEEK_CUR);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Start high: 0x%X\n", aux_16);
	read(fs, &aux_32, sizeof(uint32_t));
	printf("Start low: 0x%X\n", aux_32);

	lseek(fs, SUPER_BLOCK_BASE + ext.block.size * aux_32, SEEK_SET);

	read(fs, &aux_32, sizeof(uint32_t));
	printf("Inode: 0x%X\n", aux_32);
	read(fs, &aux_16, sizeof(uint16_t));
	printf("Length of directory entry: 0x%X\n", aux_16);
	read(fs, &aux_8, sizeof(uint8_t));
	printf("Filename length: 0x%X\n", aux_8);
	read(fs, &aux_8, sizeof(uint8_t));
	printf("File Type: 0x%X\nName: ", aux_8);
	for(i = 0; i < aux_8; i++){
		read(fs, &aux_16, sizeof(uint8_t));
		aux_16 &= 0x0FF;
		printf("%c", aux_16);
	}
	printf("\n");
}
