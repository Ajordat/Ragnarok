# Ragnarok

This project is a program able of recognizing the following file-systems: FAT16, FAT32, EXT2, EXT3 and EXT4.

Among those volumes, it can interact with FAT32 and EXT4. The read operations performed on these fs are the following:

Parameter | Effect
----- | -----
`--info` | Print basic information.
`--list` | Show the directory tree.
`--search <file>` | Show the size and date creation of the file.
`--show <file>` | Show the content of the file.

The following operations are performed on the same fs, but this set of actions are not just reading the volum but also interacting with it.

Parameter | Effect | File system
--- | --- | ---
`-r <file>` | Set file as _read-only_. | FAT32, EXT4
`-w <file>` | Set file as _read-write_. | FAT32, EXT4
`-h <file>` | Set file as hidden. | FAT32
`-s <file>` | Set file as un-hidden. | FAT32
`-d <date> <file>` | Change file's creation date. | FAT32, EXT4



