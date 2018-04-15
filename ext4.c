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
	read(fs, &ext.block.first_free_block, sizeof(uint32_t));			//0x14
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