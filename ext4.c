//
// Created by alexj on 10/4/2018.
//

#include <time.h>
#include "ext4.h"


SuperBlockExt4 extractExt4(int fs) {
	SuperBlockExt4 ext;

	lseek(fs, 1024, SEEK_SET);
	read(fs, &ext.inode.total_inode_count, sizeof(uint32_t));	//0x00
	read(fs, &ext.block.total_block_count, sizeof(uint32_t));	//0x04
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.free_blocks, sizeof(uint32_t));			//0x0C
	read(fs, &ext.inode.free_inodes, sizeof(uint32_t));			//0x10
	read(fs, &ext.block.first_block, sizeof(uint32_t));			//0x14
	read(fs, &ext.block.block_size, sizeof(uint32_t));			//0x18
	ext.block.block_size = (uint32_t) (1024 << ext.block.block_size);
	lseek(fs, 0x04, SEEK_CUR);
	read(fs, &ext.block.block_group, sizeof(uint32_t));			//0x20
	read(fs, &ext.block.clusters_per_group, sizeof(uint32_t));	//0x24
	read(fs, &ext.inode.inodes_per_group, sizeof(uint32_t));	//0x28
	read(fs, &ext.volume.last_mount, sizeof(uint32_t));			//0x2C
	read(fs, &ext.volume.last_written, sizeof(uint32_t));		//0x30
	lseek(fs, 1024+0x40, SEEK_SET);
	read(fs, &ext.volume.last_check, sizeof(uint32_t));			//0x40
	lseek(fs, 1024+0x54, SEEK_SET);
	read(fs, &ext.inode.first_inode, sizeof(uint32_t));			//0x54
	read(fs, &ext.inode.inode_size, sizeof(uint16_t));			//0x58
	lseek(fs, 1024+0x78, SEEK_SET);
	memset(ext.volume.volume_name, '\0', 17*sizeof(uint8_t));
	read(fs, &ext.volume.volume_name, 16*sizeof(uint8_t));		//0x78
	lseek(fs, 1024+0xCE, SEEK_SET);
	read(fs, &ext.block.reserved_blocks, sizeof(uint16_t));		//0xCE

	return ext;
}

void printExt4(SuperBlockExt4 ext) {
	char aux[LENGTH];

	print("---- Filesystem Information ----\n\n");
	print("Filesystem: EXT4\n\n");

	print("INODE INFO");
	printv("\nInode size: ", ext.inode.inode_size);
	printv("\nNumber of inodes: ", ext.inode.total_inode_count);
	printv("\nFirst inode: ", ext.inode.first_inode);
	printv("\nInodes group: ", ext.inode.inodes_per_group);
	printv("\nFree inodes: ", ext.inode.free_inodes);

	print("\n\nBLOCK INFO");
	printv("\nBlock size: ", ext.block.block_size);
	printv("\nReserved blocks: ", ext.block.reserved_blocks);
	printv("\nFree blocks: ", ext.block.free_blocks);
	printv("\nTotal blocks: ", ext.block.total_block_count);
	printv("\nFirst block: ", ext.block.first_block);
	printv("\nBlock group: ", ext.block.block_group);
	printv("\nFrags group: ", ext.block.clusters_per_group);

	print("\n\nVOLUME INFO");
	print("\nVolume name: ");print(ext.volume.volume_name);
	print("\nLast check: ");print(getDate(aux, ext.volume.last_check));
	print("\nLast mount: ");print(getDate(aux, ext.volume.last_mount));
	print("\nLast written: ");print(getDate(aux, ext.volume.last_written));
	print("\n");
}