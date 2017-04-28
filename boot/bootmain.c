#include<defs.h>
#include<x86.h>
#include<elf.h>

unsigned int SECTSIZE = 512;
struct elfhdr* ELFHDR = ((struct elfhdr*) 0x10000);

static void waitdisk(void) {
	while ((inb(0x1F7) & 0xC0) != 0x40)
		continue;
}

static void readsect(void *dst, uint32_t secno) {
	waitdisk();

	outb(0x1F2, 1); //count=1
	outb(0x1F3, secno & 0xff);
	outb(0x1F4, (secno >> 8) & 0xff);
	outb(0x1F5, (secno >> 16) & 0xff);
	outb(0x1F6, ((secno >> 24) & 0xff) | 0xe0);
	outb(0x1f7, 0x20); //cmd 0x20 read sectors;

	waitdisk();

	insl(0x1f0, dst, SECTSIZE / 4);
}

//readseg - read @count bytes at @offset from kernel into virtual address @va,
//va为内存起点，offset为文件起点

static void readseg(uintptr_t va,uint32_t count,uint32_t offset){
	uintptr_t end_va=va+count;
	va-=offset%SECTSIZE;

	uint32_t secno =(offset/SECTSIZE)+1;// translate from bytes to sectors; kernel starts at sector 1

	for(;va<end_va;va+=SECTSIZE,secno++)
	{
		readsect((void *)va,secno);
	}
}

void bootmain(){

	readseg((uintptr_t)ELFHDR,SECTSIZE*8,0);//// read the 1st page off disk

	if(ELFHDR->e_magic!=ELF_MAGIC)
	{
		goto bad;
	}

	struct proghdr*ph,*eph;

	ph=(struct proghdr *)((uintptr_t)ELFHDR+ELFHDR->e_phoff);
	eph=ph+ELFHDR->e_phnum;
	for(;ph<eph;ph++)
	{
		readseg(ph->p_va & 0xffffff,ph->p_memsz,ph->p_offset);
	}


	((void(*)(void))(ELFHDR->e_entry & 0xFFFFFF))();


	bad:
	outw(0x8A00, 0x8A00);
	outw(0x8A00, 0x8E00);
	while(1)
		;

}
