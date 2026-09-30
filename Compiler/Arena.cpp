//
//  Arena.cpp
//  Compiler
//
//  Created by Nathan Thurber on 2/7/24.
//

#include "Arena.hpp"

ArenaAllocator::ArenaAllocator(size_t bytes)
    : m_size(bytes) 
{
    m_buffer = static_cast<unsigned char*>(malloc(m_size));
    m_offset = m_buffer;
}

