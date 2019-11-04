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

struct HIK_BTREE
{
    uint8_t f1[0x10];
    uint8_t signature[0x10];
    uint8_t f2[0x1c];
    uint32_t created_time;
    uint64_t footer_offset;
    uint8_t f3[0x8];
    uint64_t page_list_offset;
    uint64_t first_page_offset;
};

struct HIK_PAGE_LIST
{
    uint8_t f1[0x10];
    uint32_t num_pages;
    uint8_t f2[0x4];
    uint64_t first_page_offset;
    uint8_t f3[0x40];
};

struct HIK_PAGE_LIST_ENTRY
{
    uint64_t offset;
    uint8_t f1[0x9];
    uint8_t channel;
    uint8_t f2[0x6];
    uint32_t start_time;
    uint32_t end_time;
    uint64_t first_data_block_offset;
    uint8_t f3[0x8];
};

struct HIK_DATA_BLOCK_ENTRY
{
    uint8_t f1[0x8];
    uint64_t exist;
    uint8_t f2;
    uint8_t channel;
    uint8_t f3[0x6];
    uint32_t start_time;
    uint32_t end_time;
    uint64_t first_data_block_offset;
    uint8_t f4[0x8];
};

struct HIK_FOOTER
{
    uint64_t last_page_offset;
};

class HikFs 
{
  private:
    char filename_[STR_LEN];
    const int kNumBtrees_ = 2;
    struct HIK_BTREE btrees_[2];
    FILE *fp_; 
    HIK_FS_HEADER fs_header_;
  public: 
    HikFs(char *filename) : fp_(NULL), fs_header_{0}, btrees_{0}
    {
        strncpy(filename_, filename, sizeof(filename_) - 1);
        fp_ = fopen(filename, "rb");
        if (!fp_)
            throw "Cannot open image file";
        
        fread((void*)&fs_header_, sizeof(fs_header_), 1, fp_);
        ReadAt((void*)&btrees_[0], fs_header_.btree1_offset,  sizeof(btrees_[0]));
        ReadAt((void*)&btrees_[1], fs_header_.btree2_offset,  sizeof(btrees_[1]));

    }

    size_t ReadAt(void *buf, long offset, size_t size)
    {
        fseek(fp_, offset, SEEK_SET);
        return fread(buf, size, 1, fp_);
    }

    void PrintFsInfo()
    {
        printf("============== FS INFO ==============\n");
        printf("Signature : %s\n", fs_header_.signature);
        printf("Capacity : %p\n", fs_header_.capacity);
        printf("System Logs Offset: %p\n", fs_header_.logs_offset);
        printf("System Logs Size: %p\n", fs_header_.logs_size);
        printf("Video Data Area Offset: %p\n", fs_header_.data_area_offset);
        printf("Data Block Size: %p\n", fs_header_.data_block_size);
        printf("Total of Data Blocks: %p\n", fs_header_.num_data_blocks);
        printf("HIKBTREE1 Offset: %p\n", fs_header_.btree1_offset);
        printf("HIKBTREE1 Size: %p\n", fs_header_.btree1_size);
        printf("HIKBTREE2 Offset: %p\n", fs_header_.btree2_offset);
        printf("HIKBTREE2 Size: %p\n", fs_header_.btree2_size);
        printf("System Initialized Time : %d\n", fs_header_.system_init_time);
    }

    
    void PrintBtree(struct HIK_BTREE *btree)
    {
        printf("============== BTREE ==============\n");
        printf("Signature : %s\n", btree->signature);
        printf("Created Time : %d\n", btree->created_time);
        printf("Footer Offset: %p\n", btree->footer_offset);
        printf("Page List Offset: %p\n", btree->page_list_offset);
        printf("First Page Offset: %p\n", btree->first_page_offset);
    }

    void PrintPageList(struct HIK_PAGE_LIST *page_list)
    {
        printf("Total of Pages : %d\n", page_list->num_pages);
        printf("First Page Offset : %p\n", page_list->first_page_offset);
    }

    void PrintPageListEntry(struct HIK_PAGE_LIST_ENTRY *entry)
    {
        printf("============== PAGE LIST ENTRY ==============\n");
        printf("Page Offset : %p\n", entry->offset);
        printf("Channel : %d\n", entry->channel);
        printf("Start/End Time : %d / %d\n", entry->start_time, entry->end_time);
        printf("First Data Block Offset : %p\n", entry->first_data_block_offset);
    }

    void PrintDataBlockEntry(struct HIK_DATA_BLOCK_ENTRY *entry)
    {
        printf("============== DATA BLOCK ENTRY ==============\n");
        printf("Existence : %d\n", entry->exist);
        printf("Channel : %d\n", entry->channel);
        printf("Start/End Time : %d / %d\n", entry->start_time, entry->end_time);
        printf("First Data Block Offset : %p\n", entry->first_data_block_offset);
    }
    
    void PrintFooter(struct HIK_FOOTER *footer)
    {
        printf("============== FOOTER ==============\n");
        printf("Last Page Offset : %p\n", footer->last_page_offset);
    }
    
    void PrintDataBlock(uint64_t data_block_offset)
    {
        printf("============== DATA BLOCK ==============\n");
        for (int i = 0; i < 16; i++)
            printf("%02X ");
    }

    void EnumeratePages(struct HIK_BTREE *btree)
    {
        struct HIK_PAGE_LIST page_list;
        ReadAt(&page_list, btree->page_list_offset, sizeof(page_list));
        PrintPageList(&page_list);
        for (int i = 0; i < page_list.num_pages; i++)
        {
            struct HIK_PAGE_LIST_ENTRY page_list_entry;
            ReadAt(&page_list_entry, btree->page_list_offset + sizeof(page_list) + sizeof(page_list_entry) * i, sizeof(page_list_entry));
            PrintPageListEntry(&page_list_entry);
            
        }

    }

    void Enumerate()
    {
        for (int i = 0; i < kNumBtrees_; i++)
        {
            PrintBtree(&btrees_[i]);
            EnumeratePages(&btrees_[i]);
        }
    }
};

int main()
{
    HikFs hikfs("/mnt/d/hik/images/hik.img");
    hikfs.PrintFsInfo();
    hikfs.Enumerate();
    return 0;
}