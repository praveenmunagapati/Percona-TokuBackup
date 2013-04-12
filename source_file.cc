/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: ft=cpp:expandtab:ts=8:sw=4:softtabstop=4:
#ident "Copyright (c) 2012-2013 Tokutek Inc.  All rights reserved."
#ident "$Id: file_descriptor_map.h 54931 2013-03-29 19:51:23Z bkuszmaul $"

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "source_file.h"

////////////////////////////////////////////////////////
//
source_file::source_file(const char * const path) : m_full_path(strdup(path)), m_next(NULL), m_reference_count(0) 
{
};

source_file::~source_file(void) {
    free(m_full_path);
}

int source_file::init(void)
{
    int r = pthread_mutex_init(&m_range_mutex, NULL);
    if (r == 0) {
        r = pthread_rwlock_init(&m_name_rwlock, NULL);
    }

    return r;
}

////////////////////////////////////////////////////////
//
const char * source_file::name(void)
{
    return m_full_path;
}

////////////////////////////////////////////////////////
//
source_file * source_file::next(void)
{
    return m_next;
}

////////////////////////////////////////////////////////
//
void source_file::set_next(source_file *next)
{
    m_next = next;
}

////////////////////////////////////////////////////////
//
void source_file::lock_range(uint64_t lo __attribute__((unused)), uint64_t hi __attribute__((unused)))
// For now the lo and hi are unused.  We'll just grab the m_mutex.
{
    pthread_mutex_lock(&m_range_mutex);
}


////////////////////////////////////////////////////////
//
void source_file::unlock_range(uint64_t lo __attribute__((unused)), uint64_t hi __attribute__((unused)))
// For now the lo and hi are unused.  We'll just release the m_mutex.
{
    pthread_mutex_unlock(&m_range_mutex);
}

////////////////////////////////////////////////////////
//
int source_file::name_write_lock(void)
{
    return pthread_rwlock_wrlock(&m_name_rwlock);
}

////////////////////////////////////////////////////////
//
int source_file::name_read_lock(void)
{
    return pthread_rwlock_rdlock(&m_name_rwlock);
}

////////////////////////////////////////////////////////
//
int source_file::name_unlock(void)
{
    return pthread_rwlock_unlock(&m_name_rwlock);
}

////////////////////////////////////////////////////////
//
int source_file::rename(const char * new_name)
{
    int r = 0;
    free(m_full_path);
    m_full_path = realpath(new_name, NULL);
    if (m_full_path == NULL) {
        r = -1;
    }

    return r;
}

////////////////////////////////////////////////////////
//
void source_file::add_reference(void)
{
    ++m_reference_count;
}

////////////////////////////////////////////////////////
//
void source_file::remove_reference(void)
{
    if (m_reference_count != 0) {
        --m_reference_count;
    }
}

////////////////////////////////////////////////////////
//
unsigned int source_file::get_reference_count(void)
{
    return m_reference_count;
}
