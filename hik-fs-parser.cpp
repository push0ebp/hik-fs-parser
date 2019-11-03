#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//https://google.github.io/styleguide/cppguide.html
#define STR_LEN 256

struct HIK_FS_HEADER //master header
{
    uint8_t f1[0x210];
    uint8_t signature[0x20];
    uint8_t f2[0x18];
    uint64_t capacity;
    uint8_t f3[0x10];
    uint64_t logs_offset;
    uint64_t logs_size;
    uint8_t f4[0x8]; 
    uint64_t data_area_offset;
    uint8_t f5[0x8]; 
    uint64_t data_block_size;
    uint64_t num_data_blocks;
    uint64_t btree1_offset;
    uint64_t btree1_size;
    uint64_t btree2_offset;
    uint64_t btree2_size;
    uint8_t f6[0x38]; 
    uint64_t system_init_time;   
};

class HikFs 
{
  private:
    char filename_[STR_LEN];
    FILE *fp_; 
    HIK_FS_HEADER fs_header;
  public: 
    //HikFs() : filename_{0,}, fp_(NULL) 
    HikFs(char *filename) : fs_header{0}
    {
        strncpy(filename_, filename, sizeof(filename_) - 1);
        fp_ = fopen(filename, "rb");
        if (!fp_)
            throw "Cannot open image file";
        fread((void*)&fs_header, sizeof(fs_header), 1, fp_);
    }

    void PrintFsHeader()
    {
        printf("Signature : %s\n", fs_header.signature);
        printf("Capacity : %p\n", fs_header.capacity);
        printf("System Logs Offset: %p\n", fs_header.logs_offset);
        printf("System Logs Size: %p\n", fs_header.logs_size);
        printf("Video Data Area Offset: %p\n", fs_header.data_area_offset);
        printf("Data Block Size: %p\n", fs_header.data_block_size);
        printf("Total of Data Blocks: %p\n", fs_header.num_data_blocks);
        printf("HIKBTREE1 Offset: %p\n", fs_header.btree1_offset);
        printf("HIKBTREE1 Size: %p\n", fs_header.btree1_size);
        printf("HIKBTREE2 Offset: %p\n", fs_header.btree2_offset);
        printf("HIKBTREE2 Size: %p\n", fs_header.btree2_size);
        printf("System Initialized Time : %d\n", fs_header.system_init_time);
    }
};

int main()
{
    struct HIK_FS_HEADER fs_header;
    HikFs hikfs("/mnt/d/hik/images/hik.img");
    hikfs.PrintFsHeader();
    return 0;
}