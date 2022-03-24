/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
                ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
                ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
                ehdr->e_ident[EI_MAG3] == ELFMAG3) {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

        int Nr;

        Elf32_Phdr *phdr = NULL;

        u_char *ptr_ph_table = NULL;
        Elf32_Half ph_entry_count;
        Elf32_Half ph_entry_size;


        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary)) {
                printf("not a standard elf format\n");
                return 0;
        }

        // get section table addr, section header number and section header size.
		
		ptr_ph_table = (u_char *)(ehdr->e_phoff + binary);
		ph_entry_count = ehdr->e_phnum;
		ph_entry_size = ehdr->e_phentsize;
        
		// for each section header, output section number and section addr. 
        // hint: section number starts at 0.
		phdr = (Elf32_Phdr*)(ptr_ph_table);
		int flag = 0;
		Elf32_Off first_page;
		for (Nr = 0; Nr < ph_entry_count; Nr++){
			if (Nr != ph_entry_count - 1) {
				Elf32_Addr addr = (phdr + Nr)->v_paddr + (Elf32_Addr)((phdr + Nr)->p_memsz);
				Elf32_Addr next_addr = (phdr + (Nr + 1))->v_paddr;
				if ((next_addr) != (Elf32_Addr)0){
					if (addr >= next_addr){
						flag = 1;
						first_page = addr - (addr % (Elf32_Addr)4096);
						break;
					}
					else{
						Elf32_Addr page = addr % (Elf32_Addr)4096;
		                Elf32_Addr next_page = next_addr % (Elf32_Addr)4096;
		                if ((addr - page) == (next_addr - next_page) && (page != (Elf32_Addr)0)){
							flag = 2;
							first_page = addr - (addr % (Elf32_Addr)4096);
							break;
						}
					}
				}
			}
		}
		if (flag == 0) {
			for (Nr = 0; Nr < ph_entry_count; Nr++) {
				printf("%d:0x%x,0x%x\n", Nr, (phdr + Nr)->p_filesz, (phdr + Nr)->p_memsz);
			}
		}
		else if (flag == 1) {
			printf("Conflict at page va : 0x%x\n", first_page);
		}
		else if (flag == 2) {
			printf("Overlay at page va : 0x%x\n", first_page);
		}

        return 0;
}

