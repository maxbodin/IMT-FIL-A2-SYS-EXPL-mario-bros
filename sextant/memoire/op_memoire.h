/* sextant/memoire/op_memoire.h — Opérations mémoire bas-niveau (pas de stdlib).
 *
 * Fourni à cpu_context.cpp pour le "poisonnage" des piles (détection de
 * variables non initialisées et de dépassement de pile).
 */
#ifndef _OP_MEMOIRE_H_
#define _OP_MEMOIRE_H_

static inline void* memset(void* dst, int c, unsigned int n) {
    unsigned char* p = (unsigned char*)dst;
    while (n--) *p++ = (unsigned char)c;
    return dst;
}

static inline void* memcpy(void* dst, const void* src, unsigned int n) {
    unsigned char* d = (unsigned char*)dst;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) *d++ = *s++;
    return dst;
}

#endif /* _OP_MEMOIRE_H_ */
