#include <stdio.h> 
#include <stdint.h>
#include <iostream>
// #include <fstream>
#include <arpa/inet.h>
using namespace std;

// int open_file(const char* filename) {
// 	fstream my_file;
// 	uint32_t n = 0;
// 	my_file.open(filename, ios::in | ios::binary);
// 	if(!my_file) {
// 		printf("No file!\n");
// 		return 0;
// 	}
// 	else {
// 		my_file.read(reinterpret_cast<char*> (&n), sizeof(n));
// 	}
// 	my_file.close();
// 	return htonl(n);
// }

int open_file(const char* filename) {
    FILE* file = fopen(filename,"rb");
    uint32_t n = 0;
    if (!file) {
        printf("No file!\n");
        exit(EXIT_FAILURE);
    }
    size_t read_size = fread(&n, sizeof(n), 1, file);
    if (read_size != 1) {
        printf("Failed to read 4 bytes\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return htonl(n);
}

int main() {
	uint32_t a = open_file("five-hundred.bin");
	uint32_t b = open_file("thousand.bin");
	uint32_t result = a+b;
	printf("%08x, %d", result, result);
	return 0;
}
