/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __CIRCBUFFER_H
#define __CIRCBUFFER_H

#include "platform/mbed_critical.h"
#include "platform/mbed_assert.h"

namespace internal {
/* Detect if CounterType of the Circular buffer is of unsigned type. */
template<typename T>
struct is_unsigned {
    static const bool value = false;
};
template<>
struct is_unsigned<unsigned char> {
    static const bool value = true;
};
template<>
struct is_unsigned<unsigned short> {
    static const bool value = true;
};
template<>
struct is_unsigned<unsigned int> {
    static const bool value = true;
};
template<>
struct is_unsigned<unsigned long> {
    static const bool value = true;
};
template<>
struct is_unsigned<unsigned long long> {
    static const bool value = true;
};
};

namespace {

// iterator handle
typedef struct {
    int iterator_num;
    int32_t buff_index;
} key_iterator_handle_t;

}
/** \addtogroup platform */
/** @{*/
/**
 * \defgroup platform_CircularBuffer CircularBuffer functions
 * @{
 */

/** Templated Circular buffer class
 *
 *  @note Synchronization level: Interrupt safe
 *  @note CounterType must be unsigned and consistent with BufferSize
 */
template<typename T, uint32_t BufferSize, typename CounterType = uint32_t>
class CircBuffer {
public:

    typedef struct _opaque_set_handle *set_handle_t;
    typedef struct _opaque_key_iterator *iterator_t;

    CircBuffer() : _head(0), _tail(0), _full(false)
    {
        // MBED_STATIC_ASSERT(
        //     internal::is_unsigned<CounterType>::value,
        //     "CounterType must be unsigned"
        // );

        // MBED_STATIC_ASSERT(
        //     (sizeof(CounterType) >= sizeof(uint32_t)) ||
        //     (BufferSize < (((uint64_t) 1) << (sizeof(CounterType) * 8))),
        //     "Invalid BufferSize for the CounterType"
        // );
    }

    ~CircBuffer()
    {
    }

    /** Push the transaction to the buffer. This overwrites the buffer if it's
     *  full
     *
     * @param data Data to be pushed to the buffer
     */
    void push(const T &data)
    {
        core_util_critical_section_enter();
        if (full()) {
            _tail++;
            if (_tail == BufferSize) {
                _tail = 0;
            }
        }
        _pool[_head++] = data;
        if (_head == BufferSize) {
                _head = 0;
        }
        if (_head == _tail) {
            _full = true;
        }
        core_util_critical_section_exit();
    }

    /** Pop the transaction from the buffer
     *
     * @param data Data to be popped from the buffer
     * @return True if the buffer is not empty and data contains a transaction, false otherwise
     */
    bool pop(T &data)
    {
        bool data_popped = false;
        core_util_critical_section_enter();
        if (!empty()) {
            data = _pool[_tail++];
            if (_tail == BufferSize) {
                _tail = 0;
            }
            _full = false;
            data_popped = true;
        }
        core_util_critical_section_exit();
        return data_popped;
    }

    /** Check if the buffer is empty
     *
     * @return True if the buffer is empty, false if not
     */
    bool empty() const
    {
        core_util_critical_section_enter();
        bool is_empty = (_head == _tail) && !_full;
        core_util_critical_section_exit();
        return is_empty;
    }

    /** Check if the buffer is full
     *
     * @return True if the buffer is full, false if not
     */
    bool full() const
    {
        core_util_critical_section_enter();
        bool full = _full;
        core_util_critical_section_exit();
        return full;
    }

    /** Reset the buffer
     *
     */
    void reset()
    {
        core_util_critical_section_enter();
        _head = 0;
        _tail = 0;
        _full = false;
        core_util_critical_section_exit();
    }

    /** Get the number of elements currently stored in the circular_buffer */
    CounterType size() const
    {
        core_util_critical_section_enter();
        CounterType elements;
        if (!_full) {
            if (_head < _tail) {
                elements = BufferSize + _head - _tail;
            } else {
                elements = _head - _tail;
            }
        } else {
            elements = BufferSize;
        }
        core_util_critical_section_exit();
        return elements;
    }

    /** Peek into circular buffer without popping
     *
     * @param data Data to be peeked from the buffer
     * @return True if the buffer is not empty and data contains a transaction, false otherwise
     */
    bool peek(T &data) const
    {
        bool data_updated = false;
        core_util_critical_section_enter();
        if (!empty()) {
            data = _pool[_tail];
            data_updated = true;
        }
        core_util_critical_section_exit();
        return data_updated;
    }

    int iterator_open(iterator_t *it)
    {
        key_iterator_handle_t *handle;
        int ret = MBED_SUCCESS;

        if (!it) {
            return MBED_ERROR_INVALID_ARGUMENT;
        }

        core_util_critical_section_enter();

        int it_num;
        for (it_num = 0; it_num < _max_open_iterators; it_num++) {
            if (!_iterator_table[it_num]) {
                break;
            }
        }

        if (it_num == _max_open_iterators) {
            ret = MBED_ERROR_OUT_OF_RESOURCES;
            goto end;
        }

        handle = new key_iterator_handle_t;
        *it = reinterpret_cast<iterator_t>(handle);

        handle->buff_index = _head; // Start at the head
        handle->iterator_num = it_num;
        _iterator_table[it_num] = handle;

    end:
        core_util_critical_section_exit();
        return ret;
    }

    /**
     *
     *
     * 
    */
    int iterator_next(iterator_t it, T& data)
    {
        key_iterator_handle_t *handle;
        int ret = MBED_SUCCESS;

        core_util_critical_section_enter();
    
        handle = reinterpret_cast<key_iterator_handle_t *>(it);

        if (empty()) {
            ret = MBED_ERROR_ITEM_NOT_FOUND;
            goto end;
        }

        // // Have we finished and backed up to the tail
        if (handle->buff_index == _tail && !full()) {
            ret = MBED_ERROR_ITEM_NOT_FOUND;
            goto end;
        }

        handle->buff_index--;
        if (handle->buff_index < 0) {
            handle->buff_index = BufferSize - 1;
        }
        data = _pool[handle->buff_index];
end:
        core_util_critical_section_exit();
        return ret;
    }

    /**
     * 
     * 
    */
    int iterator_close(iterator_t it)
    {
        key_iterator_handle_t *handle;

        core_util_critical_section_enter();

        handle = reinterpret_cast<key_iterator_handle_t *>(it);
        _iterator_table[handle->iterator_num] = 0;
        delete handle;

        core_util_critical_section_exit();

        return MBED_SUCCESS;
    }

private:
    T _pool[BufferSize];
    CounterType _head;
    CounterType _tail;
    bool _full;
    static const int _max_open_iterators = 16;
    void *_iterator_table[_max_open_iterators];
};

/**@}*/

/**@}*/

#endif
