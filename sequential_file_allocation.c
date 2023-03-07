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
} Block;

typedef struct {
    char name[MAX_NAME_SIZE];
    int fileSize;
} File;

void print_configurations(){
    printf("File Allocation Mode: SEQUENTIAL\n");
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

int find_block_to_place_file_from(Block* blocks, File file){
    int startSequence, j;
    int MAX_LOOP_AMOUNT = 75;
    int i = 0;
    srand(time(NULL));

    startSequence = -1;
    while (startSequence == -1) {
        // Prevent looping forever if there is no space
        if (MAX_LOOP_AMOUNT <= i){ break; }

        int startIndex = (rand() % TOTAL_BLOCKS) + (INODE_SIZE + 1);

        if (startIndex + file.fileSize >= TOTAL_BLOCKS) { continue; }

        // From the generated index, check if there is enough space to place the file.
        int hasSpace = 1;
        for (j = startIndex; j < startIndex + file.fileSize; j++) {
            if (blocks[j].allocatedFlag == 1) {
                hasSpace = 0;
                break;
            }
        }

        if (hasSpace) {
            startSequence = startIndex;
        }
        ++i;
    }

    return startSequence;
}

void place_file_in_block(Block* block, File file, int startIndex){
    int i;
    for (i = 0; i < file.fileSize; ++i){
        block[i + startIndex].allocatedFlag = 1;
        strcpy(block[i + startIndex].allocatedName, file.name);
    }
}

void print_file_details_in_block(Block* blocks){
    int i, j, k;

    int blocksOccupiedCount, lastAllocated;
    printf("\n%-20s %-20s %s\n", "Name", "Blocks Occupied", "Allocated Indexes");
    for (i = INODE_SIZE + 1; i < TOTAL_BLOCKS; i++) {
        if (blocks[i].allocatedFlag) {
            blocksOccupiedCount = 1;
            lastAllocated = i;
            for (j = i + 1; j < TOTAL_BLOCKS; j++) {
                if (strcmp(blocks[j].allocatedName, blocks[i].allocatedName) == 0) {
                    blocksOccupiedCount++;
                    lastAllocated = j;
                } else {
                    break;
                }
            }
            printf("%-20s %-20d %d", blocks[i].allocatedName, blocksOccupiedCount, i);
            if (blocksOccupiedCount > 1) {
                printf("-");
                for (k = i + 1; k < lastAllocated; k++) {
                    printf("%d-", k);
                }
                printf("%d", lastAllocated);
            }
            printf("\n");
            i = lastAllocated;
        }
    }
}

int main() {
    int i;
    Block blocks[TOTAL_BLOCKS];

    // Initalize all the blocks to be free, first few blocks allocated to iNode.
    for (i = 0; i < TOTAL_BLOCKS; ++i){
        if (i <= INODE_SIZE) {
            strcpy(blocks[i].allocatedName, "iNode");
            blocks[i].allocatedFlag = 1;
        } else {
            blocks[i].allocatedFlag = 0;
            strcpy(blocks[i].allocatedName, "");
        }
    }

    print_configurations();

    int fileCount = request_user_for_positive_integer("Enter no of files: ");
    int createdFileCount = 0;

    while (createdFileCount < fileCount){
        File newFile = request_create_file(createdFileCount + 1);

        int startPlacementIndex = find_block_to_place_file_from(blocks, newFile);
        if (startPlacementIndex == -1){
            printf("Insufficient space to place the file. Input another file with a smaller size.\n");
            continue;
        }

        place_file_in_block(blocks, newFile, startPlacementIndex);
        ++createdFileCount;
    }

    print_file_details_in_block(blocks);

    return 0;
}
