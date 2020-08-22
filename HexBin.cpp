#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cctype>

static const size_t BUFF_LEN = 1024 * 2;
static const size_t LINE_LEN = 16;
static const size_t SIZE_EACH_BUFF = 128;  // BUFF_LEN/LINE_LEN

void usage(const char *bin_name)
{
    printf("Usage: %s path_of_bin_file \n", bin_name);
}

int make_print(int ch)
{
    if (ch < 0 || ch > 0xFF) return '.';
    if (isprint(ch)) return ch;
    return '.';
}

void print_bin(int fd, size_t num, ssize_t length, const char *source)
{
    size_t begin = num * SIZE_EACH_BUFF;
    size_t count = length / LINE_LEN;
    if (count <= 0 || length % LINE_LEN != 0)
        count += 1;

    for (size_t i = 0; i < count; ++i)
    {
        char buffer[256] = {0x00};
        snprintf(buffer, 256,
                 "%08lX  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ; %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                 begin + i,

                 uint8_t(source[i * LINE_LEN]     ), uint8_t(source[i * LINE_LEN +  1]),
                 uint8_t(source[i * LINE_LEN +  2]), uint8_t(source[i * LINE_LEN +  3]),
                 uint8_t(source[i * LINE_LEN +  4]), uint8_t(source[i * LINE_LEN +  5]),
                 uint8_t(source[i * LINE_LEN +  6]), uint8_t(source[i * LINE_LEN +  7]),
                 uint8_t(source[i * LINE_LEN +  8]), uint8_t(source[i * LINE_LEN +  9]),
                 uint8_t(source[i * LINE_LEN + 10]), uint8_t(source[i * LINE_LEN + 11]),
                 uint8_t(source[i * LINE_LEN + 12]), uint8_t(source[i * LINE_LEN + 13]),
                 uint8_t(source[i * LINE_LEN + 14]), uint8_t(source[i * LINE_LEN + 15]),

                 make_print(source[i * LINE_LEN]     ), make_print(source[i * LINE_LEN +  1]),
                 make_print(source[i * LINE_LEN +  2]), make_print(source[i * LINE_LEN +  3]),
                 make_print(source[i * LINE_LEN +  4]), make_print(source[i * LINE_LEN +  5]),
                 make_print(source[i * LINE_LEN +  6]), make_print(source[i * LINE_LEN +  7]),
                 make_print(source[i * LINE_LEN +  8]), make_print(source[i * LINE_LEN +  9]),
                 make_print(source[i * LINE_LEN + 10]), make_print(source[i * LINE_LEN + 11]),
                 make_print(source[i * LINE_LEN + 12]), make_print(source[i * LINE_LEN + 13]),
                 make_print(source[i * LINE_LEN + 14]), make_print(source[i * LINE_LEN + 15])
                );

        printf("%s", buffer);
        size_t str_len = strlen(buffer);
        write(fd, buffer, str_len);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv[0]);
        exit(1);
    }

    const char *file_name = argv[1];
    const char *out_name = argv[2];
    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
    {
        printf("can not open file: %s\n", file_name);
        exit(1);
    }

    int outfd = open(out_name, O_WRONLY);
    if (outfd < 0)
    {
        printf("can not open write file: %s\n", out_name);
    }

    struct stat st {};
    if (stat(file_name, &st) < 0)
    {
        printf("can not get length of file: %s\n", file_name);
        exit(1);
    }

    off_t file_length = st.st_size;
    printf("file:%s whole length %lld.\n", file_name, file_length);

    off_t count_num = file_length / BUFF_LEN;
    if (count_num <= 0 || file_length % BUFF_LEN != 0)
        count_num += 1;

    // read 2K each
    char buffer[BUFF_LEN] = {0x00};
    for (off_t i = 0; i < count_num; ++i)
    {
        memset(buffer, 0x00, BUFF_LEN);
        ssize_t len = read(fd, buffer, BUFF_LEN);
        print_bin(outfd, i, len, buffer);
    }

    if (fd > 0)
    {
        close(fd);
    }

    if (outfd > 0)
    {
        close(outfd);
    }

    return 0;
}
