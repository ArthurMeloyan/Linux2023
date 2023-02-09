#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if (argc == 1){
        printf("Invalid count of arguments is provided\n");
        return 1;
    }
    
    int source = open(argv[1], O_RDONLY);
    if (source == -1){
        perror("Error occurred while trying to open the file");
        return 1;
    }

    int target;
    if (argc == 2){
        target = open("target", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }

    if (argc == 3){
        std::string answ;
        std::cout << "Do you want to override it? ";
        std::cin >> answ;
        if (answ == "yes"){
            target = open(argv[2], O_WRONLY | O_TRUNC);
        }

        if (answ == "no"){
            target = open(argv[2], O_WRONLY | O_APPEND);
        }
    }

    if (target == -1){
        perror("Error occurred while trying to open the file");
        return 1;
    }
    ssize_t bytes;
    char buf[1024];
    while((bytes = read(source, buf, 1024)) > 0){
        write(target, buf, bytes);
    }
    close(source);
    close(target);
    return 0;
}