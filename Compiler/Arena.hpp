//
//  Arena.hpp
//  Compiler
//
//  Created by Nathan Thurber on 2/7/24.
//


#pragma once
#include <stdio.h>
#include <stdlib.h>

class ArenaAllocator
{
public:
    ArenaAllocator(size_t bytes);
    
    template<typename T>
    inline T* alloc()
    {
        void* offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T*>(offset);
    }
    
    inline ArenaAllocator(const ArenaAllocator& other) = delete;
    
    inline ArenaAllocator operator = (const ArenaAllocator& other) = delete;
    
    inline ~ArenaAllocator() { free(m_buffer); }
    
private:
    size_t m_size;
    unsigned char* m_buffer;
    unsigned char* m_offset;
};
