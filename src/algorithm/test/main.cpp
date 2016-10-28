#include <gtest/gtest.h>
#include "trie_tree.h"
#include "bsearch_tree.h"

using namespace LabSpace::algorithm;

int main(int _argc, char* _argv[])
{
#ifdef WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetBreakAlloc(1400);
#endif
    
    testing::InitGoogleTest(&_argc, _argv);
    return RUN_ALL_TESTS();
}