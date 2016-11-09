#ifndef __HL_HASHALG_INCLUDE_H__
#define __HL_HASHALG_INCLUDE_H__

template<class T>
size_t BKDRHash(const T *str)
{
    register size_t hash = 0;
    while (size_t ch = (size_t)*str++)
    {
        hash = hash * 131 + ch;        
    }
    return hash;
}

/// @brief SDBM Hash Function  
/// @detail ���㷨�������ڿ�Դ��ĿSDBM��һ�ּ򵥵����ݿ����棩�б�Ӧ�ö�����������BKDRHash˼��һ�£�ֻ�����Ӳ�ͬ���ѡ�  
template<class T>
size_t SDBMHash(const T *str)
{
    register size_t hash = 0;
    while (size_t ch = (size_t)*str++)
    {
        hash = 65599 * hash + ch;
        //hash = (size_t)ch + (hash << 6) + (hash << 16) - hash;  
    }
    return hash;
}

/// @brief RS Hash Function  
/// @detail ��Robert Sedgwicks���䡶Algorithms in C��һ����չʾ��������  
template<class T>
size_t RSHash(const T *str)
{
    register size_t hash = 0;
    size_t magic = 63689;
    while (size_t ch = (size_t)*str++)
    {
        hash = hash * magic + ch;
        magic *= 378551;
    }
    return hash;
}

/// @brief AP Hash Function  
/// @detail ��Arash Partow������һ��hash�㷨��  
template<class T>
size_t APHash(const T *str)
{
    register size_t hash = 0;
    size_t ch;
    for (long i = 0; ch = (size_t)*str++; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
        }
    }
    return hash;
}

/// @brief JS Hash Function  
/// ��Justin Sobel������һ��hash�㷨��  
template<class T>
size_t JSHash(const T *str)
{
    if (!*str)        
        return 0;
    register size_t hash = 1315423911;
    while (size_t ch = (size_t)*str++)
    {
        hash ^= ((hash << 5) + ch + (hash >> 2));
    }
    return hash;
}

/// @brief DEK Function  
/// @detail ���㷨������Donald E. Knuth�ڡ�Art Of Computer Programming Volume 3����չʾ��������  
template<class T>
size_t DEKHash(const T* str)
{
    if (!*str)      
        return 0;
    register size_t hash = 1315423911;
    while (size_t ch = (size_t)*str++)
    {
        hash = ((hash << 5) ^ (hash >> 27)) ^ ch;
    }
    return hash;
}

/// @brief FNV Hash Function  
/// @detail Unix systemϵͳ��ʹ�õ�һ������hash�㷨������΢��Ҳ����hash_map��ʵ�֡�  
template<class T>
size_t FNVHash(const T* str)
{
    if (!*str)   // �����ɱ�����ӣ��Ա�֤���ַ������ع�ϣֵ0  
        return 0;
    register size_t hash = 2166136261;
    while (size_t ch = (size_t)*str++)
    {
        hash *= 16777619;
        hash ^= ch;
    }
    return hash;
}

/// @brief DJB Hash Function  
/// @detail ��Daniel J. Bernstein���ڷ�����һ��hash�㷨��  
template<class T>
size_t DJBHash(const T *str)
{
    if (!*str)  return 0;

    register size_t hash = 5381;
    while (size_t ch = (size_t)*str++)
    {
        hash += (hash << 5) + ch;
    }
    return hash;
}

/// @brief DJB Hash Function 2  
/// @detail ��Daniel J. Bernstein ��������һ��hash�㷨��  
template<class T>
size_t DJB2Hash(const T *str)
{
    if (!*str) return 0;
    register size_t hash = 5381;
    while (size_t ch = (size_t)*str++)
    {
        hash = hash * 33 ^ ch;
    }
    return hash;
}

/// @brief PJW Hash Function  
/// @detail ���㷨�ǻ���AT&T����ʵ���ҵ�Peter J. Weinberger�����Ķ�������һ��hash�㷨��  
template<class T>
size_t PJWHash(const T *str)
{
    static const size_t TotalBits = sizeof(size_t) * 8;
    static const size_t ThreeQuarters = (TotalBits * 3) / 4;
    static const size_t OneEighth = TotalBits / 8;
    static const size_t HighBits = ((size_t)-1) << (TotalBits - OneEighth);

    register size_t hash = 0;
    size_t magic = 0;
    while (size_t ch = (size_t)*str++)
    {
        hash = (hash << OneEighth) + ch;
        if ((magic = hash & HighBits) != 0)
        {
            hash = ((hash ^ (magic >> ThreeQuarters)) & (~HighBits));
        }
    }
    return hash;
}

/// @brief ELF Hash Function  
/// @detail ������Unix��Extended Library Function��������������һ��hash�㷨������ʵ����PJW Hash�ı��Ρ�  
template<class T>
size_t ELFHash(const T *str)
{
    static const size_t TotalBits = sizeof(size_t) * 8;
    static const size_t ThreeQuarters = (TotalBits * 3) / 4;
    static const size_t OneEighth = TotalBits / 8;
    static const size_t HighBits = ((size_t)-1) << (TotalBits - OneEighth);
    register size_t hash = 0;
    size_t magic = 0;
    while (size_t ch = (size_t)*str++)
    {
        hash = (hash << OneEighth) + ch;
        if ((magic = hash & HighBits) != 0)
        {
            hash ^= (magic >> ThreeQuarters);
            hash &= ~magic;
        }
    }
    return hash;
}


#endif
