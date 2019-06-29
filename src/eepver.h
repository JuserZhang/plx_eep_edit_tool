#ifndef __EEPVER_H
#define __EEPVER_H

#define NAME_SIZE 20
#define u32 unsigned int
#define u64 unsigned long

extern u32 eep_image_size,fsize;
extern int ver_len;

u32 eepread32(int fd,u32 offset);
void eepwrite32(int fd,u32 offset, u32 val);

int is_data_size_multiple_of_four(int fd);
u32 get_eep_version_offset(int fd);

void modify_version(int fd,u64 sn,u32 ver);
int read_version(int fd, u32* ver, u64* sn);
void show_eep_image_dump(int fd);

#endif
