#include "../io/filefn.c"
#include "../io/iofn.c"

int main() {
    // // Test 1: Write a file
    char *filename = "test_output.txt";
    char *message = "Hello from FILE_HANDLE wrapper!\n";

    struct FILE_HANDLE *fw = fopen(filename, "w");
    if (!fw) {
        printf("Failed to open file for writing\n", 0);
        return 1;
    }
    
    fwrite(message, 1, 30, fw);
    fclose(fw);
    printf("File written successfully.\n", 0);

    // Test 2: Read the file back
    struct FILE_HANDLE *fr = fopen(filename, "r");
    if (!fr) {
        printf("Failed to open file for reading\n", 0);
        return 1;
    }

    char buffer[128] = {0};
    size_t read = fread(buffer, 1, sizeof(buffer) - 1, fr);
    fclose(fr);

    print_int(read);
    nline();
    print(buffer);

    // Test 3: Remove file
    if (remove_file(filename) == 0)
        printf("File deleted successfully.\n", 0);
    else
        printf(" File delete failed.\n", 0);

    return 0;
}
