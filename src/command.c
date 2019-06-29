#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "clour.h"

command_t commands[] = {
  { "dl", com_dl, "dl [offset]","Display EEPROM file value" },
  { "el", com_el, "el [offset] [value]","Modify EEPROM file value"},
  { "swc", com_swc, "swc [filename]","Save it wihtout crc as filename"},
  { "modver", com_modver, "modver [SN] [version]","Modify EEPROM file version" },
  { "dump", com_dump, "dump","Show EEPROM file raw data format" },
  { "version", com_version, "version","Display version infomation" },
  { "quit", com_quit, "quit","Quit" },
  { "help", com_help, "help", "May I help you?" },
  { "?", com_help, "?","Synonym for `help'" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL,(char *)NULL}
};


/*命令执行函数*/
int com_dump(char *para)
{
    if(strcmp(para,"")!=0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("dump:the command takes no arguments\n");
        printf(CLOUR_END);
        return -1;
    }

    printf("----------------eeprom image raw data format---------------\n");
    show_eep_image_dump(fd);
    printf("-----------------------------end---------------------------\n");
}

int com_swc(char *para)
{
    int fd_tmp, ret, i, quotient = 0, residue = 0;
    u32 eep_size,buff;
    char dir_buf[256];
    char fname[128];

    if(strcmp(para,"")==0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("Please enter the file name to save as.\n");
        printf(CLOUR_END);
        return -1;
    }

    sprintf(fname,"%s",para);

    //获取当前工作目录
	getcwd(dir_buf, sizeof(dir_buf));
	sprintf(dir_buf,"%s/%s",dir_buf,fname);

	fd_tmp = open(dir_buf , O_RDWR|O_NDELAY|O_CREAT|O_EXCL);
	if(fd_tmp < 0) 
    {
		printf("File already exist, choose another\n");
		return 1;
	}
    eep_size = (eepread32(fd, 0) >> 16) + 4;//CRC之前的所有数据

    quotient = eep_size / 4;
	residue  = eep_size % 4;

    for(i = 0; i < quotient; i++)
    {
		buff = eepread32(fd, i);
		ret = write(fd_tmp, &buff, 0x4);
		if(4 != ret)
        {
			printf("Write file failed-%d\n",i);
        }
	}

    if(residue)
    {
		buff = eepread32(fd, i);
		ret = write(fd_tmp, &buff, 0x2);
		if(2 != ret)
        {
			printf("Write file failed-%d\n",i);
        }
	}
    printf("-----------------------------------------------------------\n");
    printf("Save eeprom without crc to %s(%dB) successful\n",dir_buf,eep_size);
    printf("-----------------------------------------------------------\n");
    close(fd_tmp);
    return 0;
}

int com_modver(char *para)
{
    u32 ver=0;
    u64 sn=0;

    char *p=para;
    char serial_number[128],version[128];
    int i=0,j=0,len;

    if(strcmp(para,"")==0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("modver [serial_nameber] [version]\n");
        printf("For example:modver 07020017 101\n");
        printf(CLOUR_END);
        return -1;
    }


   len = strlen(para);

   for(i=0;i<len;i++)//serial_nameber
   {
        if(*p == ' ')
        {
            p++;
            serial_number[j]='\0';
            break;
        }
        serial_number[j]=*p;
        j++;
        p++;
   }

   if(i==len)
   {
        printf(CLOUR_BEGIN,font_red);
        printf("modver:too few parameters\n");
        printf(CLOUR_END);
        return -1;
   }

   for(j=0;i<len;i++,j++)//version
   {
        if(*p == ' ')
        {
            p++;
            version[j]='\0';
            break;
        }
        version[j]=*p;
        p++;
   }

    if(i!=len)
    {
         printf(CLOUR_BEGIN,font_red);
         printf("modver:too many parameters\n");
         printf(CLOUR_END);
         return -1;
    }

     sn = strtoul(serial_number,NULL,16);
     ver = strtoul(version,NULL,16);
     modify_version(fd,sn,ver);

     printf("-----------------------------------------------------------\n");
     printf("Modified version success!\n");
     printf("Current version:%s_V%s\n",serial_number,version);
     printf("-----------------------------------------------------------\n");
     fsize = eep_image_size;
     return 0;
}

int com_version(char *para)
{
    u32 ver = 0;
    u64 sn  = 0;

    if(strcmp(para,"")!=0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("version:the command takes no arguments\n");
        printf(CLOUR_END);
        return -1;
    }

    if(0 == read_version(fd,&ver,&sn))
    {
        printf("-----------------------------------------------------------\n");
        printf("version: %08x_V%x\n",sn,ver);
        printf("-----------------------------------------------------------\n");
    }
    else
    {
        printf("-----------------------------------------------------------\n");
        printf("The version number is invalid\n");
        printf("-----------------------------------------------------------\n");
    }
    return 0;
}

int com_dl(char *para)
{
   int len,n=0;
   char *p=para;
   u32 offset,val;

   if(strcmp(para,"")==0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("dl [offset]\n");
        printf("For example:dl 1\n");
        printf(CLOUR_END);
        return -1;
    }

   len = strlen(para);
    while(len--)
    {
        if(*p>='0' && *p<='9')
        {
            p++;
            n++;
        }
    }

    len = strlen(para);

    if(len != n)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("offset is illegal!\n");
        printf(CLOUR_END);
        return -1;
    }

   offset=strtoul(para,NULL,10);//将字符串转换成10进制无符号长整型数

   if(offset>=0 && offset<= eep_image_size/4-1)
    {
        val=eepread32(fd,offset);
        printf("-----------------------------------------------------------\n");
        printf("%08xh: %08x\n",offset*4,val);
        printf("-----------------------------------------------------------\n");
    }
   else
    {
        printf(CLOUR_BEGIN,font_red);
        printf("offset:[0,%d]\n",eep_image_size/4-1);
        printf(CLOUR_END);
    }
    return 0;
}

int com_el(char *para)
{
    int len,n=0;
    int i=0,j=0;
    char *p=para;
    char offset_srt[128]={0},value_str[128]={0};
    u32 val,offset,data;

   if(strcmp(para,"")==0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("el [offset] [value]\n");
        printf("For example:el 1 1234\n");
        printf(CLOUR_END);
        return -1;
    }

   len = strlen(para);

   for(i=0;i<len;i++)//offset
   {
        if(*p == ' ')
        {
            p++;
            offset_srt[j]='\0';
            break;
        }
        offset_srt[j]=*p;
        j++;
        p++;
   }

   if(i==len)
   {
        printf(CLOUR_BEGIN,font_red);
        printf("el:too few parameters\n");
        printf(CLOUR_END);
        return -1;
   }

   for(j=0;i<len;i++,j++)//value
   {
        if(*p == ' ')
        {
            p++;
            value_str[j]='\0';
            break;
        }
        value_str[j]=*p;
        p++;
   }

   if(i!=len)
   {
        printf(CLOUR_BEGIN,font_red);
        printf("el:too many parameters\n");
        printf(CLOUR_END);
        return -1;
   }

   n=0;
   p = offset_srt;//验证offset是否为十进制数字
   while(*p!='\0')
   {
       if(*p>='0' && *p<='9')
       {
            n++;
       }
       p++;
   }
   if(strlen(offset_srt)!=n)
   {
        printf(CLOUR_BEGIN,font_red);
        printf("offset is illegal!\n");
        printf(CLOUR_END);
        return -1;
   }

   n=0;
   p = value_str;
   while(*p!='\0')//验证value是否为16进制数字
   {
        if(isxdigit(*p))
        {
            n++;
        }
        p++;
   }
   if(strlen(value_str)!=n)
   {
        printf(CLOUR_BEGIN,font_red);
        printf("Value is illegal!\n");
        printf(CLOUR_END);
        return -1;
   }


   offset=strtoul(offset_srt,NULL,10);//将字符串转换成10进制无符号长整型数
   data=strtoul(value_str,NULL,16);//将字符串转换成16进制无符号长整型数

    if(offset>=0 && offset<= eep_image_size/4-1)
    {
        eepwrite32(fd,offset,data);
        printf("-----------------------------------------------------------\n");
        printf("Write to successful!\n");
        printf("%08xh:%08x\n",offset*4,data);
        printf("-----------------------------------------------------------\n");
    }
    else
    {
        printf(CLOUR_BEGIN,font_red);
        printf("offset:[0,%d]\n",eep_image_size/4-1);
        printf(CLOUR_END);
    }
    return 0;
}

int com_help(char *para)
{
    int i=0;
    if(strcmp(para,"")!=0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("help:the command takes no arguments\n");
        printf(CLOUR_END);
        return -1;
    }
    printf("-----------------------------------------------------------\n");
    while(commands[i].name)
    {
        printf("%-25s%s\n",commands[i].doc_name,commands[i].doc_desc);
        i++;
    }
    printf("-----------------------------------------------------------\n");
    return 0;
}

int com_quit(char *para)
{
    if(strcmp(para,"")!=0)
    {
        printf(CLOUR_BEGIN,font_red);
        printf("quit:the command takes no arguments\n");
        printf(CLOUR_END);
        return -1;
    }
    close(fd);
    exit(0);
}


/***************************************************************************/
char* dupstr(char *string)
{
  char *ret;
  ret = malloc (strlen (string) + 1);
  strcpy(ret, string);
  return (ret);
}

// clear up white spaces
char* stripwhite (char *string)
{
    register char *s, *t;

    for (s = string; whitespace (*s); s++)
        ;

    if (*s == 0)
        return (s);

    t = s + strlen (s) - 1;
    while (t > s && whitespace (*t))
        t--;

    *++t = '\0';

    return s;
}

command_t *find_command (char *name)
{
    register int i;

    for (i = 0; commands[i].name; i++)
        if (strcmp (name, commands[i].name) == 0)
            return (&commands[i]);
    return ((command_t *)NULL);
}

int execute_cmd (char *string)
{
    register int i;
    command_t *command;
    char *cmd;
    char *para;

    i = 0;
    while (string[i] && whitespace (string[i]))
        i++;

    cmd= string + i;

    while (string[i] && !whitespace (string[i]))
        i++;

    if (string[i])
        string[i++] = '\0';

    command = find_command (cmd);
    if (!command)
    {
        printf(CLOUR_BEGIN,font_red);
        printf ("%s: No such command.\n", cmd);
        printf(CLOUR_END);
        return (-1);
    }

    /* Get argument to command, if any. */
    while (whitespace (string[i]))
        i++;

    para = string + i;
    /* Call the function. */
    return ((*(command->func)) (para));
}

/*
* Generator function for command completion. STATE lets us know whether
* to start from scratch; without any state (i.e. STATE == 0), then we
* start at the top of the list.
*/
char* command_generator (const char *text, int state)
{
    static int list_index, len;
    char *name;

    /*
    * If this is a new word to complete, initialize now. This includes
    * saving the length of TEXT for efficiency, and initializing the index
    * variable to 0.
    */
    if (!state)
    {
         list_index = 0;
        len = strlen (text);
    }

    /* Return the next name which partially matches from the command list. */
    while (name = commands[list_index].name)
    {
         list_index++;

         if (strncmp (name, text, len) == 0)
             return (dupstr(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

/*
* Attempt to complete on the contents of TEXT. START and END bound the
* region of rl_line_buffer that contains the word to complete. TEXT is
* the word to complete. We can use the entire contents of rl_line_buffer
* in case we want to do some simple parsing. Return the array of matches,
* or NULL if there aren't any.
*/
char** fileman_completion (const char *text, int start, int end)
{
    char **matches;

    matches = (char **)NULL;

    /*
    * If this word is at the start of the line, then it is a command
    * to complete. Otherwise it is the name of a file in the current
    * directory.
    */
    if (start == 0)
        matches = rl_completion_matches (text, command_generator);

    return (matches);
}


/*
* Tell the GNU Readline library how to complete. We want to try to complete
* on command names if this is the first word in the line, or on filenames
* if not.
*/
void initialize_readline ()
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = ">>>";
    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = fileman_completion;
}

