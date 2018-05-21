//
// Created by alexj on 10/4/2018.
//

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
	read(fs, &ext.block.reserved_count, sizeof(uint32_t));        //0xCE
	lseek(fs, DESC_SIZE_OFFSET, SEEK_SET);
	read(fs, &ext.block.desc_size, sizeof(uint16_t));            //0xFE

	return ext;
}

GroupDesc extractGroup(int fs, SuperBlockExt4 ext) {
	GroupDesc group;
	uint32_t aux_32;
	off_t group_base = ext.block.size == 1024 ? 2048 : ext.block.size;

	group.size = ext.block.desc_size;

	lseek(fs, group_base, SEEK_SET);
	read(fs, &group.block_bitmap_offset, sizeof(uint32_t));        //0x00
	group.block_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_bitmap_offset, sizeof(uint32_t));        //0x04
	group.inode_bitmap_offset &= 0xFFFFFFFF;
	read(fs, &group.inode_table_offset, sizeof(uint32_t));        //0x08
	group.inode_table_offset &= 0xFFFFFFFF;

	lseek(fs, group_base + 0x20, SEEK_SET);
//	lseek(fs, SUPER_BLOCK_BASE + ext.block.size + 0x20, SEEK_SET);
	read(fs, &aux_32, sizeof(uint32_t));
	group.block_bitmap_offset |= ((uint64_t) aux_32) << 32;
	read(fs, &aux_32, sizeof(uint32_t));
	group.inode_bitmap_offset |= ((uint64_t) aux_32) << 32;
	read(fs, &aux_32, sizeof(uint32_t));
	group.inode_table_offset |= ((uint64_t) aux_32) << 32;

	return group;
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


void printInodeFile(int fs, SuperBlockExt4 ext, GroupDesc group, uint32_t inode) {
	off_t offset, base;
	uint64_t size;
	uint32_t size_low, date;
	char aux[LENGTH];

	offset = getBase(fs);
	base = lseek(fs, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1), SEEK_SET);
	lseek(fs, 0x4, SEEK_CUR);
	read(fs, &size_low, sizeof(uint32_t));
	lseek(fs, base + 0x6C, SEEK_SET);
	read(fs, &size, sizeof(uint32_t));
	size = (size << 32) | size_low;
	lseek(fs, base + 0x90, SEEK_SET);
	read(fs, &date, sizeof(uint32_t));

	debug("PRINT_INODE_FILE\n");

	if (search) {
		printv("Size", size);
		print("\nCreation: ");
		print(getDate(aux, date));
		println();
	}
	if (show) {
		lseek(fs, base + 0x28, SEEK_SET);
		printFileOnInode(fs, ext, group, inode, size);
	}

	recoverBase(fs, offset);
}


int printFileOnInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode, uint64_t size) {
	uint64_t i;
	uint16_t entries, depth, magic;
	uint32_t aux_32;
	off_t offset;
	Extent_node extentNode;
	int found;

	read(fs, &magic, sizeof(uint16_t));
	debugvh("Magic number", magic);
	read(fs, &entries, sizeof(uint16_t));
	debugvh("Entries", entries);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &depth, sizeof(uint16_t));
	debugvh("\nDepth", depth);
	debugln();

	lseek(fs, 0x4, SEEK_CUR);    //0x0C


	if (depth) {
		while (entries--) {
			debug("   NODE\n");
			read(fs, &extentNode.file_block_number, sizeof(uint32_t));
			read(fs, &aux_32, sizeof(uint32_t));
			read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
			extentNode.file_block_addr <<= 32;
			extentNode.file_block_addr |= aux_32;

			offset = lseek(fs, 0x02, SEEK_CUR);
			lseek(fs, ext.block.size * extentNode.file_block_addr, SEEK_SET);
			read(fs, &aux_32, sizeof(uint16_t));    //Ha de ser el magic number 0xF30A

			found = printFileOnInode(fs, ext, group, inode, size);
			if (found == FILE_FOUND)
				return FILE_FOUND;
			recoverBase(fs, offset);
		}
		return 0;
	}

	while (entries--) {
		debug("   NODE\n");
		read(fs, &extentNode.file_block_number, sizeof(uint32_t));
		debugvh("File block number", extentNode.file_block_number);
		read(fs, &extentNode.number_of_blocks, sizeof(uint16_t));
		debugvh("\nNumber of blocks", extentNode.number_of_blocks);
		read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
		extentNode.file_block_addr = (extentNode.file_block_addr << 32) & 0xFFFF00000000;
		debugvh("\nStart high", (unsigned int) extentNode.file_block_addr);
		read(fs, &aux_32, sizeof(uint32_t));
		debugvh("\nStart low", aux_32);
		extentNode.file_block_addr |= aux_32;
		debugvh("\nAddr", (unsigned int) extentNode.file_block_addr);
		debugln();
		// END ENTRIES

		offset = getBase(fs);
		lseek(fs, ext.block.size * extentNode.file_block_addr, SEEK_SET);

		uint8_t byte;

		for (i = 0; i < size; i++) {
			read(fs, &byte, sizeof(uint8_t));
			write(STDOUT, &byte, 1);
		}

		recoverBase(fs, offset);
	}
	return 0;
}


long searchOnLinearDirectory(int fs, SuperBlockExt4 ext, GroupDesc group/*, unsigned int inode*/) {
	uint32_t next_inode;
	uint16_t directory_length;
	uint8_t name_length, type;
	off_t offset;
	int found;
	char name[EXT4_NAME_LEN + 1];


	memset(name, '\0', EXT4_NAME_LEN + 1);
	offset = getBase(fs);

	read(fs, &next_inode, sizeof(uint32_t));
	debugvh("Inode", next_inode);
	read(fs, &directory_length, sizeof(uint16_t));
	debugvh("\nLength of directory entry", directory_length);
	read(fs, &name_length, sizeof(uint8_t));
	debugvh("\nFilename length", name_length);
	read(fs, &type, sizeof(uint8_t));
	debugvh("\nFile Type", type);
	debug("\nName: ");

	directory_length = (uint16_t) (directory_length > 263 ? name_length + 8 : directory_length);

	read(fs, name, name_length);
	debug(name);
	debug("\n");
	found = !strcmp(file_ext4, name);


	if (list) {
		listFile(name);
	}

	if (found && type)
		return next_inode;
//	if (search && found && type) {
//		return next_inode;
//	}
//	if (show && found && type == 0x01) {
//		return next_inode;
//	}


	if (type == 0x02 && !SAME_DIR_EXT4(name) && !LAST_DIR_EXT4(name)) {
//		getchar();
		depth++;
		found = searchOnInode(fs, ext, group, next_inode);
		depth--;
		if (found > 0)
			return found;
	}
	debugvh("Offset: ", (uint32_t) (offset + directory_length));
	debugln();

	recoverBase(fs, offset + directory_length);
	read(fs, &next_inode, sizeof(uint32_t));
	lseek(fs, -sizeof(uint32_t), SEEK_CUR);
	debugvh("Next inode", next_inode);
	debugln();
	if (next_inode > ext.inode.total_count || !next_inode)
		return DIRECTORY_ENTRIES_END;

	return -1;
}


int searchOnExtentTree(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode) {
	uint16_t entries, depth;
	uint32_t aux_32;
	off_t offset;
	Extent_node extentNode;
	long valid_entries = -1;
	int found;


	read(fs, &entries, sizeof(uint16_t));
	debugvh("Entries", entries);
	lseek(fs, 0x2, SEEK_CUR);
	read(fs, &depth, sizeof(uint16_t));
	debugvh("\nDepth", depth);
	debugln();

	lseek(fs, 0x4, SEEK_CUR);    //0x0C


	if (depth) {
		while (entries--) {
			debug("   NODE\n");
			read(fs, &extentNode.file_block_number, sizeof(uint32_t));
			read(fs, &aux_32, sizeof(uint32_t));
			read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
			extentNode.file_block_addr <<= 32;
			extentNode.file_block_addr |= aux_32;

			offset = lseek(fs, 0x02, SEEK_CUR);
			lseek(fs, ext.block.size * extentNode.file_block_addr/* + 0x28*/, SEEK_SET);
			read(fs, &aux_32, sizeof(uint16_t));    //Ha de ser el magic number 0xF30A

			found = searchOnExtentTree(fs, ext, group, inode);
			if (found == FILE_FOUND)
				return FILE_FOUND;
			recoverBase(fs, offset);
		}
		return 0;
	}

	while (entries--) {
		debug("   NODE\n");
		read(fs, &extentNode.file_block_number, sizeof(uint32_t));
		debugvh("File block number", extentNode.file_block_number);
		read(fs, &extentNode.number_of_blocks, sizeof(uint16_t));
		debugvh("\nNumber of blocks", extentNode.number_of_blocks);
		read(fs, &extentNode.file_block_addr, sizeof(uint16_t));
		extentNode.file_block_addr = (extentNode.file_block_addr << 32) & 0xFFFF00000000;
		debugvh("\nStart high", (unsigned int) extentNode.file_block_addr);
		read(fs, &aux_32, sizeof(uint32_t));
		debugvh("\nStart low", aux_32);
		extentNode.file_block_addr |= aux_32;
		debugvh("\nAddr", (unsigned int) extentNode.file_block_addr);
		debugln();
		// END ENTRIES

		offset = getBase(fs);
		lseek(fs, ext.block.size * extentNode.file_block_addr, SEEK_SET);

		while (valid_entries < 0)
			valid_entries = searchOnLinearDirectory(fs, ext, group);

		if (valid_entries > 0)
			return (int) valid_entries;

		recoverBase(fs, offset);
	}
	return 0;
}


int searchOnInode(int fs, SuperBlockExt4 ext, GroupDesc group, unsigned int inode) {
	uint16_t aux_16;
	char aux[LENGTH];

	debug("----------------------------\n");

	lseek(fs, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1), SEEK_SET);
	sprintf(aux, "@inode[%d]: %lu\n", inode, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1));
	debug(aux);

	debug("\n-- INODE TABLE --");
	read(fs, &aux_16, sizeof(uint16_t));
	debugvh("\ni_mode", aux_16);

	debug("\n - EXTENT TREE -\n   HEADER\n");
	lseek(fs, 0x26, SEEK_CUR);


	read(fs, &aux_16, sizeof(uint16_t));
	debugvh("Magic number", aux_16);
	debugln();

	if (aux_16 != INODE_MAGIC_NUMBER)
		return -1;

	return searchOnExtentTree(fs, ext, group, inode);
}

void searchOnExt4(int fs, const char *file) {
	SuperBlockExt4 ext;
	GroupDesc group;
	long inode;


	memset(file_ext4, '\0', EXT4_NAME_LEN + 1);
	strcpy(file_ext4, file);

	ext = extractExt4(fs);
	group = extractGroup(fs, ext);

	debug("-- BLOCK GROUP DESCRIPTOR --\n");
	debugv("Block bitmap offset", group.block_bitmap_offset);
	debugv("\nInode bitmap offset", group.inode_bitmap_offset);
	debugv("\nInode table offset", group.inode_table_offset);
	debugln();

	depth = 0;
	inode = searchOnInode(fs, ext, group, ROOT_INODE);

	debugv("INODE FILE", inode);
	debugln();

	if (inode > 0) {
		printInodeFile(fs, ext, group, (uint32_t) inode);
	}
}

void actionOnExt4(enum Action action, int fs, const char *file, uint32_t time) {
	SuperBlockExt4 ext;
	GroupDesc group;
	long inode;
	char aux[LENGTH];
	uint32_t flags;

	memset(file_ext4, '\0', EXT4_NAME_LEN + 1);
	strcpy(file_ext4, file);

	ext = extractExt4(fs);
	group = extractGroup(fs, ext);

	debug("-- BLOCK GROUP DESCRIPTOR --\n");
	debugv("Block bitmap offset", group.block_bitmap_offset);
	debugv("\nInode bitmap offset", group.inode_bitmap_offset);
	debugv("\nInode table offset", group.inode_table_offset);
	debugln();

	depth = 0;
//	list = show = 1;
	inode = searchOnInode(fs, ext, group, ROOT_INODE);

	debugv("INODE FILE", inode);
	debugln();

	if (inode > 0) {
		lseek(fs, ext.block.size * group.inode_table_offset + ext.inode.size * (inode - 1), SEEK_SET);
		switch (action) {
			case READ_ONLY:
				lseek(fs, 0x20, SEEK_CUR);
				read(fs, &flags, sizeof(uint32_t));
				debugvh("FLAGS", flags);
				debugln();
				if (flags & 0x10) {
					print("File is already read-only.\n");
					return;
				}
				lseek(fs, -sizeof(uint32_t), SEEK_CUR);
				flags |= 0x10;
				debugvh("FLAGS", flags);
				debugln();
				write(fs, &flags, sizeof(uint32_t));
				print("File is now read-only.\n");
				break;
			case WRITE:
				lseek(fs, 0x20, SEEK_CUR);
				read(fs, &flags, sizeof(uint32_t));
				debugvh("FLAGS", flags);
				debugln();
				if (!(flags & 0x10)) {
					print("File is not read-only.\n");
					return;
				}
				lseek(fs, -sizeof(uint32_t), SEEK_CUR);
				flags &= 0xFFFFFFEF;
				write(fs, &flags, sizeof(uint32_t));
				print("File is not read-only anymore.\n");
				break;
			case DATE:
				lseek(fs, 0x90, SEEK_CUR);
				write(fs, &time, sizeof(uint32_t));
				print("Creation date updated to ");
				print(getDate(aux, time));
				println();
				break;
			default:
				break;
		}

	}
}