#ifndef __MMVIEW_H
#define __MMVIEW_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

static inline long mmview_create(void) {
    long ret = syscall(1000);
    printf("mmview_create() -> %ld\n", ret);
    return ret;
}

static inline long mmview_delete(long id) {
    long ret = syscall(1003, id);
    printf("mmview_delete(%ld) -> %ld\n", id, ret);
    return ret;
}

static inline long mmview_migrate(long id) {
    long ret = syscall(1001, id);
    printf("mmview_migrate(%ld) -> %ld\n", id, ret);
    return ret;
}

static inline long mmview_current(void) {
    long ret = syscall(1001, -1);
    printf("mmview_current() -> %ld\n", ret);
    return ret;
}

static uintptr_t page_align_down(uintptr_t addr) {
    static unsigned pagesize = 0;
    if (pagesize == 0)
        pagesize = sysconf(_SC_PAGESIZE);

    uintptr_t page = addr - (addr % pagesize);
    return page;
}

static inline long mmview_unshare(void *start, size_t len) {
    uintptr_t start_page = page_align_down((uintptr_t) start);

    long ret = syscall(1002, start_page, len);
    printf("mmview_unshare(0x%lx, %ld) -> %ld\n", start_page, len, ret);
    return ret;
}

static inline long mmview_unshare_text(void) {
    extern char __executable_start;
    extern char __etext;
    uintptr_t start = (uintptr_t)&__executable_start;
    uintptr_t end = (uintptr_t)&__etext;
    return mmview_unshare((void *) start, end - start);
}

#endif
