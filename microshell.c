#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "dirent.h"

#define PATH_SIZE 128
#define SIZE 1024

bool main_flag=true;

void read_command(char command[]);
void get_command(char cmd[],char command[]);
int amount_of_args(char command[]);
void change_directory(int args,char path[]);
void fill_two_dimensial_array_of_args(int argc, char command[], char **arguments);
void exec_script(char **path);
void copy(int argc,char **arguments);
void cat(int argc,char **arguments);
void split(char *string,char **array2d, char separator);
int count_separator(char *string,char separator);
void exec_command(char **path,char **PATHS,int length);


int main()
{
    char path[PATH_SIZE]; /*aktualna sciezka*/
            
    while(main_flag)
    {

        /*command- cala komenda*/
        char *command= (char*)malloc(sizeof(char)*(SIZE+1));
        /*cmd - komenda bez argumentow*/
        char *cmd=(char*)malloc(sizeof(char)*(SIZE+1));; 
        
        getcwd(path,sizeof(path));
        printf("[%s]$ ", path);
        
        read_command(command);
        get_command(cmd,command);
        
        int argc=amount_of_args(command);/*liczba wprowadzonych argumentow, polecen (wyrazow)*/
        char *p[argc];/*tablica dwuwymiarowa z wprowadzonymi przez uzytkownika argumentami*/
        int i;
        
        for(i=0;i<=argc;i++)
        {
            p[i]=(char*)malloc(sizeof(char)*(SIZE+1));
            memset(p[i],'\0',SIZE+1);
        }
        fill_two_dimensial_array_of_args(argc,command,p);
        
        
        /*Obsluga polecen*/
        
        /*Wyjdz z programu*/
        if(!strcmp(cmd,"exit"))
        {
            /*printf("EXIT\n");*/
            main_flag=false;
        }
        /*Pomoc*/
        else if(!strcmp(cmd,"help"))
        {
            printf("MICROSHELL\n");
            printf("\nAuthor: Dominik Cupal\n");
            printf("***************\n");
            printf("Zaimplementowane komendy:\n");
            printf("exit - wyjscie z programu\n");
            printf("cd - zmiana katalogu\n");
            printf("cp - kopiowanie plikow\n");
            printf("cat - czytanie plikow\n");
            printf("help - wyswietlenie pomocy\n\n");
        }
        /*Zmien katalog*/
        else if(!strcmp(cmd,"cd"))
        {
            change_directory(argc,p[1]);
        }
        /*Wykonaj skrypt*/
        else if((cmd[0]=='.')&&(cmd[1]=='/'))
        {
            p[argc]=NULL;
            exec_script(p);
        }
        /*Kopiowanie plikow*/
        else if(!strcmp(cmd,"cp"))
        {            
            copy(argc-1,p);
        }
        /*konkatenacja,wyswietlenie zawartosci plikow*/
        else if(!strcmp(cmd,"cat"))
        {
            cat(argc-1,p);
        }
        /*wykonanie innych komend, jesli jakas komenda nie znajduje sie w ktoryms z katalogow
         to zostanie wyswietlony komunikat o nie znalezieniu komendy*/
        else
        {
            /* printf("%s: command not found\n",cmd); */
                    
            /*s - liczba separatorow*/
            int s = count_separator(getenv("PATH"), ':');
            char *paths[s];/*sciezki do komend*/
            for(i=0;i<=s;i++)
            {
                paths[i]=(char*)malloc(sizeof(char)*(SIZE+1));
                memset(paths[i],'\0',SIZE+1);
            }
            split(getenv("PATH"),paths, ':');
            /*for(i=0;i<=s;i++)
                printf("%s\n",paths[i]);*/
            
            p[argc]=NULL;
            exec_command(p,paths,s+1);
            for(i=0;i<=s;i++)
            {
                free(paths[i]);
            }
            
        }
        
        /*Zwolnienie zaalokowanej pamieci*/
        free(cmd);
        free(command);
        for (i=0;i<=argc;i++) {
            free(p[i]);
        }
    }
    
    return 0;
}

void read_command(char command[])
{
    /*czytanie komendy tak dlugo az nie pojawi sie znak nowej linii*/
    int i=0;
    do {
        scanf("%c", &command[i]);
        i++;
    } while((command[i-1]!='\n') && (i<SIZE));
    command[i-1]='\0';
}

void get_command(char cmd[],char command[])
{
    /*pobranie pierwszego argumentu(komendy np. cd, cp itd)*/
    int j=0;
    int i;
    bool flag=false;/*flaga do usuwania spacji*/
    for (i=0;i<strlen(command);i++) {
        if(command[i]!=' ') {
            cmd[j]=command[i];
            j++;
            flag=true;
        } else {
            if (flag) {
                break;
            }
        }
    }
    cmd[j]='\0';
}

int amount_of_args(char command[])
{
    /*funkcja liczy liczbe argumentow z komenda wlacznie*/
    int x=0;
    int i;
    /*x-liczba argumentow z komenda*/
    if(command[0]!=' ') {
        x++;
    }
    for(i=1;i<strlen(command);i++) {
        if((command[i-1]==' ')&&(command[i]!=' ')){
            x++;
        }
    }
    
    return x;
}

void change_directory(int argc,char path[])
{
    /*Obsluga bledow*/
    if(argc>2) {
       printf("Too many arguments!\n");
    } else {
        /*zmiana biezacego katalogu*/
        int err;
        if((argc==1) || !strcmp("~",path))
            err=chdir(getenv("HOME"));/*zmiana na katalog domowy*/
        else
            err=chdir(path);/*zmiana na katalog okreslony zmiena path*/
        
        if(err==-1) 
            printf("No such file or directory!\n");

    }
}

void fill_two_dimensial_array_of_args(int argc, char command[], char **arguments)
{
    /*Utworzenie dwuwymiarowej tablicy do exec'a i do zaimplementownych komend*/
    int x;
    for(x=0;x<argc;x++) {
        int j=0;
        int i;
        bool flag=false;/*pomocnicza flaga do rozdzielania 'slow' w zmiennej command*/
        for(i=0;i<strlen(command);i++) {
            if (command[i]!=' ') {
                arguments[x][j]=command[i];
                j++;
                flag=true;
            } else {
                if(flag) {
                    flag=false;
                    arguments[x][j]='\0';
                    x++;
                    j=0;
                    if (argc<=x) break;
                }
            }

        }
    }
}

void exec_script(char **path)
{
    /*wykonanie skryptu*/    
    int pid = fork();
	if (pid == -1)
        printf("fork error!\n");
	else if (pid==0) {
        int error=execv(path[0],path);
        if (error==-1) 
            printf("bash %s: No such file or directory!\n",path[0]);
        exit(0);
	} else {
	    wait(NULL);
    }
}

void exec_command(char **path, char **PATHS, int length)
{
    /*wykonanie komend za pomoca fork i exec*/
    int i,j;
    bool flag=false;
    DIR *dir;
    struct dirent *directory;
    
    /*sprawdzanie czy komenda znajduje sie w jakims katalogow
     wskazanym przez zmienna srodowiskowa PATH*/
    for(i=0;((i<length) &&(!flag));i++)
    {
        dir=opendir(PATHS[i]);
        if(dir!=NULL) {
            while((directory=readdir(dir))!=NULL) {
                if(!strcmp(directory->d_name,path[0])) {
                 flag=true;  
                 j=i;
                 /*printf("File: %s\n",PATHS[i]);*/
                }
            }
            closedir(dir);
        }
        else {
            printf("error: %s\n", PATHS[i]);
        }
    }
    
    /*Komenda nie znaleziona*/
    if(!flag) {
        printf("[%s]: command not found!\n", path[0]);
    }
    /*Uruchomienie komendy jesli znaleziono sciezke do tej komendy*/
    else {
        strcat(PATHS[j],"/");
        strcat(PATHS[j],path[0]);
        
        int pid =fork();
        if (pid==-1)printf("fork error!\n");
        else if (pid==0)
        {
        
        int err = execv(PATHS[j],path);
        if(err==-1) printf("exec error!\n");
            exit(0);
        }
        else {
            wait(NULL);
        }
    }
}

void copy(int argc,char **arguments)
{
    if(argc==2) {
        /*Kopiowanie plikow*/
        int fd1,fd2;
        
        fd1=open(arguments[1],O_RDONLY);
        
        if (fd1==-1) {
            printf("cp: %s: No such file or directory!\n",arguments[1]);   
        } else {
            fd2=open(arguments[2],O_WRONLY|O_CREAT,0644);
            
            if(fd2==-1) 
                printf("Cannot write data to %s\n",arguments[2]);   
            else
            {
                char buf[SIZE];
                int n;/*Liczba odczytanych znakow*/
                
                while((n=read(fd1,&buf,SIZE))>0)/*odczyt danych z pierwszego pliku*/
                {
                    write(fd2,&buf,n);/*zapis danych z pierwszego pliku do drugiego*/
                }
            }
            
        }     
        
        close(fd1);
        close(fd2);
    }
    /*Obsluga bledu*/
    else if(argc==1){
        printf("cp: missing destination file operand after '%s'\n",arguments[1]);   
    } else if(argc==0) {
        printf("cp: missing file operand\n");   
    } else {
        printf("Too many arguments!\n");   
    }
}

void cat(int argc,char **arguments)
{
    if(argc==0) {   
        /*Brak argumentow*/
        printf("cat: missing file operand\n");   
    }
    else {
        int fd;
        int i;
        
        /*Odczyt danych z jednego lub wiecej plikow*/
        for(i=1;i<=argc;i++)
        {
            
            fd=open(arguments[i],O_RDONLY);
            
            if(fd==-1)
            {
                printf("cat: %s: No such file or directory!\n",arguments[i]);   
            }
            else
            {
                char buf[SIZE];
                while(read(fd,&buf,SIZE)>0)
                {
                    printf("%s",buf);
                    memset(buf,'\0',SIZE);
                }
                close(fd);
            }     
        }
    }

}

int count_separator(char *string,char separator)
{
    /*liczenie separatorow*/
    int s=0;
    int i;
    
    for(i=0;i<strlen(string);i++) {
        if(string[i]==separator)s++;
    }
    return s;
}

void split(char *string,char** array2d, char separator)
{

    /*utworzenie tablicy dwuwymiarowej z sciezkami */
    int i;
    int j=0;
    int k=0;
    for(i=0;i<strlen(string);i++) {
        if(string[i]==separator) {
            j++;            
            k=0;
        }
        else
            array2d[j][k++]=string[i];
    }
}
