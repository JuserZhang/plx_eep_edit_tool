#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eepver.h"

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

//判断eep文件大小是否是4的整数倍
int is_data_size_multiple_of_four(int fd)
{
    int residue = 0;
    u32 val,eep_image_size; 

    val=eepread32(fd,0);
    eep_image_size= (val >> 16) + 10 + ver_len;
    residue=eep_image_size%4;
    
    return residue;
}

/*计算eerpom文件版本信息的偏移,在此之前必须确定最后四字节的版本信息是否需要进行两次读、写，
 *从而确定需要偏移一次或两次，该函数总是返回第一次的偏移*/
u32 get_eep_version_offset(int fd)
{
    int quotient = 0;
    u32 val,eep_size,offset; 
    
    val=eepread32(fd,0);
    eep_size= (val >> 16) + 8 + 2 + ver_len;
    quotient=eep_size/4;
    offset=quotient-2;

    return offset;
}

/*把传入的版本信息和序列号写入eeprom文件*/
void modify_version(int fd, u64 sn, u32 ver)
{
    u32 offset,val;
    u32 a,b,c,d;
    u32 data;

    offset = get_eep_version_offset(fd);

    if(6 == ver_len )
    {   
        if(0 != is_data_size_multiple_of_four(fd))//不能被4整除
        {
            val = eepread32(fd,offset); //读取需要覆盖的数据
        
            a = val & 0xff;
            b = (val >> 8) & 0xff;
            c = 0x5a;
            d = ver_len; 
            data = (d << 24) | (c << 16) | (b << 8) | a;             
            eepwrite32(fd,offset,data); //第一次写入 5a06
            offset++;     
 
            a = (ver >> 8) & 0xff;
            b = ver & 0xff;
            c = (sn >> 24) & 0xff;
            d = (sn >> 16) & 0xff;
            data = (d << 24) | (c << 16) | (b << 8) | a;  
            eepwrite32(fd,offset,data); //第二次写入 sn前2字节
            offset++;

            a = (sn >> 8) & 0xff;
            b = sn & 0xff;
            data =((b << 8) | a) & 0x00ffff;
            eepwrite32(fd,offset,data); //第三次写入 sn后2字节   
        }
        else
        {
            a = 0x5a;
            b = ver_len;
            c = (ver >> 8) & 0xff;
            d = ver & 0xff;

            data = (d << 24) | (c << 16) | (b << 8) | a;  
            eepwrite32(fd,offset,data);//第一次写入ver
            offset++; 

            a = (sn >> 24) & 0xff;
            b = (sn >> 16) & 0xff;
            c = (sn >> 8) & 0xff;
            d = sn & 0xff;
            data = (d << 24) | (c << 16) | (b << 8) | a;  
            eepwrite32(fd,offset,data);//第二次写入sn
        }
    }
    else
    {
        //something to do 
    }
}

int read_version(int fd,u32 *ver ,u64 *sn)
{
    u32 val,offset;  
    u32 a,b,c,d;    

    int flag = 0;
        
    if(6 == ver_len)
    {
        offset=get_eep_version_offset(fd);
        val=eepread32(fd,offset);

        if(0 != is_data_size_multiple_of_four(fd))
        {          
              a = (val >> 16) & 0xff;

              if(0x5a == a)
              {
                   offset++;
                   val=eepread32(fd,offset);
                   a = val & 0xff;
                   b = (val >> 8) & 0xff;
                   
                   *ver =((a << 8) | b ) & 0xffff;

                   c = (val >> 16) & 0xff;//0x07
                   d = (val >> 24) & 0xff;//0x02
                   offset++;
                   val=eepread32(fd,offset);
                   a = val & 0xff;//0x02
                   b = (val >> 8) & 0xff;//0x47
                   
                   *sn = c << 24 | d << 16 | a << 8 | b;
              } 
              else
              {
                   flag = 1;
              }
        }
        else//只需要读取两次
        {
             if(0x5a == (val & 0xff))
             {
                 a = (val >> 16) & 0xff;//0x01
                 b = (val >> 24) & 0xff;

                 *ver = ((a << 8) | b) & 0xffff;
                 offset++;
                 val = eepread32(fd,offset);

                 a = val & 0xff;//0x07
                 b = (val >> 8) & 0xff; //0x02
                 c = (val >> 16) & 0xff;//0x00
                 d = (val >> 24)& 0xff;//0x69

                 *sn = a << 24 | b << 16 | c << 8 | d;  
             }
             else
             {
                 flag = 1;
             }
        }   

    }
    else
    {
         //something to do.
    }
    return flag;
}

void show_eep_image_dump(int fd)
{
    u32 val=0,offset=0,max_offset;
    int i=0;
    int raw_data1=0,raw_data2=0,raw_data3=0,raw_data4=0;

    max_offset=get_eep_version_offset(fd)+1;
    
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
        if((0 != is_data_size_multiple_of_four(fd)) && (offset > max_offset))//最后两个字节
        {
			int eep_size;
            eep_size = (eepread32(fd, 0) >> 16) + 10 + ver_len;
            if(eep_size % 16 == 2)
            {
                printf("\n%08x:",offset*4);
            }
            val=eepread32(fd,offset);
            raw_data1 = val & 0xff;
            raw_data2 = (val >> 8) & 0xff;
            printf(" %02x %02x",raw_data1,raw_data2);
        }
        printf("\n");
    }  
}

