# File_Allocation_Algorithm
Some file algorithm implemented as part of University's submission


### File Allocation Task
1. Write a C program to simulate the Linked list, Indexed, and Sequential file allocation in a very simple file system.
2. Assume a disk of 32 blocks, each block is of 1 KB size
3. First 8 blocks (0 to 7) are allocated to the `iNodes` and can’t be used by the file system. Hence blocks
available for allocation are from block 8 to block 31.
4. Minimum file size is 1 KB. Hence the file system can have minimum of one file of size 24 KB or maximum
of 24 files.
5. At the start, it is assumed that the file system has no files.
6. The program shall ask the user to input the number of files to allocate and their respective names and
file sizes.
7. The program shall randomly (set a seed with `srand(seed)` to replicate the randomness) select any free
block as a start block. Subsequent blocks shall be randomly picked (based on file size) if free.
8. All selected blocks of a file shall be maintained in a linked list.
9. After allocating buffers for all the files, the program shall print file name, file size, and the allocated
linked list of blocks for each file. Refer to the program output shown below.
10. Not required test cases:
 * If there are not enough contiguous blocks available for a file, the program can exit.
 * Program need not implement file deletion or modification. 

### File Handling Task
1. Create a text file “Lab_Quick.txt” containing the following text:
`THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG`
2. Create a c program to read the file `Lab_Quick.txt`.
3. Encrypt the content using Caesar Cipher substitution encryption and write the
encrypted content in text mode to file `Lab_EncryptedText.txt`. Close the file.
 * Note: Assume only capital letters (A to Z) are used in plain text.
4. Open and read the file `Lab_EncryptText.txt` in text mode and display its content.
5. Decrypt the content using Caesar Cipher substitution decryption and display.
6. Write the decrypted contents in a binary mode file `Lab_DecryptBin.txt`. Close the file.
7. Open and read `Lab_DecryptBin.txt` in binary mode and display the contents.
8. Write different types (integer, float etc.) of data into a binary mode file `Lab_BinData.bin` 
