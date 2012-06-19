
enum IOCTL_CMD {
	IOCTL_UNKOWN = 0x100,
	IOCTL_GET_ENV_INFO,
	IOCTL_WAIT_VE,
	IOCTL_RESET_VE,
	IOCTL_ENABLE_VE,
	IOCTL_DISABLE_VE,
	IOCTL_SET_VE_FREQ,

	IOCTL_CONFIG_AVS2 = 0x200,
	IOCTL_GETVALUE_AVS2 ,
	IOCTL_PAUSE_AVS2 ,
	IOCTL_START_AVS2 ,
	IOCTL_RESET_AVS2 ,
	IOCTL_ADJUST_AVS2,
	IOCTL_ENGINE_REQ,
	IOCTL_ENGINE_REL,
	IOCTL_ENGINE_CHECK_DELAY,
	IOCTL_GET_IC_VER,

	IOCTL_ADJUST_AVS2_ABS,
	IOCTL_FLUSH_CACHE,
};

typedef struct CEDAR_ENV_INFORMATION {
	unsigned int phymem_start;
	int  phymem_total_size;
	unsigned int  address_macc;
}cedar_env_info_t;

typedef struct {
    void *start_virt;
    void *start_phys;
    int size;
}mapping_t;

int          cdxalloc_open(void);
int          cdxalloc_close(void);
void*        cdxalloc_alloc(int size);
void*        cdxalloc_allocregs();
void         cdxalloc_free(void *address);
unsigned int cdxalloc_vir2phy(void *address);
void         cdxalloc_createmapping(void *virt,void *phys,int size);
#define cedar_dev_name "/dev/cedar_dev"
