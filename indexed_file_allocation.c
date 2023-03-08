#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_NAME_SIZE 32
// In KB
#define BLOCK_SIZE 1
#define TOTAL_BLOCKS 32

#define INODE_SIZE 7

typedef struct {
    char allocatedName[MAX_NAME_SIZE];
    int allocatedFlag;

    // Used to index the next block.
    int nextBlockIndex;
} Block;

/**
Represents a stored file in the system.
*/
typedef struct {
    char name[MAX_NAME_SIZE];
    int fileSize;
    int* storedBlocks;
} SystemFile;

/**
Represents a file (user file requests to be stored).
*/
typedef struct {
    char name[MAX_NAME_SIZE];
    int fileSize;
} File;

void print_configurations(){
    printf("File Allocation Mode: INDEXED\n");
    printf("Total Blocks: %d\n", TOTAL_BLOCKS);
    printf("File allocation starts at block: %d\n", INODE_SIZE + 1);
    printf("File allocation ends at block: %d\n", TOTAL_BLOCKS - 1);
    printf("Size (kB) of each block: %d\n\n", BLOCK_SIZE);
}

int request_user_for_positive_integer(char* requestMessage){
    int result = -1;
    while (result <= 0){
        printf("%s", requestMessage);
        scanf("%d", &result);
        getchar(); // consume the newline character left in the input buffer

        if (result <= 0){
            printf("A non-zero positive number is required!\n");
        }
    }
    return result;
}

File request_create_file(int fileNumber){
    File userFile;

    char input[MAX_NAME_SIZE - 1];
    printf("\nEnter name of file #%d: ", fileNumber);
    fgets(input, MAX_NAME_SIZE - 1, stdin);
    input[strcspn(input, "\n")] = '\0';
    strcpy(userFile.name, input);

    int MAX_FILE_SIZE = TOTAL_BLOCKS - (INODE_SIZE + 1);
    userFile.fileSize = MAX_FILE_SIZE + 1;
    while (userFile.fileSize > MAX_FILE_SIZE){
        userFile.fileSize = request_user_for_positive_integer("Enter the size (kB) of file: ");

        if (userFile.fileSize > MAX_FILE_SIZE){
            printf("File is too large for the system!\nHint: System has %dkB.\n", MAX_FILE_SIZE);
        }
    }
    return userFile;
}

int* find_blocks_to_place_file_from(Block* blocks, File file){
    int startSequence, j, k;
    int MAX_LOOP_AMOUNT = 75;
    int i = 0;
    srand(time(NULL));

    int* blocksIndex = (int*) malloc(file.fileSize * sizeof(int));
    int currentlyFoundSize = 0;
    int foundSpaceFlag = 1;

    int currentIndex = -1;
    // Recursively find a free block to place the file in,
    // for each block the file requires.
    for (j = 0; j < file.fileSize; ++j) {

        int foundNewFlag = 0;
        while (foundNewFlag == 0) {
            // Prevent looping forever if there is no space
            if (MAX_LOOP_AMOUNT <= i){
                    foundSpaceFlag = 0;
                    break;
            }

            int currentIndex = (rand() % (TOTAL_BLOCKS - INODE_SIZE - 1)) + (INODE_SIZE + 1);

            // Ensure block isn't occupied by another file.
            int isOccupiedFlag = 0;
            if (blocks[currentIndex].allocatedFlag == 1){
                isOccupiedFlag = 1;
            }

            // Ensure that when we store the file, no two index intersects itself.
            if (isOccupiedFlag == 0){
                for (k = 0;  k < currentlyFoundSize; ++k){
                    if (blocksIndex[k] == currentIndex) {
                        isOccupiedFlag = 1;
                        break;
                    }
                }
            }

            // Block can be allocated.
            if (isOccupiedFlag == 0){
                blocksIndex[currentlyFoundSize] = currentIndex;
                foundNewFlag = 1;
                ++currentlyFoundSize;
            }

            ++i;
        }

        if (foundSpaceFlag == 0){
            break;
        }
        i = 0;
    }

    if (foundSpaceFlag == 0){
        return NULL;
    }
    return blocksIndex;
}

void place_file_in_blocks(Block* blocks, File file, int* placementIndexes, SystemFile* systemFiles, int systemFileCount){
    int i;

    // Store into the blocks first.
    for (i = 0; i < file.fileSize; ++i){
        // If we reach the end of file, indicate there is no next block.
        // else, set to the next block index.
        int nextBlockIndex = (i + 1) >= file.fileSize ? -1 : placementIndexes[i + 1];
        int currentIndex = placementIndexes[i];

        blocks[currentIndex].allocatedFlag = 1;
        blocks[currentIndex].nextBlockIndex = nextBlockIndex;
        strcpy(blocks[currentIndex].allocatedName, file.name);
    }

    // Store into system files.
    strcpy(systemFiles[systemFileCount].name, file.name);
    systemFiles[systemFileCount].fileSize = file.fileSize;
    systemFiles[systemFileCount].storedBlocks = placementIndexes;
}

void print_file_details(SystemFile* systemFiles, int systemFilesCount){
    int i, j;

    printf("\n%-20s %-20s %s\n", "Name", "Blocks Occupied", "Allocated Indexes");
    // Recursively print for each file.
    for (i = 0; i < systemFilesCount; ++i){
        printf("%-20s %-20d ", systemFiles[i].name, systemFiles[i].fileSize);

        // Print allocated indexes.
        for (j = 0; j < systemFiles[i].fileSize; ++j){
            if (j + 1 >= systemFiles[i].fileSize) {
                printf("%d", systemFiles[i].storedBlocks[j]);
            } else {
                printf("%d-", systemFiles[i].storedBlocks[j]);
            }
        }
        printf("\n");
    }
}

int main() {
    int i;
    Block blocks[TOTAL_BLOCKS];
    SystemFile systemFiles[TOTAL_BLOCKS];

    // Initalize all the blocks to be free, first few blocks allocated to iNode.
    for (i = 0; i < TOTAL_BLOCKS; ++i){
        if (i <= INODE_SIZE) {
            strcpy(blocks[i].allocatedName, "iNode");
            blocks[i].allocatedFlag = 1;
        } else {
            blocks[i].allocatedFlag = 0;
            strcpy(blocks[i].allocatedName, "");
        }
        blocks[i].nextBlockIndex = -1;
    }

    print_configurations();

    int fileCount = request_user_for_positive_integer("Enter no of files: ");
    int createdFileCount = 0;

    while (createdFileCount < fileCount){
        File newFile = request_create_file(createdFileCount + 1);

        int* filePlacements = find_blocks_to_place_file_from(blocks, newFile);
        if (filePlacements == NULL){
            printf("Insufficient space to place the file. Input another file with a smaller size.\n");
            continue;
        }

        place_file_in_blocks(blocks, newFile, filePlacements, systemFiles, createdFileCount);
        ++createdFileCount;
    }

    print_file_details(systemFiles, createdFileCount);

    //free
    for (i = 0; i < createdFileCount; ++i){
        free (systemFiles[i].storedBlocks);
    }

    return 0;
}
