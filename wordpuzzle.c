#include <ctype.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 2000000000L

char* dict[100000];
int LEN = 8, THREADS = 1, dictionary_size;
int verbose = 0, timeOption = 0, lengthOption = 0, lenCounter;
char** buffer;
char* nthread;
long totalBinarySearchTime;

typedef struct {
    int id, len;
    int* len_list;
} thread_info;

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;

static struct option long_options[] =
    {
        {"len", required_argument, NULL, '1'},
        {"nthreads", required_argument, NULL, '2'},
        {"verbose", no_argument, NULL, '3'},
        {"time", no_argument, NULL, '4'},
        {"sorted", no_argument, NULL, '5'},
        {"distinct", no_argument, NULL, '6'},
        {NULL, 0, NULL, 0}};

long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int binsearch(char* dictionaryWords[], int listSize, char* keyword) {
    int mid, low = 0, high = listSize - 1;
    while (high >= low) {
        mid = (high + low) / 2;
        if (strcmp(dictionaryWords[mid], keyword) < 0)
            low = mid + 1;
        else if (strcmp(dictionaryWords[mid], keyword) > 0)
            high = mid - 1;
        else
            return mid;
    }
    return -1;  // not found
}

void* word_puzzle_solver(void* id) {
    thread_info* all = id;
    for (int j = 0; j < all->len; j++) {
        char c = 0;
        int i, n = BUFFER_SIZE / THREADS - all->len_list[j], time;
        char* buf = buffer[all->id];
        if (verbose) {
            if (j == 0) {
                fprintf(stderr, "Note: Thread #%d: Started! Length: %d\n", all->id, all->len_list[j]);
            } else {
                fprintf(stderr, "Note: Thread #%d: Length: %d\n", all->id, all->len_list[j]);
            }
        }
        for (i = 0; i < n; i++) {
            if (c)
                buf[i + all->len_list[j] - 1] = c;
            c = buf[i + all->len_list[j]];
            buf[i + all->len_list[j]] = '\0';
            if (timeOption) {
                time = get_nanos();
                // pthread_mutex_lock(&id);
                if (binsearch(dict, dictionary_size, buf + i) + 1)  // if search is successful!
                    printf("Thread #%d: %s\n", all->id, buf + i);
                time = get_nanos() - time;
                // lock mutex
                pthread_mutex_lock(&mutex_1);
                totalBinarySearchTime += time;
                // unlock mutex
                pthread_mutex_unlock(&mutex_1);
            } else {
                if (binsearch(dict, dictionary_size, buf + i) + 1)  // if search is successful!
                    printf("Thread #%d: %s\n", all->id, buf + i);
            }
        }
    }
    free(all->len_list);
}

void main(int argc, char** argv) {
    long startTime = get_nanos();
    int opt, validValues = 0, i = 0, thread_number, j;
    char temp[100], line[1000];
    char* tempList;
    int* lengthList;
    while ((opt = getopt_long_only(argc, argv, ":l:n:vtsd", long_options, NULL)) != -1) {
        switch (opt) {
            case '1':
                lengthOption = 1;

                tempList = (char*)malloc(sizeof(char) * (strlen(optarg) + 1));

                strcpy(tempList, optarg);
                char* token = strtok(optarg, " ,\n");
                // Get amount of actual numbers/values
                while (token != NULL) {
                    validValues++;
                    token = strtok(NULL, " ,\n");
                }
                // Allocates the exact amount needed to store the list.
                lengthList = (int*)malloc(sizeof(int) * validValues);
                token = strtok(tempList, " ,\n");
                while (token != NULL) {
                    lengthList[i++] = atoi(token);
                    token = strtok(NULL, " ,\n");
                }
                break;
            case '2':
                nthread = optarg;
                THREADS = atoi(nthread);
                if (THREADS < 1) {
                    THREADS = 1;
                }
                printf("Number of Threads Specified: %d\n", THREADS);
                break;
            case '3':
                printf("-verbose enabled!\n");
                verbose = 1;
                break;
            case '4':
                printf("-time enabled!\n");
                timeOption = 1;
                break;
            case '5':
                printf("s\n");
                break;
            case '6':
                printf("d\n");
                break;
            case '?':
                printf("Unknown arg -%c\n", optopt);
                break;
            case ':':
                printf("Missing option for arg -%c\n", optopt);
                break;
        }
    }
    // Allocates the buffer based -nthreads or default 1.
    buffer = (char**)malloc(THREADS);

    int size = BUFFER_SIZE / THREADS;
    pthread_t threadID[THREADS];
    thread_info info[THREADS];

    FILE* f = fopen("dict.txt", "r");
    dictionary_size = 0;
    while (fgets(line, 1000, f)) {
        sscanf(line, "%s\n", temp);
        if (strlen(temp) == 0)
            continue;
        dict[dictionary_size] = (char*)malloc(sizeof(temp) + 1);
        strcpy(dict[dictionary_size++], temp);
    }
    fclose(f);

    pthread_mutex_init(&mutex_1, NULL);

    for (thread_number = 0; thread_number < THREADS; thread_number++) {
        if (!lengthOption) {
            // Length set to [8,9 default].
            info[thread_number].len_list = (int*)malloc(sizeof(int) * 2);
            info[thread_number].len_list[0] = 8;
            info[thread_number].len_list[1] = 9;
            info[thread_number].len = 2;

        } else {
            info[thread_number].len = validValues;
            info[thread_number].len_list = (int*)malloc(sizeof(int) * validValues);
            // Copies the Length list to each struct.
            memcpy(info[thread_number].len_list, lengthList, sizeof(int) * validValues);
        }
        info[thread_number].id = thread_number;
        buffer[thread_number] = (char*)malloc(size + 1);
        if (!fgets(buffer[thread_number], size + 1, stdin)) {
            fprintf(stderr, "Error: can't read the input stream!\n");
        }

        if (pthread_create(threadID + thread_number, NULL, word_puzzle_solver, &info[thread_number])) {
            fprintf(stderr, "Error: Too many threads are created!\n");
            break;
        }
        // thread_info all [THREADS](void*)(all+ thread_number);
    }

    for (j = 0; j < thread_number; j++) {
        pthread_join(threadID[j], NULL);
        if (verbose) {
            fprintf(stderr, "Note: Thread %d joined!\n", j);
        }
    }

    free(tempList);
    free(lengthList);
    free(buffer);
    
    if (verbose) {
        fprintf(stderr, "Note: Total time: %ld nanoseconds using %d threads!\n",
                get_nanos() - startTime, thread_number);
    }
    if (timeOption) {
        fprintf(stderr, "Note: Total time for Binary Search: %ld nanoseconds using %d threads!\n",
                totalBinarySearchTime, thread_number);
    }
}