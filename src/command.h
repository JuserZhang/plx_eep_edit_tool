#ifndef __COMMAND_H
#define __COMMAND_H

#define u32 unsigned int
#define u64 unsigned long

extern u32 eep_image_size;
extern int fd;

typedef struct{
    char *name;               
    rl_icpfunc_t *func;       
    char *doc_name;
    char *doc_desc;   
}command_t;

//ÃüÁîÖ´ĞĞº¯Êı
int com_dump(char *para);
int com_modver(char *para);
int com_version(char *para);
int com_dl(char *para);
int com_el(char *para);
int com_help(char *para);
int com_quit(char *para);

char* dupstr(char *s);
char* stripwhite (char *string);
command_t *find_command (char *name);
int execute_cmd (char *string);
char* command_generator (const char *text, int state);
char** fileman_completion (const char *text, int start, int end);
void initialize_readline ();

#endif
