#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  if(fd != -1){
    close(fd);
  }
  if(ehdr != NULL){
    free(ehdr);
  }
  if(phdr != NULL){
    free(phdr);
  }
}
Elf32_Ehdr* allocateElfHeader() {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)malloc(sizeof(Elf32_Ehdr));
    if (!ehdr) {
        perror("Ehdr memory allocation failed! ");
        return NULL;
    }
    return ehdr;
}
Elf32_Phdr* allocateProgramHeaders(Elf32_Ehdr* ehdr) {
    if (!ehdr) {
        return NULL;  
    }
    int total_ph_size = ehdr->e_phentsize * ehdr->e_phnum;
    Elf32_Phdr* phdr = (Elf32_Phdr*)malloc(total_ph_size);
    if (!phdr) {
        perror("Phdr memory allocation failed! ");
        free(phdr);
        return NULL;  
    }
    return phdr;
}
int* allocateVerMemory(int size) {
    int* vmem = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS| MAP_PRIVATE, 0, 0);
    if (vmem == MAP_FAILED) {
      perror("Error allocating v_mem using mmap");
      munmap(vmem,size);
      exit(1);
    }
    return vmem;
}
int readFile(int fd, void * buffer, int size){
    int rd = read(fd,buffer,size);
    if(rd==-1){
        perror("Error reading file \n");
        return -1;
    }
    return rd;
}
void movFilePointer(int fd, int offset, int start) {
    int res = lseek(fd, offset, start);
    if (res == -1) {
        perror("Error moving pointer (lseek) \n");
        exit(1);
    }
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** exe) {
  fd = open(exe[1], O_RDONLY);
  if (fd == -1) {
    perror("'Error' file is not opening\n");
    exit(1);
  }

  // 1. Load entire binary content into the memory from the ELF file.
  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"
  Elf32_Ehdr* ehdr = allocateElfHeader();
  readFile(fd,ehdr,sizeof(Elf32_Ehdr));
  Elf32_Phdr* phdr = allocateProgramHeaders(ehdr);
  movFilePointer(fd,ehdr->e_phoff, SEEK_SET);
  readFile(fd,phdr,ehdr->e_phentsize * ehdr->e_phnum);
  for (int i = 0; i < ehdr->e_phnum; ++i) {
    if (phdr[i].p_type == PT_LOAD) {
        if (ehdr->e_entry > phdr[i].p_vaddr && ehdr->e_entry < phdr[i].p_vaddr + phdr[i].p_memsz) {
            int * virtual_mem = allocateVerMemory(phdr[i].p_vaddr);
            movFilePointer(fd, phdr[i].p_offset, SEEK_SET);
            readFile(fd,virtual_mem,phdr[i].p_vaddr);
            int entry_offset = ehdr->e_entry - phdr[i].p_paddr;
            int* entry_address = (int*)((int)virtual_mem + entry_offset);
            int (* _start)() = (int(*)())entry_address;
            int result = _start();
            printf("User _start return value = %d\n",result); 
            if(munmap(virtual_mem, phdr[i].p_vaddr) == -1){
                perror("Error deallocating virtual_mem using munmap");
                exit(1);
            }
            break; 
        }            
    }
  }
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
