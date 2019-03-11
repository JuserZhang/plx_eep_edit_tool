#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eepver.h"

/*���������������Ҫ��plxli�еı���һ��*/
eep_serial_number_t eep_serial_number_info []={//һ���ֽ���Ч
		{"07020008",0x01},//MS3000
		{"07020017",0x02},//MS5000
		{"07020046",0x03},//MS5500   
		{"07020068",0x04},//MS7000
		{"07020139",0x05},
		{"07020097",0x06},
		{"07020140",0x07},
		{"07020099",0x08},//MS3000G2
		{"07020171",0x09},//MS3000G2-FT
		{"07020225",0x0a},//MS7000-Mach
		{"07020224",0x0b},//MS5500G2
		{"07020229",0x0c},//MS7000G2
		{"07020087",0x0d},//PCIex4 Swap
		{"07020116",0x0e},//PCIex8 Swap 
		{"07020173",0x0f},//PCIex4 Vnic
		{"07020164",0x10},//PCIex4 Fabric 
		{"07020191",0x11},//PCIex4 Switch
		{"07020024",0x12},
		{"07020069",0x13},
		{"07020192",0x14},
		{"07020019",0x15},
		{"07020074",0x16},
		{"07020119",0x17}
};

eep_version_t eep_version_info []={//һ���ֽ���Ч
        {"V100",0x0},
        {"V101",0x1},
		{"V102",0x2},
		{"V103",0x3},
		{"V104",0x4},
		{"V105",0x5},
		{"V106",0x6},
		{"V107",0x7},
        {"V108",0x8},
        {"V109",0x9},
        {"V110",0xa}
};

void traversal_eep_serial_number_info()
{
    int i,j,size;
    size = sizeof(eep_serial_number_info)/sizeof(eep_serial_number_t);
    
    for(i=0;i<size;)
    {
        for(j=0;j< 4&& i<size;j++,i++)
        {
            printf("%s ",eep_serial_number_info[i].serial_number);
        }
       printf("\n");
    }
}

void traversal_eep_version_info()
{
     int i,j,size;
     size = sizeof(eep_version_info)/sizeof(eep_version_t);
     for(i=0;i<size;)
     {
        for(j=0;j<4 && i<size;j++,i++)
        {
            printf("%s ",eep_version_info[i].version);
        }
        printf("\n");        
     }
}

//�ɹ��ҵ�����1�����߷���0
int get_eep_serial_number(const u32 sn,char *serial_number)
{
    int size=0;
    int i,flag=0;
    size = sizeof(eep_serial_number_info)/sizeof(eep_serial_number_t);
    for(i=0;i<size;i++)
    {
        if(eep_serial_number_info[i].sn == sn)
        {
            strcpy(serial_number,eep_serial_number_info[i].serial_number);
            flag=1;
            break;
        }
    }
    return flag;
}

int get_eep_version(const u32 ver, char *version)
{
    int size=0;
    int i,flag=0;
    size = sizeof(eep_version_info)/sizeof(eep_version_t);
    for(i=0;i<size;i++)
    {
        if(eep_version_info[i].ver_magic == ver)
        {
            strcpy(version,eep_version_info[i].version);
            flag=1;
            break;
        }
    }
    return flag;
}

//�ɹ������ҵ���ver��ʧ�ܷ���-1
int get_eep_version_magic(const char *version)
{
    int size=0;
    int i;
    int ret=-1;
    
    size = sizeof(eep_version_info)/sizeof(eep_version_t);
    for(i=0;i<size;i++)
    {
        if(!strcmp(eep_version_info[i].version, version))
        {
            
            ret = eep_version_info[i].ver_magic;
            break;
        }
    }
    return ret;
}

int get_eep_serial_number_magic(const char *serial_number)
{
    int size=0;
    int i;
    int ret=-1;
    
    size = sizeof(eep_serial_number_info)/sizeof(eep_serial_number_t);
    for(i=0;i<size;i++)
    {
        if(!strcmp(eep_serial_number_info[i].serial_number, serial_number))
        {
            
            ret = eep_serial_number_info[i].sn;
        }
    }
    return ret;
}

u32 eepread32(int fd,u32 offset)
{
	unsigned char ch=0;
    u32 val=0;
    int i,size=0;
    int ret;
   
    for(i=offset*4+3;i >= offset*4;i--)
    {

        ret=lseek(fd,i,SEEK_SET);
        if(-1==ret)
        {
            break;
        }
        size=read(fd,&ch,1);
        if(1 != size)
        {
            ch=0x0; 
        }
        val=(val << 8 )| ch;       
    }
    return val;
}

void eepwrite32(int fd,u32 offset, u32 val)
{
	unsigned char ch=0;
    int i,j;
    int ret;
    //printf("val:%08x\n",val);
    for(i=offset*4+3,j=3;i >= offset*4 ;i--,j--)
    {
        if(i< eep_image_size)
        {
            ret=lseek(fd,i,SEEK_SET);
            if(-1==ret)
            {
                break;
            }
            ch = (val >> (8*j))& 0xff;
            write(fd,&ch,1);
        }         
    } 
}

/*�Ѵ����dword�ֽڰ汾��Ϣд��eeprom�ļ�*/
void modify_version(int fd,u32 data)
{
    u32 offset,val,magic;

    magic = (0x5a << 24) | (data & 0xffff); //�Ѵ������кźͰ汾��Ϣ��dword����0x5a�����һ���ֽ�ʹ�ð汾��Ϣ��Ч
        
    offset = get_eep_version_offset(fd);
    if(is_version_divided_two_parts(fd))
    {
        val = eepread32(fd,offset); //��ȡ��Ҫ���ǵ�����
        val &= 0xffff;       
        val |= (magic & 0xffff) << 16; //ȷ�����Ḳ��У��λ��2byte        
        eepwrite32(fd,offset,val); //��һ����д��2 byte���ֽ�

        offset++;     
 
        val = eepread32(fd,offset);
        val &= 0xffff0000;
        val |= magic >> 16;
        eepwrite32(fd,offset,val); //�ڶ�����д��2 byte���ֽ�
    }
    else
    {
         eepwrite32(fd,offset,magic);
    }
}

/*�ж�������ֽڵİ汾��Ϣ�Ƿ���Ҫ�������ζ�/д*/
int is_version_divided_two_parts(int fd)
{
    int residue = 0;
    u32 val,eep_image_size; 

    val=eepread32(fd,0);
    eep_image_size= (val >> 16)+8;//���ֽڵ�У���룬���ֽڵİ汾��Ϣ�������ֽ�
    residue=eep_image_size%4;
    
    return residue;
}


/*����eerpom�ļ��汾��Ϣ��ƫ��,�ڴ�֮ǰ����ȷ��������ֽڵİ汾��Ϣ�Ƿ���Ҫ�������ζ���д��
 *�Ӷ�ȷ����Ҫƫ��һ�λ����Σ��ú������Ƿ��ص�һ�ε�ƫ��*/
u32 get_eep_version_offset(int fd)
{
    int quotient = 0;
    u32 val,eep_image_size,offset; 
    
    val=eepread32(fd,0);
    eep_image_size= (val >> 16)+8;
    quotient=eep_image_size/4;
    offset=quotient-1;

    return offset;
}

/*��ȡeeprom�����ļ���������ֽڣ���dword����������İ汾��Ϣ*/
u32 read_version_magic(int fd)
{
    u32 ver = 0, ver_part1=0, ver_part2=0;
    u32 val,offset;  
    
    offset=get_eep_version_offset(fd);
    if(is_version_divided_two_parts(fd))
    {        
        val=eepread32(fd,offset);//val��������ֽڰ����汾��Ϣһ����
        ver_part1 = val >> 16;
        offset++;
        val=eepread32(fd,offset);//val��������ֽڰ����汾��Ϣ����һ����
        ver_part2 = val & 0xffff;
        ver = (ver_part2 << 16) | ver_part1;//�ϲ������ֵİ汾��ϢΪһ��dword                      
    }
    else//���һ��dword�ֽ��а����˰汾��Ϣ����һ�ζ�ȡ�ꡣ
    {
        ver=eepread32(fd,offset);
    }   

    return ver;
}


/****************************************************************   
 *      ___ ___ ___ ___ ___ ___ ___ ___    
 *     | 5 | a | x | x |   |   |   |   |
 *     |___|___|___|___|___|___|___|___|
 *     |-------------magic-------------| 
 *     | 1byte | 1byte | 1byte | 1byte |
 *       check  reserve    sn     ver
 *                               
 *****************************************************************/
int  show_version(u32 magic)
{
    char serial_number[25]={0},version[25]={0};
    u32 ver,sn,check;

    check = ( magic >> 24 );
    if(0x5a != check)//dword�����һ���ֽ�Ϊ0x5a��ʾ�汾��Ϣ����
    {
        printf("version: NULL\n");
        return -1;
    }
    
    ver = magic & 0xff;
    sn = ( magic >> 8 ) & 0xff; 
    
    if(1 != get_eep_version(ver, version))
    {
        printf("Invalid version\n");
        return -1;
    }
    
    if(1 != get_eep_serial_number(sn,serial_number))
    {    
        printf("Invalid serial number!\n");
        return -1;     
    }
    
    printf("version: %s_%s\n",serial_number,version);
    return 0;
}

void show_eep_image_dump(int fd)
{
    u32 val=0,offset=0,max_offset;
    int i=0;
    int raw_data1=0,raw_data2=0,raw_data3=0,raw_data4=0;

    max_offset=get_eep_version_offset(fd);
    
    for(offset=0;offset<=max_offset;)
    {
        printf("%08xh:",offset*4);
        for(i=0;i<4 && offset<=max_offset;i++,offset++)
        {
            val=eepread32(fd,offset);
            raw_data1 = val & 0xff;
            raw_data2 = (val >> 8) & 0xff;
            raw_data3 = (val >> 16) & 0xff;
            raw_data4 = (val >> 24) & 0xff;          
            printf(" %02x %02x %02x %02x",raw_data1,raw_data2,raw_data3,raw_data4);
        }
        if((0 < is_version_divided_two_parts(fd)) && (offset > max_offset))//��������ֽ�
        {
            val=eepread32(fd,offset);
            raw_data1 = val & 0xff;
            raw_data2 = (val >> 8) & 0xff;
            printf(" %02x %02x",raw_data1,raw_data2);
        }
        printf("\n");
    }  
}

