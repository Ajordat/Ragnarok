//
// Created by alexj on 10/4/2018.
//

#ifndef _EXT4_H_
#define _EXT4_H_

#include <stdint-gcc.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "parameters.h"


#define SAME_DIR_EXT4(name)            (!strcmp((name), "."))
#define LAST_DIR_EXT4(name)            (!strcmp((name), ".."))

#define SUPER_BLOCK_BASE            0x400
#define LAST_CHECK_OFFSET            (SUPER_BLOCK_BASE + 0x40)
#define FIRST_FREE_INODE_OFFSET        (SUPER_BLOCK_BASE + 0x54)
#define VOLUME_NAME_OFFSET            (SUPER_BLOCK_BASE + 0x78)
#define DESC_SIZE_OFFSET            (SUPER_BLOCK_BASE + 0xFE)
#define GDT_ENTRIES_OFFSET            (SUPER_BLOCK_BASE + 0x154)

#define INODE_MAGIC_NUMBER            0xF30A
#define EXT4_NAME_LEN                255
#define VOLUME_NAME_LENGTH		17
#define ROOT_INODE                    2

#define FILE_FOUND                    (-1)
#define DIRECTORY_ENTRIES_END        0

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
	uint32_t reserved_count;
	uint32_t free_blocks_count;
	uint32_t total_count;
	uint32_t first_free_block;
	uint32_t block_group;
	uint32_t clusters_per_group;
} BlockInfo;

typedef struct {
	char name[VOLUME_NAME_LENGTH];
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

typedef struct {
	uint32_t file_block_number;
	uint16_t number_of_blocks;
	uint64_t file_block_addr;
} Extent_node;


char file_ext4[EXT4_NAME_LEN + 1];


SuperBlockExt4 extractExt4(int fs);

void printExt4(SuperBlockExt4 ext);

int printFileOnInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode, uint64_t size, uint64_t *index);

int searchOnInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode);

void searchOnExt4(int fs, const char *file);

void printInodeFile(int fs, SuperBlockExt4 ext, GroupDesc group, uint32_t inode);

void actionOnExt4(enum Action action, int fs, const char *file, uint32_t time);


#endif //RAGNAROK_EXT4_H
