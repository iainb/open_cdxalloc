#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "cdxalloc.h"

#define cdxalloc_simple_debug 1
#define cdxalloc_verbose_debug 0

int fd;

cedar_env_info_t *cedar_env;
void *pos;
mapping_t **mappings;
int num_mappings = 100;

/*
    cdxalloc_open sets up the cdxalloc library, it is required
    to be called before the library is used.
*/
int cdxalloc_open(void) 
{
    int i;
    // alloc cedar_env_info_t structure
    cedar_env = (cedar_env_info_t*)malloc(sizeof(cedar_env_info_t));
    if (!cedar_env) {
        return -1;
    }
    memset(cedar_env, 0,sizeof(cedar_env_info_t));

    // allocate memory mappings
    mappings = (mapping_t **) malloc(sizeof(mapping_t *) * num_mappings);
    for(i = 0; i < num_mappings; i++)
    {
        mappings[i] = (mapping_t *) malloc(sizeof(mapping_t));
        memset(mappings[i],0,sizeof(mapping_t));
    }

    // open cedar_dev device
    fd = open(cedar_dev_name,O_RDWR);
    if (fd == -1) {
        return -1;
    }
   
    // load cedar environment info into user land 
    ioctl(fd,IOCTL_GET_ENV_INFO,cedar_env);

    #ifdef cdxalloc_simple_debug
        printf("phymem_start:0x%08x\n",cedar_env->phymem_start);
    #endif

    pos = (void *)cedar_env->phymem_start;
    return 0;
}

/*
    cdxalloc_close closes down the driver, freeing memory etc.
    TODO: implement this.
*/
int cdxalloc_close(void) 
{
    // close fd
    close(fd);
    // free cedar memory
    // free cedar_env
    return 0; 
}

/*
    cdxalloc_alloc creates a memory mapping into the cedarx video
    decoder of a specified size.

    in the mmapcall the offset (defined by the integer pos) is the
    physical address at which the mapping should start at.

    Observations of libceaderxalloc has shown mappings all being created
    on 4096 byte boundries - so this works in the same way.

*/
void* cdxalloc_alloc(int size) 
{
    void *ret;
    ret = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(int) pos);
    if (ret == MAP_FAILED) {
        printf("Error allocating %d bytes\n",size);
        return ret;
    }
    cdxalloc_createmapping(ret,pos,size);
    pos = pos + size;
    // align to 4kb boundry
    pos = pos + 4096 - ((unsigned int)pos % 4096);
    return ret;
}
/*
 * cdxalloc_free frees memory allocated at a particular address
 * TODO: implement this.
 *
 */
void cdxalloc_free(void *address) 
{
    //munmap(address,size)
}

/*
    cdxalloc_vir2phy takes a virtual user space address which
    maps to physical memory owned by the cedarx video decoder.
    This address is then converted from a virtual to a phyical 
    address.
*/
unsigned int cdxalloc_vir2phy(void *address) 
{
    int i;
    unsigned int ret = 0;

    for(i=0;i<num_mappings;i++) {
        #ifdef cdxalloc_verbose_debug
            printf("cmp: 0x%08x == 0x%08x\n",(unsigned int) mappings[i]->start_virt, (unsigned int) address);
        #endif
        if (mappings[i]->start_virt == address) {
            ret = (unsigned int) mappings[i]->start_phys;
            break;
        } else if (address > mappings[i]->start_virt && address < (mappings[i]->start_virt + mappings[i]->size)) {
            ret = (unsigned int) mappings[i]->start_phys + (address - mappings[i]->start_virt);
            break;
        }
    }
    if (ret == 0) {
        printf("couldn't find address! 0x%08x\n",(unsigned int) address);
        exit(1);
    } else {
        #ifdef cdxalloc_verbose_debug
            printf("found: 0x%08x\n",(unsigned int) address);
        #endif
    }
    ret = ret & 0x0fffffff;
    return ret;
}

/*
    cdxalloc_allocregs returns a pointer to 2048bytes of memory which
    contain the registers for the cedarx video decoder.
*/
void* cdxalloc_allocregs()
{
    return mmap(NULL, 2048, PROT_READ | PROT_WRITE, MAP_SHARED,fd,(int)cedar_env->address_macc);    
}

/*
    cdxalloc_createmapping creates an entry in the mapping array for 
    the given arguments. Used for cdxalloc_vir2phy.
*/
void cdxalloc_createmapping(void *virt,void *phys,int size) 
{
    // find first unallocated mapping
    int i;
    for (i=0;i<num_mappings;i++) {
        if (mappings[i]->start_phys == 0) {
            #ifdef cdxalloc_verbose_debug
                printf("creating mapping at %d\n",i);
            #endif
            mappings[i]->start_phys = phys;
            mappings[i]->start_virt = virt;
            mappings[i]->size = size;
            break;
        }
    }
}
