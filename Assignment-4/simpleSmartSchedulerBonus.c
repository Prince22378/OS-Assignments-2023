#include "loader.h"
#include <signal.h>
Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
int totalPageAllocated=0;
int pageFaults=0;
int totalInternalFrag=0;
int *virtual_mem = NULL;
int size = 0;
/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
    if (fd != -1)
    {
        close(fd);
    }
    if (ehdr != NULL)
    {
        free(ehdr);
    }
    if (phdr != NULL)
    {
        free(phdr);
    }
    if(munmap(virtual_mem, size) == -1)
    {
        perror("Error deallocating virtual_mem using munmap! \n");
        exit(1);
    }
}

int readFile( int fd, void *buffer, int size)
{
    int rd = read(fd, buffer, size);
    if (rd == -1)
    {
        perror("Error reading file! \n");
        return -1;
    }
    return rd;
}

void movFilePointer( int fd, int offset, int start)
{
    int res = lseek(fd, offset, start);
    if (res == -1)
    {
        perror("Error moving pointer (lseek)! \n");
        exit(1);
    }
}

Elf32_Ehdr *allocateElfHeader()
{
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    // printf("e %x\n",ehdr->e_entry);
    if (!ehdr)
    {
        perror("Ehdr memory allocation failed! \n");
        return NULL;
    }
    return ehdr;
}

Elf32_Phdr *allocateProgramHeaders(Elf32_Ehdr *ehdr)
{
    if (!ehdr)
    {
        return NULL;
    }
    int total_ph_size = ehdr->e_phentsize * ehdr->e_phnum; // program header table size is not fixed unlike ehdr.
    Elf32_Phdr *phdr = (Elf32_Phdr *)malloc(total_ph_size);
    if (!phdr)
    {
        perror("Phdr memory allocation failed! \n");
        free(phdr);
        return NULL;
    }
    return phdr;
}

int* allocateVerMemory(void* addr,int size) {
    int* vmem = mmap(addr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS| MAP_PRIVATE, 0, 0);
    if (vmem == MAP_FAILED) {
      perror("Error allocating v_mem using mmap!\n");
      munmap(vmem,size);
      exit(1);
    }
    return vmem;
}

// int count=0;
void segmentation_fault_handler(int signum, siginfo_t *info, void *context){   
    pageFaults++;
    if (signum == SIGSEGV){
        printf("Segmentation fault occurred at address: %p\n", info->si_addr);
        // count+=1;
        // printf("e entry point address %x\n", ehdr->e_entry);
        // Elf32_Phdr p = phdr;
        // Handle the segmentation fault here
        for (int i = 0; i < ehdr->e_phnum; ++i)
        {   
            int pageForThisSegment=0;
            //  printf("hi\n");
            // printf("%x\n", phdr[i].p_vaddr);
            if (info->si_addr >= (void *)phdr[i].p_vaddr && info->si_addr < (void *)(phdr[i].p_vaddr + phdr[i].p_memsz))
            {   
                while(phdr[i].p_memsz>4096*pageForThisSegment){                    
                    pageForThisSegment++;
                }
                size = 4096;
                totalInternalFrag= totalInternalFrag+((pageFaults*4096)-phdr[i].p_memsz);
                totalPageAllocated+=1;
                int *virtual_mem = allocateVerMemory(info->si_addr,4096);
                movFilePointer(fd,phdr[i].p_offset, SEEK_SET);
                readFile(fd,virtual_mem, 4096);
            }
        }       
    }
}

void load_and_run_elf(char **exe)
{
    fd = open(exe[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Error: File is not opening!\n");
        exit(1);
    }

    ehdr = allocateElfHeader();
    readFile( fd,ehdr, sizeof(Elf32_Ehdr));
    //   printf("e %x\n",ehdr->e_entry);

    phdr = allocateProgramHeaders(ehdr);
    movFilePointer(fd, ehdr->e_phoff, SEEK_SET);
    readFile( fd,phdr, ehdr->e_phentsize * ehdr->e_phnum);
    //   for (int i = 0; i < ehdr->e_phnum; ++i) {
    //     printf("%x\n", phdr[i].p_vaddr);
    //   }
}


//error handling for elf files 
// int checkElfFile(const char *filename) {
//     // Open the file in binary read mode
//     FILE *file = fopen(filename, "rb");
//     if (file == NULL) {
//         perror("Error opening file");
//         return 1;
//     }
//     // Read the first 4 bytes of the file
//     unsigned char magic[4];
//     size_t read_bytes = fread(magic, 1, 4, file);
//     // Close the file
//     fclose(file);
//     // Check if the read was successful and verify the ELF magic number
//     if (read_bytes == 4 && magic[0] == 0x7F && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F') {
//         printf("%s is an ELF file.\n", filename);
//         return 1;
//     } else {
//         printf("%s is not an ELF file.\n", filename);
//         return 0;
//     }
// }

int main(int argc, char **argv)
{
    //   1. carry out necessary checks on the input ELF file
    if (argc != 2)
    {
        printf("Usage: %s <ELF Executable> \n", argv[0]);
        exit(1);
    }
    //checking the elf file or not
    // int elf_exists = checkElfFile(argv[1]);
    // if (!elf_exists) {
    //     printf("The file is not an ELF file or could not be loaded.\n");
    // }
    // else{
    //   2. passing it to the loader for carrying out the loading/execution
    load_and_run_elf(argv);

    struct sigaction sa;
    sa.sa_sigaction = segmentation_fault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    int *entry_address = (int *)(int)ehdr->e_entry;
    int (*_start)() = (int (*)())entry_address;

    if (sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        perror("Error setting up signal handler!\n");
        exit(1);
    }

    int result = _start();
    printf("User _start return value = %d\n",result); 
    printf("total page allocated: %d\n", totalPageAllocated);
    printf("total no. of page faults: %d\n",pageFaults);
    // printf("total internal fragmentation: %d\n",totalInternalFrag);
    printf("total internal fragmentation: %0.2f KB\n",(double)(totalInternalFrag/1024.0));
    // 3. invoke the cleanup routine inside the loader
    loader_cleanup();
    // }
    return 0;
}
