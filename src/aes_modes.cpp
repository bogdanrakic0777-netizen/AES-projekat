extern "C" {
    #include "aes.h"
    #include "aes_modes.h"
}

#include <thread>
#include <vector>
#include <string>

static void aes_128_ecb_encrypt_worker(thread_128_arg_t* arg) {
    //prodje kroz celu svoju sekciju
    for(int i = arg->start_block; i < (arg->start_block + arg->num_blocks); i++) {
        aes_128_encrypt(arg->input + i*AES_BLOCK_SIZE, arg->key, arg->output + i*AES_BLOCK_SIZE);
    }
}

static void aes_128_ecb_decrypt_worker(thread_128_arg_t* arg) {
    for(int i = arg->start_block; i < (arg->start_block + arg->num_blocks); i++) {
        aes_128_decrypt((uint8_t*)(arg->input + i*AES_BLOCK_SIZE), arg->key, arg->output + i*AES_BLOCK_SIZE);
    }
}

void aes_128_ecb_encrypt_thrd(const uint8_t *input, uint8_t *output, const uint8_t key[AES_KEY_SIZE], int num_blocks, int num_threads) {
    if(num_threads > num_blocks) {
        num_threads = num_blocks;
    }
    
    std::vector<std::thread> threads;
    std::vector<thread_128_arg_t> args;

    int blocks_per_thread = num_blocks / num_threads;
    int remainder = num_blocks % num_threads;
    int start = 0;

    thread_128_arg_t temp_arg;
    for(int i = 0; i < num_threads; i++) {
        //svaka nit ce imati iste input, output i key

        temp_arg.input = input;
        
        temp_arg.output = output;
        
        for(int k = 0; k < AES_KEY_SIZE; k++) {
            temp_arg.key[k] = key[k];
        }

        temp_arg.start_block = start;
        temp_arg.num_blocks = blocks_per_thread;
        if(i == num_threads - 1) {
            temp_arg.num_blocks += remainder;
        }

        //podesi start za slececu iteraciju
        start += temp_arg.num_blocks;

        args.push_back(temp_arg);
    }

    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(aes_128_ecb_encrypt_worker, &args[i]);
    }

    for(int i = 0; i < num_threads; i++) {
        threads[i].join();
    }
}

void aes_128_ecb_decrypt_thrd(const uint8_t* input, uint8_t* output, const uint8_t key[AES_KEY_SIZE], int num_blocks, int num_threads) {

    if(num_threads > num_blocks) {
        num_threads = num_blocks;
    }

    std::vector<std::thread> threads;
    std::vector<thread_128_arg_t> args;

    int blocks_per_thread = num_blocks / num_threads;
    int remainder = num_blocks % num_threads;
    int start = 0;

    thread_128_arg_t temp_arg;
    for(int i = 0; i < num_threads; i++) {
        //svaka nit ce imati iste input, output i key

        temp_arg.input = input;
        
        temp_arg.output = output;
        
        for(int k = 0; k < AES_KEY_SIZE; k++) {
            temp_arg.key[k] = key[k];
        }

        temp_arg.start_block = start;
        temp_arg.num_blocks = blocks_per_thread;
        if(i == num_threads - 1) {
            temp_arg.num_blocks += remainder;
        }

        //podesi start za slececu iteraciju
        start += temp_arg.num_blocks;

        args.push_back(temp_arg);
    }

    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(aes_128_ecb_decrypt_worker, &args[i]);
    }

    for(int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

}


static void aes_256_ecb_encrypt_worker(thread_256_arg_t* arg) {
    //prodje kroz celu svoju sekciju
    for(int i = arg->start_block; i < (arg->start_block + arg->num_blocks); i++) {
        aes_256_encrypt(arg->input + i*AES_BLOCK_SIZE, arg->key, arg->output + i*AES_BLOCK_SIZE);
    }
}

static void aes_256_ecb_decrypt_worker(thread_256_arg_t* arg) {
    for(int i = arg->start_block; i < (arg->start_block + arg->num_blocks); i++) {
        aes_256_decrypt(arg->output + i*AES_BLOCK_SIZE  , arg->key,   (uint8_t*)(arg->input + i*AES_BLOCK_SIZE));
    }
}


void aes_256_ecb_encrypt_thrd(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], int num_blocks, int num_threads) {
    if(num_threads > num_blocks) {
        num_threads = num_blocks;
    }

    std::vector<std::thread> threads;
    std::vector<thread_256_arg_t> args;

    int blocks_per_thread = num_blocks / num_threads;
    int remainder = num_blocks % num_threads;
    int start = 0;

    thread_256_arg_t temp_arg;
    for(int i = 0; i < num_threads; i++) {
        //svaka nit ce imati iste input, output i key

        temp_arg.input = input;
        
        temp_arg.output = output;
        
        for(int k = 0; k < AES_256_KEY_SIZE; k++) {
            temp_arg.key[k] = key[k];
        }

        temp_arg.start_block = start;
        temp_arg.num_blocks = blocks_per_thread;
        if(i == num_threads - 1) {
            temp_arg.num_blocks += remainder;
        }

        //podesi start za slececu iteraciju
        start += temp_arg.num_blocks;

        args.push_back(temp_arg);
    }

    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(aes_256_ecb_encrypt_worker, &args[i]);
    }

    for(int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

}

void aes_256_ecb_decrypt_thrd(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], int num_blocks, int num_threads) {

    if(num_threads > num_blocks) {
        num_threads = num_blocks;
    }

    std::vector<std::thread> threads;
    std::vector<thread_256_arg_t> args;

    int blocks_per_thread = num_blocks / num_threads;
    int remainder = num_blocks % num_threads;
    int start = 0;

    thread_256_arg_t temp_arg;
    for(int i = 0; i < num_threads; i++) {
        //svaka nit ce imati iste input, output i key

        temp_arg.input = input;
        
        temp_arg.output = output;
        
        for(int k = 0; k < AES_256_KEY_SIZE; k++) {
            temp_arg.key[k] = key[k];
        }

        temp_arg.start_block = start;
        temp_arg.num_blocks = blocks_per_thread;
        if(i == num_threads - 1) {
            temp_arg.num_blocks += remainder;
        }

        //podesi start za slececu iteraciju
        start += temp_arg.num_blocks;

        args.push_back(temp_arg);
    }

    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(aes_256_ecb_decrypt_worker, &args[i]);
    }

    for(int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

}