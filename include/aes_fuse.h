#ifndef AES_FUSE_H
#define AES_FUSE_H

#define FUSE_USE_VERSION 29

#include "aes.h"
#include "aes_modes.h"
#include <fuse.h>

// globalna putanja do storage direktorijuma gde se cuvaju enkriptovani fajlovi
extern char storage_path[4096];

// globalni kljuc (256-bit) koji se koristi za enkripciju/dekripciju
extern uint8_t fs_key[AES_256_KEY_SIZE];

// globalni IV za CBC mod
extern uint8_t fs_iv[AES_BLOCK_SIZE];

// callback funkcije koje FUSE poziva
int aes_getattr(const char* path, struct stat* stbuf);

int aes_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);

int aes_open(const char* path, struct fuse_file_info* fi);

int aes_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi);

int aes_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi);

int aes_create(const char* path, mode_t mode, struct fuse_file_info* fi);

int aes_truncate(const char* path, off_t size);

int aes_unlink(const char* path);

#endif //AES_FUSE_H