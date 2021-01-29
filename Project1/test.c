#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<fcntl.h> 
#include <string.h>

int main() {

    struct stat st;
    stat("source.txt", &st);
    int size = st.st_size;

    int fd, sz; 
    char *c = (char *) calloc(size, sizeof(char)); 
  
    fd = open("source.txt", O_RDONLY); 

    if (fd < 0) { perror("r1"); exit(1); } 
    sz = read(fd, c, size); 
    c[sz] = '\0'; 
    printf("Length: %d\n", (int)strlen(c));
    //   char s = (char) c[3];
    char s = (char) 49;
    int count = 0;
    char temp[size];
    char returnString[size];
    int type;
    int firstInsertion = 1;
    int firstInsertionTemp = 1;
    // char type
    for(int i = 0; i < (int)strlen(c); i++){
        printf("C: %c\n", c[i]);
        if(i != (int)strlen(c) - 1 && c[i] == c[i + 1] && (c[i] == 49 || c[i] == 48)){
            if(c[i] == 49){
                type = 1;
                char a[] = {c[i]};
                if(firstInsertionTemp){
                    strcpy(temp, a);
                    firstInsertionTemp = 0;
                }else{
                    strcat(temp, a);
                }
                
                // printf("Temp: %s\n", temp);
            //else meaing it is 48
            }else {
                char a[] = {c[i]};
                if(firstInsertionTemp){
                    strcpy(temp, a);
                    firstInsertionTemp = 0;
                }else{
                    strcat(temp, a);
                }
                type = 0;
            }
            count++;
        }else{
            if(count >= 15){
                memset(temp,0,sizeof(temp));
                count += 1;
                char str[100];
                char buffer[100];
                memset(buffer,0,sizeof(buffer));
                memset(str,0,sizeof(str));
                if(type == 1){
                    str[0] = '+';

                    sprintf(buffer, "%d", count); 
                    strcat(str, buffer);
                    str[strlen(buffer) + 1] = '+'; 
                }
                if(type == 0){
                    str[0] = '-';

                    sprintf(buffer, "%d", count); 
                    strcat(str, buffer);
                    str[strlen(buffer) + 1] = '-';
                }

                if(firstInsertion){
                    strcpy(returnString, str);
                    firstInsertion = 0;
                } else {strcat(returnString, str);}
                memset(temp,0,sizeof(temp));
                

            }else if(c[i] == 49 || c[i] == 48){
                char a[] = {c[i]};
                if(firstInsertionTemp){
                    strcpy(temp, a);
                    firstInsertionTemp = 0;
                }else{
                    strcat(temp, a);
                }
                printf("Temp: %s\n", temp);

                if(firstInsertion){
                    strcpy(returnString, temp);
                    firstInsertion = 0;
                } else {
                    strcat(returnString, temp);
                }
                firstInsertionTemp = 1;
                
                //whipe temp clean
                memset(temp,0,sizeof(temp));
            }else{
                // printf("h%ch", c[i]);
                
                char st[1];
                memset(st,0,sizeof(st));
                st[0] = c[i];
                if(firstInsertion){
                    strcpy(returnString, st);
                    firstInsertion = 0;
                }else{
                    strcat(returnString, st);
                }
                memset(temp,0,sizeof(temp));
                // printf("Temp: %s\n", temp);


                
            }
            count = 0;
        }
    }
    printf("Return String:%s", returnString);

    close(fd);

    return 0;
}