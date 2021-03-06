#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <sstream>

#define PORT 8080

//file system sturcture
struct FileSystem
{
    int fileID[2]; // file id contains 2 parts, the level of the file and the current position of the file
    std::string fileName; // name of the file
    int parentID[2]; // parent's file id
    std::string content; // if a file is a txt file, it will have content
    int isTxt;
};

//socket management, this will pass the socket and the related command to the thread function
struct sockPass
{
    int socket;
    int flag; // for function [L]isting
    std::string file; // file name
    std::string input; // write file purpose
};


std::ifstream in_fs; // represent the file system file/ to represent if the file system exist or not
FileSystem *current; // current directory
FileSystem *rootFile; // root directory
std::vector<FileSystem *> pwd; //store the current file directory path
std::vector<std::vector<FileSystem *>> file_level; // the overall file system storage management vector, to store the files level by level
std::string message; // a message string that use to send the message to the client
int dataStored = 0; // to check if the file system has been imported to the server program or not

// fileID[0] = file level number; [1] = file number at current level
// three different new file function, 1 for root, 1 for directory, 1 for txt file
FileSystem *newFile(std::string fname, int id[2])
{
    FileSystem *fs = new FileSystem;
    fs->fileName = fname;
    fs->fileID[0] = id[0];
    fs->fileID[1] = id[1];
    fs->isTxt = 0;

    return fs;
}

FileSystem *newFile(std::string fname, int id[2], int parentid[2])
{
    FileSystem *fs = new FileSystem;
    fs->fileName = fname;
    fs->fileID[0] = id[0];
    fs->fileID[1] = id[1];
    fs->parentID[0] = parentid[0];
    fs->parentID[1] = parentid[1];
    fs->isTxt = 0;
    return fs;
}

FileSystem *newFile(std::string fname, int id[2], int parentid[2], std::string content)
{
    FileSystem *fs = new FileSystem;
    fs->fileName = fname;
    fs->fileID[0] = id[0];
    fs->fileID[1] = id[1];
    fs->parentID[0] = parentid[0];
    fs->parentID[1] = parentid[1];
    fs->content = content;
    fs->isTxt = 1;
    return fs;
}

// update the file system file when ever the server program has an update for [file_level]
void update_file_system()
{
    //open a the current file system file and overwrite it
    std::ofstream update("filesystem.txt");

    //replace all the old data with the new data
    for (int j = 0; j < file_level.size(); j++)
    {
        if (j == 0)
        {
            update << "root";
        }
        else
        {
            for (int k = 0; k < file_level[j].size(); k++)
            {
                std::string construct;
                FileSystem *currentFile = file_level[j][k];
                // special construction in the file system file to store and read the datas
                construct = std::to_string(currentFile->parentID[0]) + '&' +
                            std::to_string(currentFile->parentID[1]) + '|' +
                            (currentFile->fileName);
                // if the current file is a txt file, include its content
                if ((currentFile->isTxt) == 1)
                {
                    construct += '~' + (currentFile->content);
                }
                update << construct;
                if (k != (file_level[j].size() - 1))
                {
                    update << " ";
                }
            }
        }
        if (j != (file_level.size() - 1))
        {
            update << "\n";
        }
    }
}

// remove all related file to a directory one level by one level
void remove_related_file(FileSystem* fs, int level) {
    if (level == (file_level.size() - 1)) {
        //base case
        std::cout << "Reaches to the end level\n";
    }
    else {
        //remove all the related file
        int nextLevel = (fs->fileID[0]) + 1;
        for (int i = 0; i < file_level[nextLevel].size(); i++) {
            if ( (file_level[nextLevel][i]->parentID[0] == fs->fileID[0]) &&
                (file_level[nextLevel][i]->parentID[1] == fs->fileID[1]) ) {

                    remove_related_file(file_level[nextLevel][i], nextLevel);
            }
        }
        int count = file_level[nextLevel].size();
        //extract the current file number, use it to shift all the remaining file's parent id later
        int fileNum = fs->fileID[1];

        for (int i = 0; i < count; i ++) {
            //if it matches the parent id then delete
            if ( (file_level[nextLevel][i]->parentID[0] == fs->fileID[0]) &&
                (file_level[nextLevel][i]->parentID[1] == fs->fileID[1]) ) {
                    //shift the next file number left 1 then remove the current file
                    for (int j = i + 1; j < file_level[nextLevel].size(); j++) {
                            int current_id = (file_level[nextLevel][j]->fileID[1] - 1);
                            file_level[nextLevel][j]->fileID[1] = current_id;
                    }

                    file_level[nextLevel].erase(file_level[nextLevel].begin() + i);
                    i --;
                    count = file_level[nextLevel].size();

            }
        }
        // for all the remaining file, if their parent's file number is > the current file's file number, shift
        // their parent's file number one to the left
        for (int i = 0; i < file_level[nextLevel].size(); i ++) {
            if ((file_level[nextLevel][i]->parentID[1] > 0) && (file_level[nextLevel][i]->parentID[1] > fileNum)) {
                int current_parentID2 = (file_level[nextLevel][i]->parentID[1] - 1);
                file_level[nextLevel][i]->parentID[1] = current_parentID2;
            }
        }
        //if the next level is empty after remove all files, erase the level as well
        if (file_level[nextLevel].size() == 0) {
            file_level.erase(file_level.begin() + nextLevel);
        }

        std::cout << "finish one level\n";
    }
}

// delete the current file system if there is one
void *delete_file_system(void *request){
    int sock = ((struct sockPass*) request)->socket;

    if (in_fs.is_open()) {
        in_fs.close();
        remove("filesystem.txt");
        file_level.clear();
        pwd.clear();
        rootFile = NULL;
        current = NULL;
        dataStored = 0;
        message = "Remove sucessfully";
        std::cout << "file system removed\n";
        send(sock, message.c_str(), message.length(), 0);
    }
    else {
        message = "No file system to remove";
        std::cout << "Fail to remove because there is no file system\n";
        send(sock, message.c_str(), message.length(), 0);
    }

    pthread_exit(0);
}

// read the file system to initialize the server data
void *read_file_system(void *request)
{
    std::string reader;

    std::vector<std::string> fileLocation;

    int count = 0;
    while (getline(in_fs, reader))
    {
        //first line is the root
        if (count == 0)
        {
            std::vector<FileSystem *> fsv;
            int id[2] = {0, 0};
            FileSystem *fs = newFile(reader, id);
            fsv.push_back(fs);
            file_level.push_back(fsv);
            count++;
        }
        // after push the root file, push one level at a time then do specification later
        else
        {  
            fileLocation.push_back(reader);
        }
    }
    // for all the level inside file location, put the level into multiple files
    for (int i = 0; i < fileLocation.size(); i++)
    {

        std::string level = fileLocation[i];
        std::stringstream s(level);
        std::string temp;
        std::vector<std::string> content;
        while (getline(s, temp, ' '))
        {
            content.push_back(temp);
        }

        // create a current level list to push all the file in the current level after everything is done
        std::vector<FileSystem *> currentLevel;
        for (int j = 0; j < content.size(); j++)
        {
            // for every file in this level, read the file by decode their structure
            std::string sub_level = content[j];
            std::stringstream s2(sub_level);
            std::vector<std::string> content2;
            while (getline(s2, temp, '|'))
            {

                content2.push_back(temp);
            }
            std::stringstream sc(content2[0]);
            short count_for_place = 0;
            int pa_id[2];
            while (getline(sc, temp, '&'))
            {

                pa_id[count_for_place] = std::stoi(temp);
                count_for_place++;
            }
            int id[2];
            id[0] = file_level.size();
            id[1] = currentLevel.size();
            //check if the last 4 digit of a file name is .txt extension
            if (content2[1].length() > 4)
            {
                std::stringstream extensionCheck(content2[1]);
                std::vector<std::string> fileNameCheck;
                while (getline(extensionCheck, temp, '~'))
                {
                    fileNameCheck.push_back(temp);
                }
                int checkposition = fileNameCheck[0].length() - 4;
                std::string extension;
                for (int i = checkposition; i < fileNameCheck[0].length(); i++)
                {
                    extension += fileNameCheck[0][i];
                }
                if (extension.compare(".txt") == 0)
                {
                    currentLevel.push_back(newFile(fileNameCheck[0], id, pa_id, fileNameCheck[1]));
                }
                else
                {
                    currentLevel.push_back(newFile(content2[1], id, pa_id));
                }
            }
            else
            {
                currentLevel.push_back(newFile(content2[1], id, pa_id));
            }
        }
        file_level.push_back(currentLevel);
    }

    current = file_level[0][0];
    rootFile = file_level[0][0];
    pwd.push_back(rootFile);
    pthread_exit(0);
}

// create a file system if no file system exist
void *create_file_system(void *request)
{
    int sock = ((struct sockPass *)request)->socket;
    if (in_fs.is_open())
    {
        message = "There is already a file system exist";
        std::cout << "Fail to create file system because there already exist one\n";
    }
    else
    {
        std::ofstream ofile("filesystem.txt");
        ofile << "root";
        ofile.close();
        message = "File system created successfully";
        std::cout << "Created a file system\n";
    }

    send(sock, message.c_str(), message.length(), 0);
    pthread_exit(0);
}

// listing all the file under the current file
void *listing(void *request)
{
    int sock = ((struct sockPass *)request)->socket;
    int flag = ((struct sockPass *)request)->flag;
    if (in_fs.is_open())
    {
        std::cout << current->fileName << "\n";
        int nextLevel = (current->fileID[0]) + 1;

        //if there does not exist a next level, then return empty string
        if (nextLevel < file_level.size())
        {

            int nextLevelSize = file_level[nextLevel].size();
            std::vector<FileSystem *> currentLevel = file_level[nextLevel];
            for (int i = 0; i < nextLevelSize; i++)
            {

                // see if the parent id in the next level matches the file ID of the current directory
                if ((currentLevel[i]->parentID[0] == current->fileID[0]) &&
                    (currentLevel[i]->parentID[1] == current->fileID[1]))
                {
                    if (flag == 1)
                    {
                        message += std::to_string(file_level[nextLevel][i]->parentID[0]);
                        message += '&' + std::to_string(file_level[nextLevel][i]->parentID[1]) + '|';
                    }
                    message += file_level[nextLevel][i]->fileName;
                    if (flag == 1) {
                        message += " " + std::to_string(file_level[nextLevel][i]->fileID[0]) + " " + std::to_string(file_level[nextLevel][i]->fileID[1]);
                    }
                    if (i != (nextLevelSize - 1))
                    {
                        message += "\n";
                    }
                }
            }
        }
        else {
            message = " ";
        }

        std::cout << "Returned current file list contents\n";
        
    }
    else
    {
        message = "No file system, fail to list";
        std::cout << "Fail to list all the file in the directory because there is no file system\n";
    }

    send(sock, message.c_str(), message.length(), 0);
    pthread_exit(0);
}

// read a .txt file
void *read_file(void *request)
{

    struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string filename = input->file;
    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;
    if (nextLevel < file_level.size())
    {
        if (filename.length() <= 4)
        {
            message = "2 file name does not match the requirement";
            std::cout << "file name has a length <= 4 which cannot hold .txt as extension\n";
        }
        else
        {
            message = "1";
            std::vector<FileSystem *> child = file_level[nextLevel];
            std::vector<FileSystem *> child_of_current_directory;
            for (int i = 0; i < child.size(); i++)
            {
                if ((child[i]->parentID[0] == current->fileID[0]) &&
                    (child[i]->parentID[1] == current->fileID[1]))
                {
                    child_of_current_directory.push_back(child[i]);
                }
            }
            for (int i = 0; i < child_of_current_directory.size(); i++)
            {
                if (((child_of_current_directory[i]->fileName).compare(filename) == 0) &&
                    child_of_current_directory[i]->isTxt == 1)
                {
                    message = "0 " + (child_of_current_directory[i]->content);
                    std::cout << "find the file, the file will be read and pass back data to the client\n";
                    send(sock, message.c_str(), message.length(), 0);

                    pthread_exit(0);
                }
            }
            std::cout << "file not find, return 1 to the cllient; ignore if the file was found"
                      << "\n";
        }
    }
    else
    {
        message = "2 there is no file";
        std::cout << "the request level does not exist\n";
    }

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

// write to a .txt file
void *write_file(void *request)
{
    struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string sentense = input->input;
    std::string filename = input->file;

    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;
    if (nextLevel < file_level.size())
    {

        if (filename.length() <= 4)
        {
            message = "2 file name does not match the requirement";
            std::cout << "file name has a length <= 4 which cannot hold .txt as extension\n";
        }
        else
        {
            message = "1";
            std::vector<FileSystem *> child = file_level[nextLevel];
            std::vector<FileSystem *> child_of_current_directory;
            for (int i = 0; i < child.size(); i++)
            {
                if ((child[i]->parentID[0] == current->fileID[0]) &&
                    (child[i]->parentID[1] == current->fileID[1]))
                {
                    child_of_current_directory.push_back(child[i]);
                }
            }
 
            for (int i = 0; i < child_of_current_directory.size(); i++)
            {
 
                if ((child_of_current_directory[i]->fileName).compare(filename) == 0)
                {

                    message = "0";

                    int fid1 = (child_of_current_directory[i]->fileID[0]);
                    int fid2 = (child_of_current_directory[i]->fileID[1]);

                    file_level[fid1][fid2]->content = sentense;

                    update_file_system();
  
                    std::cout << "find the file, the file\'s content has been updated\n";
                    send(sock, message.c_str(), message.length(), 0);

                    pthread_exit(0);
                }
            }
            std::cout << "file not find, return 1 to the cllient"
                      << "\n";
        }
    }
    else
    {
        message = "2 there is no file";
        std::cout << "the request level does not exist\n";
    }

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

// create a file with .txt extension
void *create_file(void *request)
{
    struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string filename = input->file;
    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;

    //if a file name is <= 4 which means it cannot hold the .txt extension
    if (filename.length() <= 4)
    {
        message = "2 file name does not match the requirement";
        std::cout << "file name has a length <= 4 which cannot hold .txt as extension\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    //else, check if the file contain .txt extention
    else
    {
        int checkposition = filename.length() - 4;
        std::string extension;
        for (int i = checkposition; i < filename.length(); i++)
        {
            extension += filename[i];
        }
        if (extension.compare(".txt") != 0)
        {
            message = "2 file name does not match the requirement";
            std::cout << "the file did not declare .txt as extension\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
    }

    // if this file is the first file/directory in the next level, push it directly
    // or aka, if the next level has not initilize its size yet
    if (nextLevel >= file_level.size())
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        std::vector<FileSystem *> newLevel;
        id[0] = file_level.size();
        id[1] = 0;
        FileSystem *nfile = newFile(filename, id, pa_id, "_");
        newLevel.push_back(nfile);
        file_level.push_back(newLevel);
       
        message = "0";
        std::cout << "file created successfully\n";
    }
    else
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        for (int i = 0; i < file_level[nextLevel].size(); i++)
        {
            if ((file_level[nextLevel][i]->fileName).compare(filename) == 0)
            {
                if ( (file_level[nextLevel][i]->parentID[0] == current->fileID[0]) &&
                    (file_level[nextLevel][i]->parentID[1] == current->fileID[1]) )
                {
                    message = "1 file already exist";
                    std::cout << "fail to create the file, file name is already existed\n";
                    send(sock, message.c_str(), message.length(), 0);
                    pthread_exit(0);
                }
            }
        }
        id[0] = (current->fileID[0] + 1);
        id[1] = file_level[id[0]].size();
        FileSystem *nfile = newFile(filename, id, pa_id, "_");
        file_level[id[0]].push_back(nfile);
        message = "0";
        std::cout << "file created successfully\n";
    }

    update_file_system();

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

// remove a current exist .txt extension file
void *delete_file(void *request) {
    struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string filename = input->file;
    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;

    //if a file name is <= 4 which means it cannot hold the .txt extension
    if (filename.length() <= 4)
    {
        message = "2 file name does not match the requirement";
        std::cout << "file name has a length <= 4 which cannot hold .txt as extension\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    //else, check if the file contain .txt extention
    else
    {
        int checkposition = filename.length() - 4;
        std::string extension;
        for (int i = checkposition; i < filename.length(); i++)
        {
            extension += filename[i];
        }
        if (extension.compare(".txt") != 0)
        {
            message = "2 file name does not match the requirement";
            std::cout << "the file did not declare .txt as extension\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
    }

    // if there is no next level for the current level, no file will be found
    if (nextLevel >= file_level.size())
    {
        message = "1 file does not exist";
        std::cout << "file does not exist\n";
    }
    else
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        for (int i = 0; i < file_level[nextLevel].size(); i++)
        {
            if ((file_level[nextLevel][i]->fileName).compare(filename) == 0)
            {
                if ( (file_level[nextLevel][i]->parentID[0] == current->fileID[0]) &&
                    (file_level[nextLevel][i]->parentID[1] == current->fileID[1]) ){
                    // shift all id to left 1
                    for (int j = i + 1; j < file_level[nextLevel].size(); j ++) {
                        // if the current file position/number is > 0
                            int current_id = (file_level[nextLevel][j]->fileID[1]);
                            if (current_id > 0) {
                                file_level[nextLevel][j]->fileID[1] = current_id - 1; 
                            }
                    }
                    file_level[nextLevel].erase(file_level[nextLevel].begin() + i);
                    update_file_system();
                    message = "0";
                    std::cout << "file deleted\n";
                    send(sock, message.c_str(), message.length(), 0);
                    pthread_exit(0);
                }
            }
        }
        message = "1";
        std::cout << "file does not exist\n";
    }

  

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

//make directory has a similar logic as create_file, but it check to see the file name as not in a .txt format
void *make_directory(void *request) {
struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string filename = input->file;
    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;

    //if a file name is > 4, check to see if the file declare as a .txt extension
    if (filename.length() > 4)
    {
        int checkposition = filename.length() - 4;
        std::string extension;
        for (int i = checkposition; i < filename.length(); i++)
        {
            extension += filename[i];
        }
        if (extension.compare(".txt") == 0)
        {
            message = "2";
            std::cout << "cannot make directory, because the file declare .txt as its extension\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
    }

    // if this file is the first file/directory in the next level, push it directly
    // or aka, if the next level has not initilize its size yet
    if (nextLevel >= file_level.size())
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        std::vector<FileSystem *> newLevel;
        id[0] = file_level.size();
        id[1] = 0;
        FileSystem *nfile = newFile(filename, id, pa_id);
        newLevel.push_back(nfile);
        file_level.push_back(newLevel);
       
        message = "0";
        std::cout << "directory created successfully\n";
    }
    else
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        //check if there is duplicated directory being created
        for (int i = 0; i < file_level[nextLevel].size(); i++)
        {   // first check its directory name
            if ((file_level[nextLevel][i]->fileName).compare(filename) == 0)
            {
                // if true, see if they belong to the same parent by tracking their parent's id and the current directroy's id
                if ( (file_level[nextLevel][i]->parentID[0] == current->fileID[0]) &&
                    (file_level[nextLevel][i]->parentID[1] == current->fileID[1]) )
                {
                    message = "1";
                    std::cout << "fail to create the directory, directory name is already existed\n";
                    send(sock, message.c_str(), message.length(), 0);
                    pthread_exit(0);
                }
            }
        }
        // if passed the check, create the directory
        id[0] = current->fileID[0] + 1;
        id[1] = file_level[id[0]].size();
        FileSystem *nfile = newFile(filename, id, pa_id);
        file_level[id[0]].push_back(nfile);
        message = "0";
        std::cout << "directory created successfully\n";
    }

    update_file_system();

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

// alike make_directory
void *change_directory(void *request) {
    struct sockPass* infor = (struct sockPass*) request;
    int sock = infor->socket;
    std::string filename = infor->file;

    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    //spacial cases
    if (filename.compare("..") == 0) {
        if (pwd.size() == 1) {
            message = "1";
            std::cout << "already in the root\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
        message = "0";
        pwd.pop_back();
        current = pwd[pwd.size() - 1];
        std::cout << "directory changed\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    if (filename.compare("...") == 0) {
        message = "0";
        pwd.clear();
        current = rootFile;
        pwd.push_back(current);
        std::cout << "directory changed\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }

    int nextLevel = (current->fileID[0]) + 1;

    //if a file name is > 4, check to see if the file declare as a .txt extension
    if (filename.length() > 4)
    {
        int checkposition = filename.length() - 4;
        std::string extension;
        for (int i = checkposition; i < filename.length(); i++)
        {
            extension += filename[i];
        }
        if (extension.compare(".txt") == 0)
        {
            message = "2 file name does not match the requirement";
            std::cout << "cannot change directory, because the file declare .txt as its extension\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
    }

    // if this file is the first file/directory in the next level, fail to change directory
    // or aka, if the next level has not initilize its size yet
    if (nextLevel >= file_level.size())
    {
        message = "1 no such directory";
        std::cout << "no direcotry exist, fail to change directory\n";
    }
    else
    {
        //check if there is exist directory for change directory
        for (int i = 0; i < file_level[nextLevel].size(); i++)
        {   // first check its directory name
            if ((file_level[nextLevel][i]->fileName).compare(filename) == 0)
            {
                // if true, see if they belong to the same parent by tracking their parent's id and the current directroy's id
                if ( (file_level[nextLevel][i]->parentID[0] == current->fileID[0]) &&
                    (file_level[nextLevel][i]->parentID[1] == current->fileID[1]) )
                {
                    message = "0";
                    current = NULL;
                    current = file_level[nextLevel][i];
                    pwd.push_back(current);
                    std::cout << "directory changed\n";
                    send(sock, message.c_str(), message.length(), 0);
                    pthread_exit(0);
                }
            }
        }
        // if passed the check, fail to change directory
        message = "1 no such directory";
        std::cout << "directory not exist\n";
    }
    
    send(sock, message.c_str(), message.length(), 0);
    pthread_exit(0);

}

// print workign directory
void *print_working_directory(void* request) {
    struct sockPass* infor = (struct sockPass*) request;
    int sock = infor->socket;

    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }

    std::string output;
    // print everything in the pwd list
    for (int i = 0; i < pwd.size(); i ++) {
        std::string temp = (pwd[i]->fileName);
        output += temp;
        if (i != (pwd.size() - 1)) {
            output += "/";
        }
    }

    std::cout << "printed work directory\n";
    send(sock, output.c_str(), output.length(), 0);

    pthread_exit(0);

}

// remove directory, same as make directory, has a similar logic as delete_file
void *remove_directory(void *request) {
    struct sockPass *input = (struct sockPass *)request;
    int sock = input->socket;
    std::string filename = input->file;
    if (!(in_fs.is_open())) {
        message = "Error, there is no file system";
        std::cout << "No file system in use\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    if (file_level.size() == 1) {
        message = "ERROR, cannot remove the root file, if you want to remove the whole file system, try [RESET]";
        std::cout << "cannot remove the root file\n";
        send(sock, message.c_str(), message.length(), 0);
        pthread_exit(0);
    }
    int nextLevel = (current->fileID[0]) + 1;

    //if a file name is > 4 which means check if the file name contain .txt extention
    if (filename.length() > 4)
    {
        int checkposition = filename.length() - 4;
        std::string extension;
        for (int i = checkposition; i < filename.length(); i++)
        {
            extension += filename[i];
        }
        if (extension.compare(".txt") == 0)
        {
            message = "2 file name does not match the requirement";
            std::cout << "the file name declare itself .txt as its extension\n";
            send(sock, message.c_str(), message.length(), 0);
            pthread_exit(0);
        }
    }

    // if there is no next level for the current level, no file will be found
    // because it didn't declare a new level
    if (nextLevel >= file_level.size())
    {
        message = "1 no such directory";
        std::cout << "directory does not exist\n";
    }
    else
    {
        int id[2];
        int pa_id[2] = {(current->fileID[0]), (current->fileID[1])};
        for (int i = 0; i < file_level[nextLevel].size(); i++)
        {
            // after remove the file, the file id will be shift one to the left
            if ((file_level[nextLevel][i]->fileName).compare(filename) == 0)
            {
                if ( (file_level[nextLevel][i]->parentID[0] == current->fileID[0]) &&
                    (file_level[nextLevel][i]->parentID[1] == current->fileID[1]) )
                {
                    remove_related_file(file_level[nextLevel][i], nextLevel);
                     // shift all the existing file number after it 1 if their file number is > 0
                     // starting from the current file who is ready to deleted, shift before delete
                    for (int j = i + 1; j < file_level[nextLevel].size(); j ++) {
                        // if the current file position/number is > 0
                            int current_id = (file_level[nextLevel][j]->fileID[1]);
                            if (current_id > 0) {
                                file_level[nextLevel][j]->fileID[1] = current_id - 1; 
                            }
                    }
                    file_level[nextLevel].erase(file_level[nextLevel].begin() + i);



                    update_file_system();
                    message = "0";
                    std::cout << "directory deleted\n";
                    send(sock, message.c_str(), message.length(), 0);
                    pthread_exit(0);
                }
            }
        }
        message = "1 no such directory";
        std::cout << "directory does not exist\n";
    }

  

    send(sock, message.c_str(), message.length(), 0);

    pthread_exit(0);
}

int main(int argc, char *argv[])
{

    int server_fd, new_socket, valread;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server connected to port " << PORT << "\n";

    while (true)
    {

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t pt;

        //if the server just start up and no information has been store in the program/server
        if (dataStored == 0)
        {
            //try to open the file system
            in_fs.open("filesystem.txt");
            // if the disk is existed, read the number of tracks and sectors and datas
            if (in_fs.is_open())
            {
                pthread_create(&pt, NULL, &read_file_system, NULL);
                pthread_join(pt, NULL);
                dataStored = 1;
            }
        }

        char buffer[1024] = {0};

        read(new_socket, buffer, 1024);

        std::vector<std::string> command_list;

        std::string conv = buffer;

        std::stringstream ex(conv);

        std::string temp;

        struct sockPass *arg = new sockPass;

        // push the command into a list
        while (getline(ex, temp, ' '))
        {
            command_list.push_back(temp);
        }
        // after getting the command, compare the command with all the avaliable command
        if (command_list[0].compare("exit") == 0)
        {
            std::cout << "Client exited\n------------------------\n";
        }
        else if (command_list[0].compare("F") == 0)
        {
            arg->socket = new_socket;
            pthread_create(&pt, NULL, &create_file_system, (void *)arg);
            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("RESET") == 0) {
            arg->socket = new_socket;
            pthread_create(&pt, NULL, &delete_file_system, (void *)arg);
            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("C") == 0)
        {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &create_file, (void *)arg);
                pthread_join(pt, NULL);
            }
        }
        else if (command_list[0].compare("D") == 0) {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &delete_file, (void *)arg);
                pthread_join(pt, NULL);
            }
        }
        else if (command_list[0].compare("L") == 0)
        {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->flag = std::stoi(command_list[1]);
                pthread_create(&pt, NULL, &listing, (void *)arg);
                pthread_join(pt, NULL);
            }
        }
        else if (command_list[0].compare("R") == 0)
        {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &read_file, (void *)arg);
                pthread_join(pt, NULL);
            }
        }
        else if (command_list[0].compare("W") == 0)
        {
            if (command_list.size() < 3) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                arg->input = command_list[2];
                pthread_create(&pt, NULL, &write_file, (void *)arg);
                pthread_join(pt, NULL);
            }

        }
        else if (command_list[0].compare("mkdir") == 0) {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &make_directory, (void *)arg);
                pthread_join(pt, NULL);
            }

        }
        else if (command_list[0].compare("cd") == 0) {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &change_directory, (void *)arg);
                pthread_join(pt, NULL);
            }

        }
        else if (command_list[0].compare("pwd") == 0) {
            arg->socket = new_socket;
            pthread_create(&pt, NULL, &print_working_directory, (void *)arg);
            pthread_join(pt, NULL);
        }
        else if (command_list[0].compare("rmdir") == 0) {
            if (command_list.size() < 2) {
                message = "Unknown command, please try again (input case is sensitive)";
                std::cout << "Unknown command received from the client, no information return\n";
                send(new_socket, message.c_str(), message.length(), 0);
            }
            else {
                arg->socket = new_socket;
                arg->file = command_list[1];
                pthread_create(&pt, NULL, &remove_directory, (void *)arg);
                pthread_join(pt, NULL);
            }
        }
        else
        {
            message = "Unknown command, please try again (input case is sensitive)";
            std::cout << "Unknown command received from the client, no information return\n";
            send(new_socket, message.c_str(), message.length(), 0);
        }

        message.clear();
    }

    return 0;
}