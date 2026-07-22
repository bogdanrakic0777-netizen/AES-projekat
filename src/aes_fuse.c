#define FUSE_USE_VERSION 29

#include <fuse.h>
#include "aes_fuse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

//potpuna putanja do fajla
char storage_path[4096];

uint8_t fs_key[AES_256_KEY_SIZE];
uint8_t fs_iv[AES_BLOCK_SIZE];

static void build_storage_path(char* full_path, const char* path) {
    snprintf(full_path, 4096, "%s%s", storage_path, path);
}


//GETATTR
//vraca metapodatke o fajlu ili direktorijumu
int aes_getattr(const char* path, struct stat* stbuf) {
    char full_path[4096];
    build_storage_path(full_path, path);

    int res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

//READDIR
//listanje sadrzaja direktorijuma
int aes_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
    (void) offset;
    (void) fi;


    char full_path[4096];
    build_storage_path(full_path, path);

    //fuse pokusava da otvori direktorijum
    DIR* dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    filler(buf, ".",  NULL, 0);
    filler(buf, "..", NULL, 0);


    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        filler(buf, de->d_name, NULL, 0);
    }

    closedir(dp);
    return 0;
}
//========== OPEN ==========
int aes_open(const char* path, struct fuse_file_info* fi) {
    char full_path[4096];
    build_storage_path(full_path, path);

    int res = open(full_path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

//========== CREATE ==========
int aes_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
    (void) fi;

    char full_path[4096];
    build_storage_path(full_path, path);

    int res = open(full_path, O_CREAT | O_WRONLY | O_TRUNC, mode);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

//========== TRUNCATE ==========
int aes_truncate(const char* path, off_t size) {
    char full_path[4096];
    build_storage_path(full_path, path);

    int res = truncate(full_path, size);
    if (res == -1)
        return -errno;

    return 0;
}

//========== UNLINK ==========
int aes_unlink(const char* path) {
    char full_path[4096];
    build_storage_path(full_path, path);

    int res = unlink(full_path);
    if (res == -1)
        return -errno;

    return 0;
}

//========== READ ==========
//citanje fajla sa automatskim dekriptovanjem (AES-256 CBC)
int aes_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void) fi;

    char full_path[4096];
    build_storage_path(full_path, path);

    FILE *f = fopen(full_path, "rb");
    if (!f)
        return -errno;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size == 0) {
        fclose(f);
        return 0;
    }

    //ucitaj enkriptovane podatke
    uint8_t *encrypted = malloc(file_size);
    if (!encrypted) {
        fclose(f);
        return -ENOMEM;
    }
    fread(encrypted, 1, file_size, f);
    fclose(f);

    //dekriptuj — broj blokova je file_size / 16
    int num_blocks = file_size / AES_BLOCK_SIZE;
    uint8_t *decrypted = malloc(file_size);
    if (!decrypted) {
        free(encrypted);
        return -ENOMEM;
    }

    aes_256_cbc_decrypt(encrypted, decrypted, fs_key, fs_iv, num_blocks);

    //kopiraj trazeni deo u buf
    size_t to_copy = size;
    if ((size_t)offset >= (size_t)file_size)
        to_copy = 0;    //OS trazi van opsega fajla, ne daj mu nista
    else if (offset + size > (size_t)file_size)
        to_copy = file_size - offset;       //OS trazi vise nego sto je ostalo, daj mu samo ostatak

    memcpy(buf, decrypted + offset, to_copy);


    //memset(decrypted, 0, file_size); ali BEZBEDNIJE, OBJASNJENJE U KOMENTARU NA KRAJU FUNKCIJE AES_WRITE
    explicit_bzero(decrypted, file_size);
    free(encrypted);
    free(decrypted);

    return (int)to_copy;
}

//========== WRITE ==========
//pisanje fajla sa automatskim enkriptovanjem (AES-256 CBC)
int aes_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void) fi;
    (void) offset;

    char full_path[4096];
    build_storage_path(full_path, path);

    //zaokruzi velicinu na visekratnik od 16 (AES blok)
    int num_blocks = (size + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE;
    size_t padded_size = num_blocks * AES_BLOCK_SIZE;

    //kopiraj ulaz u padded bafer (ostatak popuni nulama)
    uint8_t* plaintext = calloc(padded_size, 1);
    if (!plaintext)
        return -ENOMEM;
    memcpy(plaintext, buf, size);

    //enkriptuj
    uint8_t* encrypted = malloc(padded_size);
    if (!encrypted) {
        free(plaintext);
        return -ENOMEM;
    }
    aes_256_cbc_encrypt(plaintext, encrypted, fs_key, fs_iv, num_blocks);

    //upisuj na disk
    FILE* f = fopen(full_path, "wb");
    if (!f) {
        free(plaintext);
        free(encrypted);
        return -errno;
    }
    fwrite(encrypted, 1, padded_size, f);
    fclose(f);

    //pre oslobadjanja postavlja nule u RAM gde se nalazio plaintext
    //kako bi zastitio sistem od sofisticiranih napada (RAM scanning)
    //ne koristim memcpy sa nulama, jer neke verzije kompajlera imaju optimizaciju gde izbace tu funkciju ako se memorija odmah oslobodi
    //explicit_bzero ce se sigurno pozvati.
    explicit_bzero(plaintext, padded_size);
    free(plaintext);
    free(encrypted);

    return (int)size;
}
//========== DESETROY ==========
//brisanje bezbednosno kriticnih podataka iz RAM-a u slucaju nasilnog prekida ili CORE-DUMP a 
void aes_destroy(void* private_data) {
    (void) private_data;    //dastovanje na void zbog compiler warninga

    explicit_bzero(fs_key, AES_256_KEY_SIZE);
    explicit_bzero(fs_iv, AES_BLOCK_SIZE);

    printf("\n[FUSE SHUTDOWN] RAM memorija je bezbedno ocistena!\n");

}

//========== FUSE OPERACIJE ==========
static struct fuse_operations aes_ops = {
    .getattr    = aes_getattr,
    .readdir    = aes_readdir,
    .open       = aes_open,
    .read       = aes_read,
    .write      = aes_write,
    .create     = aes_create,
    .truncate   = aes_truncate,
    .unlink     = aes_unlink,
    .destroy    = aes_destroy 
};

//========== MAIN ==========
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Upotreba: %s <storage_dir> <mount_point> [fuse opcije]\n", argv[0]);
        return 1;
    }

    strncpy(storage_path, argv[1], sizeof(storage_path) - 1);

    //ukloni trailing slash
    int len = strlen(storage_path);
    if (len > 1 && storage_path[len-1] == '/')
        storage_path[len-1] = '\0';

    //hardkodovani 256-bit kljuc (FIPS 197 primer)
    uint8_t default_key[AES_256_KEY_SIZE] = {
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    memcpy(fs_key, default_key, AES_256_KEY_SIZE);
    explicit_bzero(default_key, AES_256_KEY_SIZE);  //ocisti se kljuc sa STACK, zbog bezbednosti.

    //fiksni IV
    uint8_t default_iv[AES_BLOCK_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    memcpy(fs_iv, default_iv, AES_BLOCK_SIZE);

    generate_sbox();

    //pomeri argumente da FUSE ne vidi storage_dir
    argv[1] = argv[2];
    argc--;

    return fuse_main(argc, argv, &aes_ops, NULL);
}
