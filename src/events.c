#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../include/simulator.h"

struct EventHeap {
    Session *data;
    int size;
    int capacity;
};

EventHeap* event_heap_create(int initial_capacity) {
    EventHeap *heap = (EventHeap*)malloc(sizeof(EventHeap));
    if (!heap) return NULL;
    if (initial_capacity < 16) initial_capacity = 16;
    heap->data = (Session*)malloc(initial_capacity * sizeof(Session));
    if (!heap->data) {
        free(heap);
        return NULL;
    }
    heap->size = 0;
    heap->capacity = initial_capacity;
    return heap;
}

void event_heap_destroy(EventHeap *heap) {
    if (heap) {
        if (heap->data) free(heap->data);
        free(heap);
    }
}

static bool session_earlier(const Session *a, const Session *b) {
    if (a->end_time < b->end_time) return true;
    if (a->end_time > b->end_time) return false;
    return a->request_id < b->request_id;
}

static void heapify_up(EventHeap *heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (session_earlier(&heap->data[idx], &heap->data[parent])) {
            Session tmp = heap->data[idx];
            heap->data[idx] = heap->data[parent];
            heap->data[parent] = tmp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void heapify_down(EventHeap *heap, int idx) {
    int size = heap->size;
    while (2 * idx + 1 < size) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = left;

        if (right < size && session_earlier(&heap->data[right], &heap->data[left])) {
            smallest = right;
        }

        if (session_earlier(&heap->data[smallest], &heap->data[idx])) {
            Session tmp = heap->data[idx];
            heap->data[idx] = heap->data[smallest];
            heap->data[smallest] = tmp;
            idx = smallest;
        } else {
            break;
        }
    }
}

void event_heap_push(EventHeap *heap, Session session) {
    if (heap->size >= heap->capacity) {
        heap->capacity *= 2;
        heap->data = (Session*)realloc(heap->data, heap->capacity * sizeof(Session));
    }
    heap->data[heap->size] = session;
    heapify_up(heap, heap->size);
    heap->size++;
}

static Session event_heap_pop(EventHeap *heap) {
    Session root = heap->data[0];
    heap->size--;
    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        heapify_down(heap, 0);
    }
    return root;
}

int event_heap_release_expired(EventHeap *heap, long long current_time, Server *servers, int server_count, int global_id_offset) {
    int released = 0;
    while (heap->size > 0 && heap->data[0].end_time <= current_time) {
        Session sess = event_heap_pop(heap);
        int local_idx = sess.server_id - global_id_offset;
        if (local_idx >= 0 && local_idx < server_count) {
            release_server_resources(&servers[local_idx], &sess);
            released++;
        }
    }
    return released;
}

void event_heap_drain_all(EventHeap *heap, Server *servers, int server_count, int global_id_offset) {
    while (heap->size > 0) {
        Session sess = event_heap_pop(heap);
        int local_idx = sess.server_id - global_id_offset;
        if (local_idx >= 0 && local_idx < server_count) {
            release_server_resources(&servers[local_idx], &sess);
        }
    }
}

bool verify_capacities_restored(const Server *current, const Server *initial, int count) {
    const double EPS = 1e-5;
    for (int i = 0; i < count; i++) {
        if (fabs(current[i].avail_cpu - initial[i].total_cpu) > EPS) return false;
        if (fabs(current[i].avail_gpu - initial[i].total_gpu) > EPS) return false;
        if (fabs(current[i].avail_ram - initial[i].total_ram) > EPS) return false;
        if (fabs(current[i].avail_bw  - initial[i].total_bw)  > EPS) return false;
    }
    return true;
}
