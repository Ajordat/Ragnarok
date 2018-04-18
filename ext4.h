//
// Created by alexj on 10/4/2018.
//

#ifndef _EXT4_H_
#define _EXT4_H_

#include <stdint-gcc.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"


#define SUPER_BLOCK_BASE			0x400
#define LAST_CHECK_OFFSET			(SUPER_BLOCK_BASE + 0x40)
#define FIRST_FREE_INODE_OFFSET		(SUPER_BLOCK_BASE + 0x54)
#define VOLUME_NAME_OFFSET			(SUPER_BLOCK_BASE + 0x78)
#define GDT_ENTRIES_OFFSET			(SUPER_BLOCK_BASE + 0xCE)
#define DESC_SIZE_OFFSET			(SUPER_BLOCK_BASE + 0xFE)



typedef struct {
	uint16_t size;
	uint32_t total_count;
	uint32_t first_free_inode;
	uint32_t inodes_per_group;
	uint32_t free_inodes_count;
} InodeInfo;

typedef struct {
	uint32_t size;
	uint16_t desc_size;
	uint16_t reserved_count;
	uint32_t free_blocks_count;
	uint32_t total_count;
	uint32_t first_free_block;
	uint32_t block_group;
	uint32_t clusters_per_group;
} BlockInfo;

typedef struct {
	char name[17];
	uint32_t last_check;
	uint32_t last_mount;
	uint32_t last_written;
} VolumeInfo;

typedef struct {
	InodeInfo inode;
	BlockInfo block;
	VolumeInfo volume;
} SuperBlockExt4;

typedef struct {
	uint16_t size;
	uint64_t block_bitmap_offset;
	uint64_t inode_bitmap_offset;
	uint64_t inode_table_offset;
} GroupDesc;


SuperBlockExt4 extractExt4(int fs);

void printExt4(SuperBlockExt4 ext);

void searchExt4(int fs, char *file);


#endif //RAGNAROK_EXT4_H
