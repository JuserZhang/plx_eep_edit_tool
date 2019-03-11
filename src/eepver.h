#ifndef __EEPVER_H
#define __EEPVER_H

#define NAME_SIZE 20
#define u32 unsigned int
extern u32 eep_image_size;

typedef struct eep_serial_number{
    const char serial_number[NAME_SIZE];  //描述eeprom文件适用的serial_number
    u32  sn;             //写入eeprom中的sn                 
}eep_serial_number_t;

typedef struct eep_version{
    const char version[NAME_SIZE];      //描述eeprom文件的版本
    u32  ver_magic;                     //写入eeprom中的version ID                   
}eep_version_t;


void traversal_eep_version_info();
void traversal_eep_serial_number_info();
int get_eep_serial_number(const u32 sn, char *serial_number);
int get_eep_serial_number_magic(const char *serial_number);
int get_eep_version(const u32 ver, char *version);
int get_eep_version_magic(const char *version);

u32 eepread32(int fd,u32 offset);
void eepwrite32(int fd,u32 offset, u32 val);

void show_eep_image_dump(int fd);
void modify_version(int fd,u32 data);
int is_version_divided_two_parts(int fd);
u32 get_eep_version_offset(int fd);
u32 read_version_magic(int fd);
int show_version(u32 magic);

#endif
