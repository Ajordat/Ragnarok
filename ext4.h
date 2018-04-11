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

typedef struct {
	uint16_t inode_size;
	uint32_t total_inode_count;
	uint32_t first_inode;
	uint32_t inodes_per_group;
	uint32_t free_inodes;
} InodeInfo;

typedef struct {
	uint32_t block_size;
	uint32_t reserved_blocks;
	uint32_t free_blocks;
	uint32_t total_block_count;
	uint32_t first_block;
	uint32_t block_group;
	uint32_t frags_group;
} BlockInfo;

typedef struct {
	char volume_name[16];
	uint32_t last_check;
	uint32_t last_mount;
	uint32_t last_written;
} VolumeInfo;

typedef struct {
	InodeInfo inode;
	BlockInfo block;
	VolumeInfo volume;
} SuperBlockExt4;


SuperBlockExt4 extractExt4(int fs);

void printExt4(SuperBlockExt4 ext);


#endif //RAGNAROK_EXT4_H
