#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<fcntl.h> 
#include <string.h>

int main() {

    struct stat st;
    char fileName[100];
    char destination[100];
    printf("Enter The File Source: ");
    scanf("%s", fileName);
    printf("You entered %s.\n", fileName);
    printf("Enter The Destionation Name: ");
    scanf("%s", destination);
    printf("You entered %s.", destination);
    // char * fl[] = fileName;
    stat(fileName, &st);
    int size = st.st_size;

    int fd, sz;
    char *c = (char *) calloc(size, sizeof(char)); 
  
    fd = open(fileName, O_RDONLY); 

    if (fd < 0) { perror("r1"); exit(1); } 
    sz = read(fd, c, size); 
    c[sz] = '\0'; 
    printf("File Length: %d\n", (int)strlen(c));
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
        // printf("Temp: %s\n", temp);
        if(c[i] == c[i + 1] && (c[i] == 49 || c[i] == 48)){
            if(c[i] == 49){
                type = 1;
                char a[2] = {(char)c[i], '\0'};
                if(firstInsertionTemp){
                    strcpy(temp, a);
                    firstInsertionTemp = 0;
                }else{
                    strcat(temp, a);
                }
            }else {
                char a[2] = {(char)c[i], '\0'};
                // printf("A HERE : %s",)
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
                char d[2] = {(char)c[i], '\0'};
                if(firstInsertionTemp){
                    strcpy(temp, d);
                    firstInsertionTemp = 0;
                }else{
                    strcat(temp, d);
                };

                if(firstInsertion){
                    strcpy(returnString, temp);
                    firstInsertion = 0;
                } else {
                    strcat(returnString, temp);
                };
                firstInsertionTemp = 1;
                
                memset(temp,0,sizeof(temp));
            }else{
                
                char st[2] = {(char)c[i], '\0'};
                if(firstInsertion){
                    strcpy(returnString, st);
                    firstInsertion = 0;
                }else{
                    strcat(returnString, st);
                }
                memset(temp,0,sizeof(temp));                
            }
            count = 0;
        }
    }
    // printf("Return String: \n%s\n", returnString);
    // int fd;


    close(fd);
    fd = open(destination, O_CREAT | O_WRONLY, 0600);

    if(fd == -1){
        printf("Failed");
    }

    write(fd, returnString, strlen(returnString));

    close(fd);

    return 0;
}