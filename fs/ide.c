/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

int time_read() {

	u_int zero = 0;

	if (syscall_write_dev((u_int)&zero, 0x15000000, 4) < 0) {
		user_panic("time_read panic");
    }

	u_int time = 0;
	if (syscall_read_dev((u_int)&time, 0x15000010 , 0x4) < 0) {
        user_panic("time_read panic");
	}

	return time;
}

void raid0_write(u_int secno, void *src, u_int nsecs) {
	int i;
	for (i = 0; i < nsecs; i++) {
		if ((i + secno) % 2 == 0) {
			ide_write(0x1, (secno + i) / 2, src + (i * 0x200), 0x1);
		} else {
			ide_write(0x2, (secno + i) / 2, src + (i * 0x200), 0x1);
		}
	}
}

void raid0_read(u_int secno, void *dst, u_int nsecs) {
	int i;
    for (i = 0; i < nsecs; i++) {
        if ((i + secno) % 2 == 0) {
            ide_read(0x1, (secno + i) / 2, dst + (i * 0x200), 0x1);
		} else {
	        ide_read(0x2, (secno + i) / 2, dst + (i * 0x200), 0x1);
        }
	}
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
