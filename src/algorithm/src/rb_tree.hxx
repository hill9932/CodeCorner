template<typename KEY_T, typename DATA_T>
CRBTree<KEY_T, DATA_T>::CRBTree()
{

}

template<typename KEY_T, typename DATA_T>
CRBTree<KEY_T, DATA_T>::~CRBTree()
{

}

template<typename KEY_T, typename DATA_T>
void CRBTree<KEY_T, DATA_T>::rotateLeft(TreeNodePtr _node)
{
    if (!_node) return;

    TreeNodePtr pivotNode = _node->right;
    if (_node->parent)
        _node->parent->left = pivotNode;

    pivotNode->parent = _node->parent;
    _node->right = pivotNode->left;
    pivotNode->left = _node;
}

template<typename KEY_T, typename DATA_T>
void CRBTree<KEY_T, DATA_T>::rotateRight(TreeNodePtr _node)
{
    if (!_node) return;

    TreeNodePtr pivotNode = _node->left;
    if (_node->parent)
        _node->parent->right = pivotNode;

    pivotNode->parent = _node->parent;
    _node->left = pivotNode->right;
    pivotNode->right = _node;
}

/**
 * RB Tree properties
 *  1. root is black
 *  2. leaf is black
 *  3. red node's children are black
 *  4. all the path from one node to the leaves have the same blacks
 **/
template<typename KEY_T, typename DATA_T>
void CRBTree<KEY_T, DATA_T>::adjust(TreeNodePtr _node)
{
    if (!_node) return;

    if (!_node->parent)  // this is the root node
        _node->color = TreeNode_t::NodeColor::BLACK;
    else if (_node->parent->color == TreeNode_t::NodeColor::BLACK)
        return;
    else if (_node->parent->color == TreeNode_t::NodeColor::RED)
    {
        _node->color = TreeNode_t::NodeColor::BLACK;

        //
        // case 1: uncle is red
        //
        TreeNodePtr uncleNode   = this->uncle(_node);
        TreeNodePtr grandNode   = this->grandparent(_node);
        TreeNodePtr parentNode  = _node->parent;

        if (uncleNode && 
            uncleNode->color == TreeNode_t::NodeColor::RED)
        {
            parentNode->color   = TreeNode_t::NodeColor::BLACK;
            uncleNode->color    = TreeNode_t::NodeColor::BLACK;

            if (grandNode)
            {
                grandNode->color = TreeNode_t::NodeColor::RED;
                adjust(grandNode);  
            }
        }
        else
        {
            //
            // case 2: uncle is black and parent is left child
            //
            if (_node == parentNode->right && 
                parentNode == grandNode->left) 
            {
                rotateLeft(parentNode);
                _node = _node->left;
            }
            //
            // case 3: uncle is black and parent is right child
            //
            else if (_node == parentNode->left && 
                parentNode == grandNode->right) 
            {
                rotateRight(parentNode);
                _node = _node->right;
            }

            //
            // case 4: 
            //
            parentNode->color   = TreeNode_t::NodeColor::BLACK;
            grandNode->color    = TreeNode_t::NodeColor::RED;
            if (_node == _node->parent->left && 
                _node->parent == this->grandparent(_node)->left)
            {
                rotateRight(this->grandparent(_node));
            }
            else 
            {
                rotateLeft(this->grandparent(_node));
            }
        }
    }
}
