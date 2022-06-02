/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

int raid4_valid(u_int diskno) {
	if (syscall_write_dev((u_int)&diskno, 0x13000010, 4) < 0) {
		user_panic("raid4_valid panic");
    }
	u_int zero = 0;
	if (syscall_write_dev((u_int)&zero, 0x13000020, 4) < 0) {
		user_panic("raid4_valid panic");
    }
	u_int status;
    if (syscall_read_dev((u_int)&status, 0x13000030 ,4) < 0) {
        user_panic("raid4_valid panic");
    }
	return status;
}

int raid4_write(u_int blockno, void *src) {
	int i;
	int count = 0;
	for (i = 1; i <= 4; i++) {
		if (raid4_valid(i) == 1) {
			ide_write(i, blockno * 2, src + (i - 1) * 0x200, 0x1);
			ide_write(i, blockno * 2 + 1, src + (i - 1) * 0x200 + 0x800, 0x1);
		} else {
			count++;
		}
	}
	if (raid4_valid(5) == 1) {
		char *out[0x200];
		get(out, src, src + 0x200, src + 0x400, src + 0x600);
		ide_write(5, blockno * 2, out, 0x1);
		get(out, src + 0x800, src + 0x200 + 0x800, src + 0x400 + 0x800, src + 0x600 + 0x800);
		ide_write(5, blockno * 2 + 1, out, 0x1);
	} else {
		count++;
	}
	return count;
}

int raid4_read(u_int blockno, void *dst) {
	int i;
	int count = 0;
	for (i = 1; i <= 5; i++) {
		if (raid4_valid(i) != 1) {
			count++;
		}
	}
	if (count == 0) {

	} else if (count > 1) {
		return count;
	} else {

	}
}

void* get(char *out, void *b1, void *b2, void *b3, void *b4) {
    
	user_bzero(out, 0x200);

	char *p1;
	char *p2;
	char *p3;
	char *p4;
    int len = 0x200;

    p1 = b1;
	p2 = b2;
	p3 = b3;
	p4 = b4;

	char *k = &out[0];
	while (--len >= 0) {
		(*k++) = (*p1++) ^ (*p2++) ^ (*p3++) ^ (*p4++);
	}

    return out;
}
// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;

	u_int zero = 0;
	u_int cur_offset = 0;

	while (offset_begin + offset < offset_end) {
		// Your code here 
		// error occurred, then panic.
		cur_offset = offset_begin + offset;
		if (syscall_write_dev((u_int)&diskno, 0x13000010, 4) < 0) {
			user_panic("ide_read panic");
		}
		if (syscall_write_dev((u_int)&cur_offset, 0x13000000, 4) < 0) {
			user_panic("ide_read panic");
		}
		if (syscall_write_dev((u_int)&zero, 0x13000020, 4) < 0) {
			user_panic("ide_read panic");
		}
		u_int status;
		if (syscall_read_dev((u_int)&status, 0x13000030 ,4) < 0) {
			user_panic("ide_read panic");
		}
		if (status == 0) {
			user_panic("ide_read panic");
		}
		if (syscall_read_dev((u_int)(dst + offset), 0x13004000, 0x200) < 0) {
			 user_panic("ide_read panic");
		}
		offset += 0x200;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;

	u_int one = 1;
	u_int cur_offset = 0;

	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	while (offset_begin + offset < offset_end) {
		// copy data from source array to disk buffer.
		// if error occur, then panic.
		cur_offset = offset_begin + offset;
		if (syscall_write_dev((u_int)(src + offset), 0x13004000, 0x200) < 0) {
			user_panic("ide_write panic");
		}
		if (syscall_write_dev((u_int)&diskno, 0x13000010, 4) < 0) {
			user_panic("ide_write panic");
		}
		if (syscall_write_dev((u_int)&cur_offset, 0x13000000, 4) < 0) {
			user_panic("ide_write panic");
		}
		if (syscall_write_dev((u_int)&one, 0x13000020, 4) < 0) {
			user_panic("ide_write panic");
		}

		u_int status;
		if (syscall_read_dev((u_int)&status, 0x13000030, 4) < 0) {
			user_panic("ide_write panic");
		}
		if (status == 0) {
			user_panic("ide_write panic");
		}
		offset += 0x200;
	}
}
