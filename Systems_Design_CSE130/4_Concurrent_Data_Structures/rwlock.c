#include "rwlock.h"
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    bool is_writer_holding; // Whether a writer is currently holding the lock
    uint32_t num_writers_waiting; // Number of writers waiting
    pthread_cond_t writer_cond; // Condition variable to signal writers
} ReaderPriorityState;

typedef struct {
    uint32_t max_readers; // Number of readers allowed before a writer gets the lock
    uint32_t num_readers_waiting; // Number of readers waiting
    uint32_t num_readers_passed; // Number of readers that have passed
    uint32_t num_writers_waiting; // Number of writers waiting
    pthread_cond_t writer_cond; // Condition variable to signal writers
    pthread_cond_t reader_cond; // Condition variable to signal readers
} NWayState;

typedef struct {
    uint32_t num_writers_waiting; // Number of writers waiting
    uint32_t num_readers_waiting; // Number of readers waiting
    pthread_cond_t reader_cond; // Condition variable to signal readers
} WriterPriorityState;

struct rwlock {
    union {
        NWayState nway;
        ReaderPriorityState reader_prio;
        WriterPriorityState writer_prio;
    } state;

    PRIORITY priority_type; // The priority mechanism in use
    uint32_t num_readers_holding; // Number of readers currently holding the lock
    pthread_mutex_t rw_mutex; // Mutex to protect the rwlock structure
    sem_t write_semaphore; // Semaphore to control write access
};

//Extra functions
// Acquire reader lock
void acquire_reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);
    if (rw->num_readers_holding == 0) {
        sem_wait(&rw->write_semaphore); // First reader gets the write lock
    }
    rw->num_readers_holding += 1;
    pthread_mutex_unlock(&rw->rw_mutex);
}

//release reader lock
void release_reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);
    rw->num_readers_holding -= 1;

    if (rw->num_readers_holding == 0 && sem_post(&rw->write_semaphore) != 0) {
    }

    if (rw->num_readers_holding == 0 && rw->state.reader_prio.num_writers_waiting > 0) {
        if (pthread_cond_signal(&rw->state.reader_prio.writer_cond) != 0) {
        }
    }

    pthread_mutex_unlock(&rw->rw_mutex);
}

// Acquire writer lock
void acquire_writer_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);

    rw->state.reader_prio.num_writers_waiting += 1;
    for (; rw->num_readers_holding > 0 || rw->state.reader_prio.is_writer_holding;) {
        pthread_cond_wait(&rw->state.reader_prio.writer_cond,
            &rw->rw_mutex); // Wait until no readers are holding the lock
    }

    rw->state.reader_prio.num_writers_waiting -= 1;
    rw->state.reader_prio.is_writer_holding = true;
    pthread_mutex_unlock(&rw->rw_mutex);
    sem_wait(&rw->write_semaphore); // Get the write lock
}

// Release writer lock
void release_writer_lock(rwlock_t *rw) {
    sem_post(&rw->write_semaphore);
    pthread_mutex_lock(&rw->rw_mutex);

    rw->state.reader_prio.is_writer_holding = false;
    // Check if there are waiting writers or no readers holding the lock
    if (rw->state.reader_prio.num_writers_waiting > 0 || rw->num_readers_holding == 0) {
        if (pthread_cond_signal(&rw->state.reader_prio.writer_cond) != 0) {
        }
    }
    pthread_mutex_unlock(&rw->rw_mutex);
}

// Acquire reader lock with writer priority
void acquire_reader_lock_writer_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);

    rw->state.writer_prio.num_readers_waiting += 1;
    for (; rw->state.writer_prio.num_writers_waiting > 0;) {
        pthread_cond_wait(
            &rw->state.writer_prio.reader_cond, &rw->rw_mutex); // Wait until no writers are waiting
    }
    if (rw->num_readers_holding == 0) {
        sem_wait(&rw->write_semaphore); // First reader gets the write lock
    }
    rw->state.writer_prio.num_readers_waiting -= 1;
    rw->num_readers_holding += 1;

    pthread_mutex_unlock(&rw->rw_mutex);
}

// Release reader lock with writer priority
void release_reader_lock_writer_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);
    rw->num_readers_holding -= 1;
    if (rw->num_readers_holding == 0) {
        sem_post(&rw->write_semaphore); // Last reader releases the write lock
    }
    if (rw->state.writer_prio.num_writers_waiting == 0
        && rw->state.writer_prio.num_readers_waiting > 0) {
        pthread_cond_signal(&rw->state.writer_prio.reader_cond); // Wake up one waiting reader
    }
    // Additional check to avoid signaling when no readers are waiting
    if (rw->state.writer_prio.num_readers_waiting == 0) {
        // No need to signal if no readers are waiting
    }
    pthread_mutex_unlock(&rw->rw_mutex);
}

// Acquire writer lock with writer priority
void acquire_writer_lock_writer_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);

    rw->state.writer_prio.num_writers_waiting += 1;
    pthread_mutex_unlock(&rw->rw_mutex);
    sem_wait(&rw->write_semaphore); // Get the write lock
}

// Release writer lock with writer priority
void release_writer_lock_writer_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);

    if (rw->state.writer_prio.num_writers_waiting > 0) {
        pthread_cond_broadcast(&rw->state.writer_prio.reader_cond); // Wake up waiting readers
    }
    rw->state.writer_prio.num_writers_waiting -= 1;
    sem_post(&rw->write_semaphore);
    pthread_mutex_unlock(&rw->rw_mutex);
}

// Acquire reader lock
void acquire_reader_lock_nway_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);

    rw->state.nway.num_readers_waiting += 1;
    while (rw->state.nway.num_readers_passed >= rw->state.nway.max_readers
           && rw->state.nway.num_writers_waiting > 0) {
        pthread_cond_wait(&rw->state.nway.reader_cond,
            &rw->rw_mutex); // Wait until less than N readers have passed or no writers are waiting
    }
    if (rw->state.nway.num_readers_passed < rw->state.nway.max_readers) {
        rw->state.nway.num_readers_passed += 1;
    }
    rw->state.nway.num_readers_waiting -= 1;
    if (rw->num_readers_holding == 0) {
        sem_wait(&rw->write_semaphore); // First reader gets the write lock
    }
    rw->num_readers_holding += 1;
    pthread_mutex_unlock(&rw->rw_mutex);
}

// Release reader lock
void release_reader_lock_nway_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);
    rw->num_readers_holding -= 1;
    if (rw->num_readers_holding == 0) {
        sem_post(&rw->write_semaphore); // Last reader releases the write lock

        if (rw->state.nway.num_writers_waiting > 0) {
            if (rw->state.nway.num_readers_passed >= rw->state.nway.max_readers
                || rw->state.nway.num_readers_waiting == 0) {
                pthread_cond_signal(
                    &rw->state.nway
                         .writer_cond); // Wake up a waiting writer if N readers have passed
            } else {
                uint32_t should_wake
                    = rw->state.nway.max_readers - rw->state.nway.num_readers_passed;
                if (should_wake > rw->state.nway.num_readers_waiting) {
                    pthread_cond_broadcast(
                        &rw->state.nway.reader_cond); // Wake up all waiting readers
                } else {
                    uint32_t i = 0;
                    do {
                        pthread_cond_signal(
                            &rw->state.nway.reader_cond); // Wake up the next N waiting readers
                        i += 1;
                    } while (i < should_wake);
                }
            }
        } else {
            pthread_cond_broadcast(
                &rw->state.nway.reader_cond); // Wake up waiting readers if no writers are waiting
        }
    }
    pthread_mutex_unlock(&rw->rw_mutex);
}

void acquire_writer_lock_nway_priority(rwlock_t *rw) {
    pthread_mutex_lock(&rw->rw_mutex);
    rw->state.nway.num_writers_waiting += 1;
    do {
        pthread_cond_wait(&rw->state.nway.writer_cond,
            &rw->rw_mutex); // Wait until no readers are holding or fewer than N readers have passed
    } while (rw->num_readers_holding > 0
             || (rw->state.nway.num_readers_passed < rw->state.nway.max_readers
                 && rw->state.nway.num_readers_waiting > 0));
    pthread_mutex_unlock(&rw->rw_mutex);
    sem_wait(&rw->write_semaphore); // Get the write lock
}

// Release writer lock
void release_writer_lock_nway_priority(rwlock_t *rw) {
    sem_post(&rw->write_semaphore);
    pthread_mutex_lock(&rw->rw_mutex);
    rw->state.nway.num_writers_waiting -= 1;
    rw->state.nway.num_readers_passed = 0;
    if (rw->state.nway.num_readers_waiting > 0) {
        uint32_t should_wake = rw->state.nway.max_readers;
        if (rw->state.nway.num_readers_waiting > should_wake) {
            uint32_t i = 0;
            do {
                pthread_cond_signal(
                    &rw->state.nway.reader_cond); // Wake up the next N waiting readers
                i += 1;
            } while (i < should_wake);
        } else {
            pthread_cond_broadcast(&rw->state.nway.reader_cond); // Wake up all waiting readers
        }
    } else {
        pthread_cond_signal(&rw->state.nway.writer_cond); // Wake up a waiting writer
    }
    pthread_mutex_unlock(&rw->rw_mutex);
}

// Initialize a new rwlock
rwlock_t *rwlock_new(PRIORITY p, uint32_t n) {
    rwlock_t *rw = malloc(sizeof(rwlock_t));
    if (!rw)
        return NULL;

    rw->priority_type = p;
    rw->num_readers_holding = 0;
    pthread_mutex_init(&rw->rw_mutex, NULL);
    sem_init(&rw->write_semaphore, 0, 1);

    if (p == N_WAY) {
        if (n == 0) {
            free(rw);
            return NULL;
        }
        rw->state.nway.max_readers = n;
        rw->state.nway.num_readers_waiting = 0;
        rw->state.nway.num_readers_passed = 0;
        rw->state.nway.num_writers_waiting = 0;
        pthread_cond_init(&rw->state.nway.writer_cond, NULL);
        pthread_cond_init(&rw->state.nway.reader_cond, NULL);
    } else if (p == WRITERS) {
        rw->state.writer_prio.num_writers_waiting = 0;
        rw->state.writer_prio.num_readers_waiting = 0;
        pthread_cond_init(&rw->state.writer_prio.reader_cond, NULL);
    } else if (p == READERS) {
        rw->state.reader_prio.is_writer_holding = false;
        rw->state.reader_prio.num_writers_waiting = 0;
        pthread_cond_init(&rw->state.reader_prio.writer_cond, NULL);
    } else {
        free(rw);
        return NULL;
    }

    return rw;
}

// Delete a rwlock
void rwlock_delete(rwlock_t **rw) {
    if (rw == NULL || *rw == NULL)
        return;

    if ((*rw)->priority_type == N_WAY) {
        pthread_cond_destroy(&(*rw)->state.nway.reader_cond);
        pthread_cond_destroy(&(*rw)->state.nway.writer_cond);
    } else if ((*rw)->priority_type == WRITERS) {
        pthread_cond_destroy(&(*rw)->state.writer_prio.reader_cond);
    } else if ((*rw)->priority_type == READERS) {
        pthread_cond_destroy(&(*rw)->state.reader_prio.writer_cond);
    }

    pthread_mutex_destroy(&(*rw)->rw_mutex);
    sem_destroy(&(*rw)->write_semaphore);
    free(*rw);
    *rw = NULL;
}

// Reader lock acquisition based on priority
void reader_lock(rwlock_t *rw) {
    if (rw == NULL)
        return;

    if (rw->priority_type == N_WAY) {
        acquire_reader_lock_nway_priority(rw);
    } else if (rw->priority_type == READERS) {
        acquire_reader_lock(rw);
    } else if (rw->priority_type == WRITERS) {
        acquire_reader_lock_writer_priority(rw);
    }
}

// Reader lock release based on priority
void reader_unlock(rwlock_t *rw) {
    if (rw == NULL)
        return;

    if (rw->priority_type == N_WAY) {
        release_reader_lock_nway_priority(rw);
    } else if (rw->priority_type == READERS) {
        release_reader_lock(rw);
    } else if (rw->priority_type == WRITERS) {
        release_reader_lock_writer_priority(rw);
    }
}

// Writer lock acquisition based on priority
void writer_lock(rwlock_t *rw) {
    if (rw == NULL)
        return;

    if (rw->priority_type == N_WAY) {
        acquire_writer_lock_nway_priority(rw);
    } else if (rw->priority_type == READERS) {
        acquire_writer_lock(rw);
    } else if (rw->priority_type == WRITERS) {
        acquire_writer_lock_writer_priority(rw);
    }
}

// Writer lock release based on priority
void writer_unlock(rwlock_t *rw) {
    if (rw == NULL)
        return;

    if (rw->priority_type == N_WAY) {
        release_writer_lock_nway_priority(rw);
    } else if (rw->priority_type == READERS) {
        release_writer_lock(rw);
    } else if (rw->priority_type == WRITERS) {
        release_writer_lock_writer_priority(rw);
    }
}
