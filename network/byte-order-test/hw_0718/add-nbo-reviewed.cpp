#include <stdio.h>
#include <string.h>
#include <stdlib.h> // exit 사용하기 위한 헤더파일
#include <errno.h>
#include <stdint.h>
#include <arpa/inet.h>

void usage() {
    printf("syntas: add-nbo-reviewed <file1> <file2>\n");
    printf("sample: add-nbo-reviewed a.bin b.bin\n");
}

uint32_t add_nbo(char* fileName) {
    FILE* file = fopen(fileName, "rb");
    // 파일이 없으면 에러 메시지 출력 후 종료
    if (file == nullptr) {
        fprintf(stderr, "Failed to open file %s: %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }
    uint32_t n;
    // fread 함수를 통해 4바이트를 읽어 n에 저장
    size_t readlen = fread(&n, 1, sizeof(n), file); 
    // 4바이트를 읽지 못하면 에러 메시지 출력 후 종료
    if (readlen != sizeof(n)) {
        fprintf(stderr, "Failed to read 4 bytes from %s\n", fileName);
        exit(EXIT_FAILURE);
    } 
    // 네트워크 바이트 순서로 저장된 n을 호스트 바이트 순서로 변환
    return ntohl(n);
} 

int main(int argc, char* argv[]) {
    // 인자가 3개가 아니면 사용법 출력 후 종료
    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }
    uint32_t n1 = add_nbo(argv[1]);
    uint32_t n2 = add_nbo(argv[2]);

    printf("%d(0x%x) + %d(0x%x) = %d(0x%x)\n", n1, n1, n2, n2, n1 + n2, n1 + n2);  

    return 0;
}