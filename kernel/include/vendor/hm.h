#pragma once

#include <stddef.h>
#include "mm/pmm.h"

#define MALLOC(size) malloc(size)
#define CALLOC(nmemb, size) calloc(nmemb, size)
#define FREE(ptr) free(ptr)
#define PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)

#define DEFAULT_HM_CAPAPCITY 16

#define bucket(ktype, vtype, name) typedef struct name { size_t hash; ktype key; vtype val; struct name *next; } name
#define hm(_Bucket_T, _Name) typedef struct _Name { _Bucket_T **buckets; size_t cap, len; } _Name 

#define hm_init(_HM, _Bucket_T)                                                           \
    do                                                                                    \
    {                                                                                     \
        (_HM)->cap = DEFAULT_HM_CAPAPCITY;                                                \
        (_HM)->len = 0;                                                                   \
        (_HM)->buckets = (_Bucket_T **)CALLOC(DEFAULT_HM_CAPAPCITY, sizeof(_Bucket_T *)); \
    } while (0);

#define hm_create_bucket(_K, _V, _KH, _Bucket_T)       \
    ({                                                 \
        _Bucket_T *bucket = MALLOC(sizeof(_Bucket_T)); \
        if (!bucket)                                   \
            NULL;                                      \
        bucket->key = (_K);                            \
        bucket->val = (_V);                            \
        bucket->hash = (_KH);                          \
        bucket->next = NULL;                           \
        bucket;                                        \
    })

#define hm_remove(_HM, _K, _Hash_FN, _Cmp_FN, _Bucket_T) \
    do                                                   \
    {                                                    \
        size_t hash = _Hash_FN(_K) % (_HM)->cap;         \
        _Bucket_T *curr = (_HM)->buckets[hash];          \
        _Bucket_T *prev = NULL;                          \
        while (curr)                                     \
        {                                                \
            if (_Cmp_FN(curr->key, _K) == 0)             \
            {                                            \
                if (prev == NULL)                        \
                    (_HM)->buckets[hash] = curr->next;   \
                else                                     \
                    prev->next = curr->next;             \
                FREE(curr);                              \
                (_HM)->len--;                            \
                break;                                   \
            }                                            \
            prev = curr;                                 \
            curr = curr->next;                           \
        }                                                \
    } while (0);

#define hm_get(_HM, _K, _Hash_FN, _Cmp_FN, _Bucket_T)     \
    ({                                                    \
        size_t hash = _Hash_FN(_K) % (_HM)->cap;          \
        _Bucket_T *bucket = (_HM)->buckets[hash];         \
        while (bucket && (_Cmp_FN(bucket->key, _K) != 0)) \
            bucket = bucket->next;                        \
        bucket ? &bucket->val : NULL;                     \
    })

#define hm_free(_HM, _Bucket_T)                 \
    do                                          \
    {                                           \
        _Bucket_T *bucket = NULL;               \
        for (size_t i = 0; i < (_HM)->cap; i++) \
        {                                       \
            bucket = *((_HM)->buckets + i);     \
            while (bucket)                      \
            {                                   \
                _Bucket_T *next = bucket->next; \
                FREE(bucket);                   \
                bucket = next;                  \
            }                                   \
        }                                       \
        FREE((_HM)->buckets);                   \
        (_HM)->buckets = NULL;                  \
        (_HM)->cap = 0;                         \
        (_HM)->len = 0;                         \
    } while (0);

#define buckets_insert(_HM, _Bucket, _IDX, _Bucket_T) \
    do                                                \
    {                                                 \
        (_Bucket)->next = (_HM)->buckets[_IDX];       \
        (_HM)->len++;                                 \
        (_HM)->buckets[_IDX] = (_Bucket);             \
    } while (0);

#define hm_resize(_HM, _Bucket_T)                                                  \
    do                                                                             \
    {                                                                              \
        size_t old_cap = (_HM)->cap;                                               \
        _Bucket_T **old_buckets = (_HM)->buckets;                                  \
        (_HM)->cap *= 2;                                                           \
        (_HM)->len = 0;                                                            \
        (_HM)->buckets = CALLOC((_HM)->cap, sizeof(_Bucket_T *));                  \
        for (size_t i = 0; i < old_cap; i++)                                       \
        {                                                                          \
            _Bucket_T *bucket = old_buckets[i];                                    \
            while (bucket)                                                         \
            {                                                                      \
                _Bucket_T *next = bucket->next;                                    \
                buckets_insert(_HM, bucket, bucket->hash % (_HM)->cap, _Bucket_T); \
                bucket = next;                                                     \
            }                                                                      \
        }                                                                          \
        FREE(old_buckets);                                                         \
    } while (0);

#define hm_insert(_HM, _K, _V, _Hash_FN, _Bucket_T)                            \
    do                                                                         \
    {                                                                          \
        if ((float)(_HM)->len / (_HM)->cap >= 0.75f)                           \
            hm_resize(_HM, _Bucket_T);                                         \
        size_t key_hash = _Hash_FN(_K);                                        \
        size_t hash = key_hash % (_HM)->cap;                                   \
        _Bucket_T *bucket = hm_create_bucket((_K), (_V), key_hash, _Bucket_T); \
        buckets_insert(_HM, bucket, hash, _Bucket_T);                          \
    } while (0)

#define bucket_print(_Key_Fmt_Spec, _Val_Fmt_Spec, _Bucket, _Bucket_T)               \
    do                                                                               \
    {                                                                                \
        _Bucket_T *bucket = _Bucket;                                                 \
        while (bucket)                                                               \
        {                                                                            \
            PRINTF(_Key_Fmt_Spec ": " _Val_Fmt_Spec "\n", bucket->key, bucket->val); \
            bucket = bucket->next;                                                   \
        }                                                                            \
    } while (0)

#define hm_print(_Key_Fmt_Spec, _Val_Fmt_Spec, _HM, _Bucket_T)                 \
    do                                                                         \
    {                                                                          \
        _Bucket_T **buckets = (_HM)->buckets;                                  \
        for (size_t i = 0; i < (_HM)->cap; i++)                                \
        {                                                                      \
            bucket_print(_Key_Fmt_Spec, _Val_Fmt_Spec, buckets[i], _Bucket_T); \
        }                                                                      \
    } while (0);
