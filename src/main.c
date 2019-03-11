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
*   Date: 2018-12-12
*   Version: 1.3
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
int fd;

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

    /*��ȡ�ļ��ĳ���*/
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

    val=eepread32(fd,0);
    eep_image_size= (val >> 16)+8;

    if(fsize == eep_image_size)//����CRCУ��λ�Ͱ汾��Ϣλ
    {
        printf("Contains 4 byte CRC and 4 byte version information!\n");
    }
    else if(fsize == eep_image_size-4)//����CRCУ��λ��ȱ�ٰ汾��Ϣ
    {
        printf("Lack of 4 byte version information!\n");
    }
    else if(fsize == eep_image_size-8)//ȱ��CRCУ��λ��ȱ�ٰ汾��Ϣ
    {
        printf("Lack of 4 byte CRC and 4 byte version information!\n");
    }
    else
    {
        printf("Err:This is not a eeprom image file!\n");
        printf(CLOUR_END);
        goto err;
    }   
    printf(CLOUR_END);
    initialize_readline();    /* Bind our completer. */
    read_history(NULL); //��ָ�����ļ��ж�ȡ��ʷ��¼,�������ΪNULLĬ�ϵ��ļ��ǣ�~/.history
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
             write_history(NULL);//����ʷ��¼����ָ�����ļ�,�������ΪNULLĬ�ϵ��ļ��ǣ�~/.history
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
