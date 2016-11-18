#pragma once

#include "bsearch_tree.h"


namespace LabSpace
{
    namespace algorithm
    {
        template<typename KEY_T, typename DATA_T>
        class CRBTree : public CBSTree<KEY_T, DATA_T>
        {
        public:
            CRBTree();
            ~CRBTree();

            using CBSTree<KEY_T, DATA_T>::TreeNodePtr;

        private:
            void rotateLeft(TreeNodePtr _node);
            void rotateRight(TreeNodePtr _node);

            virtual void adjust(TreeNodePtr _node) override;
        };

        #include "rb_tree.hxx"
    }
}
