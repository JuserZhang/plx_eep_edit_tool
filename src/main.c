/*******************************************************************************
*   COPYRIGHT (C) 2018 MACROSAN, INC. ALL RIGHTS RESERVED.
* --------------------------------------------------------------------------
*  This software embodies materials and concepts which are proprietary and  
*  confidential to MACROSAN, Inc.                                         
*  MACROSAN distributes this software to its customers pursuant to the terms 
*  and conditions of the Software License Agreement contained in the text 
*  file software. This software can only be utilized if all terms and conditions 
*  of the Software License Agreement are accepted. If there are any questions, 
*  concerns, or if the Software License Agreement text file is missing please    
*  contact MACROSAN for assistance.                                       
* --------------------------------------------------------------------------
*   Date: 2019-06-26
*   Version: 2.0
*   Author: ZhangPeng
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "eepver.h"
#include "command.h"
#include "clour.h"

u32 eep_image_size=0;
int fd,ver_len = 6;//默认是 2字节ver + 4字节sn

void show_logo()
{
    printf(CLOUR_BEGIN,underline);
    printf("\n          welcome to the eeprom image file editor!         \n\n");
    printf(CLOUR_END);
    printf(CLOUR_BEGIN,font_green);
    printf(CLOUR_BEGIN,flash);
    printf(CLOUR_BEGIN,hightlight);
    printf("      ******  ******  *****   *****    *  *   *     *      \n");
    printf("      *       *       *    *  *    *  *    *  * * * *      \n");
    printf("      ******  ******  *****   *****   *    *  *  *  *      \n");
    printf("      *       *       *       *  *    *    *  *     *      \n");
    printf("      ******  ******  *       *    *   *  *   *     *      \n");
    printf(CLOUR_END);
    printf(CLOUR_BEGIN,underline);
    printf("                                                           \n\n");
    printf(CLOUR_END);

}
int main (int argc, char **argv)
{
    char *line, *cmd;
    char *file_name;
    char symbol[30]={0};
    struct stat fstatus;

 //   int len=0;

    u32 val,fsize;

    if(argc!=2)
    {
       printf("main eep_file\n");
       return 0;
    }
    
    sprintf(symbol,"[%s]>",argv[0]);
    file_name=argv[1];

    fd = open(file_name,O_RDWR);
    if(fd < 0)
    {
        printf("open file fail\n");
        return -1;
    }

    /*获取文件的长度*/
	if(stat(file_name, &fstatus) < 0)
	{
		printf("get file size failed\n");
		goto err;
	}
	else
	{
		fsize = fstatus.st_size;
	}
    printf(CLOUR_BEGIN,font_yellow);  
    printf("Open %s(%dB)\n",file_name,fsize);
   
/*
//----------------------------------------------------
    val = eepread32(fd,0);
    len = ( val >> 16 ) + 8;
    val = eepread32(fd, len / 4);

    if( 0 != is_data_size_multiple_of_four(fd))
    {
        ver_len = ( val >> 24 ) & 0xFF;        
    }
    else
    {
        ver_len = ( val >> 8 ) & 0xFF;
    }
    
    printf("ver_len=%x\n",ver_len);

-------------------------------------------------------
*/
    val = eepread32(fd,0);
    //printf("val:%x",val);
    if(0x5a != (val & 0xff))
    {
        printf("Err:This is not a eeprom image file!\n");
        printf(CLOUR_END);
        goto err;
    }
    eep_image_size = (val >> 16) + 8 + (2 + ver_len);

    if(fsize == eep_image_size)//包含CRC校验位和版本信息位
    {
        printf("Contains 4 byte CRC and %d byte version information!\n",ver_len + 2);
    }
    else if(fsize == eep_image_size - (ver_len + 2))//包含CRC校验位，缺少版本信息
    {  
        printf("Lack of %d byte version information!\n",ver_len + 2);
    }
    else if(fsize == eep_image_size - 8 - (ver_len + 2))//缺少CRC校验位，缺少版本信息
    {
        printf("Lack of 4 byte CRC and %d byte version information!\n",ver_len + 2);
    }
    else
    {
        if(fsize > ((val >> 16)+8))//版本号错乱或不完整
        {
            printf("Incorrect or incomplete version number\n");
        }
        else
        {
            printf("Err:This is not a eeprom image file!\n");
            printf(CLOUR_END);
            goto err;
        }      
    }   
    printf(CLOUR_END);
    initialize_readline();    /* Bind our completer. */
    read_history(NULL); //从指定的文件中读取历史记录,如果参数为NULL默认的文件是：~/.history
    show_logo();
    while(1)
    {        
        line = readline (symbol);

        if (!line)
            break;

         cmd= stripwhite (line);
         if (*cmd)
         {
             add_history(cmd);
             write_history(NULL);//将历史记录存入指定的文件,如果参数为NULL默认的文件是：~/.history
             execute_cmd(cmd);
         }

        free(line);
    }
    
    close(fd);
    return 0;
err:
	close(fd);
	return -1;    
}
