#pragma once

//来源：http://www.cnblogs.com/hxf829/archive/2009/04/10/1659805.html

namespace mylib{

	template < class root, class value >
	class map_pair_val
	{
	public:
		root x;
		value y;

		map_pair_val() = default;
		map_pair_val(root X, value Y) :x(X), y(Y){}

		bool operator==(const map_pair_val& right) const{ return x == right.x; }
		bool operator!=(const map_pair_val& right) const{ return x != right.x; }
		bool operator<(const map_pair_val& right) const{ return x < right.x; }
		bool operator>(const map_pair_val& right) const{ return x > right.x; }
		bool operator<=(const map_pair_val& right) const{ return x <= right.x; }
		bool operator>=(const map_pair_val& right) const{ return x >= right.x; }
	};

	template <class T>
	class AvlTreeNode
	{
	public:
		AvlTreeNode<T> *left;
		AvlTreeNode<T> *right;
		AvlTreeNode<T> *parent;
		int balanceFactor;
		T data;
		AvlTreeNode(const T& item, AvlTreeNode<T>* lptr = NULL, AvlTreeNode<T>* rptr = NULL, AvlTreeNode<T>* par = NULL, int balfac = 0);
	};

	template <class T>
	class AvlTree
	{
	private:
		AvlTreeNode<T>* root;
		int size;

		AvlTreeNode<T>* LL(AvlTreeNode<T>* tree);
		AvlTreeNode<T>* LR(AvlTreeNode<T>* tree);
		AvlTreeNode<T>* RR(AvlTreeNode<T>* tree);
		AvlTreeNode<T>* RL(AvlTreeNode<T>* tree);
		void buildParent(AvlTreeNode<T>* tree);

		bool RotateSubTree(AvlTreeNode<T>* tree);

		void addtoarray(AvlTreeNode<T>* node, array_val<T> & a, int & i)
		{
			if (node->left)
				addtoarray(node->left, a, i);
			a[i++] = node->data;
			if (node->right)
				addtoarray(node->right, a, i);
		}

		static void Destory(AvlTreeNode<T>* tree);
		static AvlTreeNode<T>* Copy(AvlTreeNode<T>* tree, AvlTreeNode<T>* parent);

	public:
		AvlTree(void);
		AvlTree(const AvlTree<T> &);
		AvlTree(AvlTree<T> &&);
		AvlTree<T>& operator=(const AvlTree<T>&);
		AvlTree<T>& operator=(AvlTree<T>&&);
		T* Search(const T item);
		bool Insert(T item);
		bool Delete(T item);
		array_val<T> ToArray();
		~AvlTree(void);
		int Size(){ return size; }

		AvlTreeNode<T>* first()
		{
			AvlTreeNode<T>* result;
			while (result->left)
				result = result->left;
			return result;
		}

		AvlTreeNode<T>* next(AvlTreeNode<T>* pre)
		{
			if (pre->right)
				return pre->right;
			AvlTreeNode<T>* tmp;
			while (pre->parent)
			{
				if (pre->parent->left == pre)
					return pre->parent;
				pre = pre->parent;
			}
			return nullptr;
		}
	};

	template <class T>
	AvlTree<T>::AvlTree(const AvlTree<T> & right)
	{
		this->root = Copy(right.root, nullptr);
		this->size = right.size;
	}

	template <class T>
	AvlTree<T>::AvlTree(AvlTree<T> && right)
	{
		this->root = right.root;
		this->size = right.size;
		right.root = nullptr;
	}

	template <class T>
	AvlTree<T>& AvlTree<T>::operator=(const AvlTree<T>& right)
	{
		Destory(this->root);

		this->root = Copy(right.root, nullptr);
		this->size = right.size;
	}

	template <class T>
	AvlTree<T>& AvlTree<T>::operator=(AvlTree<T>&&)
	{
		Destory(this->root);

		this->root = right.root;
		this->size = right.size;
		right.root = nullptr;
	}

	template <class T>
	AvlTreeNode<T>* AvlTree<T>::Copy(AvlTreeNode<T>* tree, AvlTreeNode<T>* parent)
	{
		if (tree == nullptr)
			return nullptr;

		AvlTreeNode<T>* result = new AvlTreeNode<T>(tree->data, nullptr, nullptr, parent, tree->balanceFactor);
		result->left = Copy(tree->left, result);
		result->right = Copy(tree->right, result);

		return result;
	}

	template <class T>
	array_val<T> AvlTree<T>::ToArray()
	{
		array_val<T> result(size);
		int i = 0;
		addtoarray(root, result, i);
		return result;
	}

	template <class T>
	AvlTreeNode<T>::AvlTreeNode(const T &item, AvlTreeNode<T> *lptr = NULL, AvlTreeNode<T> *rptr = NULL, AvlTreeNode<T>* pr = NULL, int balfac = 0) :data(item), left(lptr), right(rptr), parent(pr), balanceFactor(balfac)
	{ }
	template <class T>
	AvlTree<T>::AvlTree() : root(NULL), size(0)
	{ }

	template <class T>
	bool AvlTree<T>::Insert(const T item)
	{
		//先找到位置
		AvlTreeNode<T>* tempPosition = this->root;
		AvlTreeNode<T>* pre = tempPosition;
		for (;;)
		{
			//如果已经到达要插入的地方，则插入新节点
			if (tempPosition == NULL)
			{
				//实例化新节点
				AvlTreeNode<T>* newNode = new AvlTreeNode<T>(item);
				//如果插入位置为其父节点的左节点，则把新节点挂接到其父节点的左节点上,否则挂接到其父节点的右节点上
				if (tempPosition == this->root)
				{
					root = newNode;
					size++;
					return true;
				}
				else if (tempPosition == pre->left && item < pre->data)
				{
					newNode->parent = pre;
					pre->left = tempPosition = newNode;
					size++;
					break;
				}
				else
				{
					newNode->parent = pre;
					pre->right = tempPosition = newNode;
					size++;
					break;
				}
			}

			else if (item < tempPosition->data)
			{
				//保存tempPosition前一节点
				pre = tempPosition;
				//在左子数寻找插入点
				tempPosition = tempPosition->left;
			}
			else if (item > tempPosition->data)
			{
				//保存tempPosition前一节点
				pre = tempPosition;
				//在右子数寻找插入点
				tempPosition = tempPosition->right;
			}
			else
			{
				//已存在此元素
				return false;
			}
		}
		//调整以使得树平衡
		int bf = 0;
		while (tempPosition->parent)
		{
			//bf表示平衡因子的改变量，当新节点插入到左子树，则平衡因子+1
			//当新节点插入到右子树，则平衡因子-1
			bf = item < tempPosition->parent->data ? 1 : -1;

			tempPosition = tempPosition->parent;//将指针指向父节点

			tempPosition->balanceFactor += bf;//改变父节点的平衡因子

			bf = tempPosition->balanceFactor;//获取当前节点的平衡因子
			//判断当前节点平衡因子，如果为表示改子树已平衡，不需要在回溯
			//
			if (bf == 0)
			{
				return true;
			}
			else if (bf == 2 || bf == -2)
			{
				//调整树以使其平衡
				RotateSubTree(tempPosition);
				buildParent(root);
				root->parent = NULL;
				return true;
			}
		}
		return true;
	}
	//寻找值为item的AvlTree节点
	template <class T>
	T* AvlTree<T>::Search(const T item)
	{
		AvlTreeNode<T>* current;
		current = this->root;
		for (;;)
		{
			//current为NULL，没有此节点
			if (current == NULL)
				return NULL;
			//current不为NULL，继续寻找
			if (item == current->data)
				return &current->data;
			else if (item < current->data)
				current = current->left;
			else if (item > current->data)
				current = current->right;
		}
	}

	//删除一个节点
	template <class T>
	bool AvlTree<T>::Delete(const T item)
	{
		AvlTreeNode<T>* deleteNode = Search(item);
		//如果所删节点不存在，返回
		if (deleteNode == NULL)
			return false;
		//pre在以下的程序中是tempPosition的父节点
		AvlTreeNode<T>*  tempPosition = NULL;
		AvlTreeNode<T>* pre = tempPosition;
		//存储真正删除的节点
		AvlTreeNode<T>* trueDeleteNode;
		//当被删除结点存在左右子树时
		if (deleteNode->left != NULL && deleteNode->right != NULL)
		{
			//获取左子树
			tempPosition = deleteNode->left;
			//获取deleteNode的中序遍历前驱结点，并存放于tempPosition中
			while (tempPosition->right != NULL)
			{
				//找到左子树中的最右下结点
				tempPosition = tempPosition->right;
			}
			//用中序遍历前驱结点的值代替被删除结点的值
			deleteNode->data = tempPosition->data;

			if (tempPosition->parent == deleteNode)
			{
				//如果被删元素的前驱是其左孩子
				tempPosition->parent->left = tempPosition->left;
			}
			else
			{
				tempPosition->parent->right = tempPosition->left;
			}
			//得到真正删除的节点
			trueDeleteNode = tempPosition;
		}
		else //当只有左子树或右子树或为叶子结点时
		{
			//首先找到惟一的孩子结点
			pre = deleteNode->parent;
			tempPosition = deleteNode->left;
			if (tempPosition == NULL) //如果只有右孩子或没孩子
			{
				tempPosition = deleteNode->right;
			}

			if (deleteNode != root)
			{
				//如果删除节点不是根节点
				if (deleteNode->parent->left == deleteNode)
				{   //如果被删结点是左孩子
					deleteNode->parent->left = tempPosition;
				}
				else
				{   //如果被删结点是右孩子
					deleteNode->parent->right = tempPosition;
				}
			}
			else
			{
				//当删除的是根结点时
				root = tempPosition;
			}
			//得到真正删除的节点
			trueDeleteNode = deleteNode;
		}
		//pre为真正删除节点的父节点
		pre = trueDeleteNode == NULL ? NULL : trueDeleteNode->parent;
		//删除完后进行旋转，现在pre指向实际被删除的结点
		while (pre)
		{   //bf表示平衡因子的改变量，当删除的是左子树中的结点时，平衡因子-1
			//当删除的是右子树的孩子时，平衡因子+1
			int bf = (trueDeleteNode->data <= pre->data) ? -1 : 1;
			pre->balanceFactor += bf; //改变当父结点的平衡因子
			tempPosition = pre;
			pre = pre->parent;
			bf = tempPosition->balanceFactor; //获取当前结点的平衡因子
			if (bf != 0) //如果bf==0，表明高度降低，继续后上回溯
			{
				//如果bf为或-1则说明高度未变，停止回溯，如果为或-2，则进行旋转
				//当旋转后高度不变，则停止回溯
				if (bf == 1 || bf == -1 || !RotateSubTree(tempPosition))
				{
					break;
				}
			}
		}
		buildParent(root);
		if (root != NULL)
			root->parent = NULL;
		delete trueDeleteNode;//析构真正删除的节点
		size--;
		return true;
	}
	//调整函数
	template <class T>
	bool AvlTree<T>::RotateSubTree(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* current;
		current = tree;
		bool tallChange = true;
		int bf = tree->balanceFactor;
		AvlTreeNode<T>* newRoot = NULL;

		if (bf == 2) //当平衡因子为时需要进行旋转操作
		{
			int leftBF = current->left->balanceFactor;//得到左子树的平衡因子
			if (leftBF == -1)
			{
				newRoot = LR(tree);//LR型旋转(左子树中插入右孩子，先左后右双向旋转)
			}
			else if (leftBF == 1)
			{
				newRoot = LL(tree); //LL型旋转(左子树中插入左孩子，右单转)
			}
			else //当旋转根左孩子的bf为时，只有删除时才会出现
			{
				newRoot = LL(tree);
				tallChange = false;
			}
		}
		if (bf == -2) //当平衡因子为-2时需要进行旋转操作
		{
			int rightBF = current->right->balanceFactor; //获取旋转根右孩子的平衡因子
			if (rightBF == 1)
			{
				newRoot = RL(tree); //RL型旋转(右子树中插入左孩子，先右后左双向旋转)
			}
			else if (rightBF == -1)
			{
				newRoot = RR(tree); //RR型旋转(右子树中插入右孩子，左单转)
			}
			else //当旋转根左孩子的bf为时，只有删除时才会出现
			{
				newRoot = RR(tree);
				tallChange = false;
			}
		}
		//更改新的子树根
		if (current->parent != NULL)
		{
			//如果旋转根为不是AVL树的根

			//newRoot为新旋转后得到的当前子树的根，一下判断的作用是
			//如果原来的根是其父节点的左子树，则新根也挂接到其父节点的左子树，
			//否则挂接到右子树
			if (tree->data < (tree->parent)->data)
			{
				current->parent->left = newRoot;
			}
			else
			{
				current->parent->right = newRoot;
			}
		}
		else
		{
			//如果旋转根为AVL树的根，则指定新AVL树根结点
			this->root = newRoot;
		}
		return tallChange;
	}

	//LL型旋转，返回旋转后的新子树根
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::LL(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->left;
		tree->left = treeNext->right;
		treeNext->right = tree;

		if (treeNext->balanceFactor == 1)
		{
			tree->balanceFactor = 0;
			treeNext->balanceFactor = 0;
		}
		else //treeNext->balanceFactor==0的情况，删除时用
		{
			tree->balanceFactor = 1;
			treeNext->balanceFactor = -1;
		}
		return treeNext; //treeNext为新子树的根
	}
	//LR型旋转，返回旋转后的新子树根
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::LR(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->left;
		AvlTreeNode<T>* newRoot = treeNext->right;
		tree->left = newRoot->right;
		treeNext->right = newRoot->left;
		newRoot->left = treeNext;
		newRoot->right = tree;

		switch (newRoot->balanceFactor) //改变平衡因子
		{
		case 0:
			tree->balanceFactor = 0;
			treeNext->balanceFactor = 0;
			break;
		case 1:
			tree->balanceFactor = -1;
			treeNext->balanceFactor = 0;
			break;
		case -1:
			tree->balanceFactor = 0;
			treeNext->balanceFactor = 1;
			break;
		}
		newRoot->balanceFactor = 0;
		return newRoot; //newRoot为新子树的根
	}
	//RR型旋转，返回旋转后的新子树根
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::RR(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->right;
		tree->right = treeNext->left;
		treeNext->left = tree;

		if (treeNext->balanceFactor == -1)
		{
			tree->balanceFactor = 0;
			treeNext->balanceFactor = 0;
		}
		else //treeNext->balanceFactor==0的情况，删除时用
		{
			tree->balanceFactor = -1;
			treeNext->balanceFactor = 1;
		}
		return treeNext; //treeNext为新子树的根
	}
	//RL型旋转，返回旋转后的新子树根
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::RL(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->right;
		AvlTreeNode<T>* newRoot = treeNext->left;
		tree->right = newRoot->left;
		treeNext->left = newRoot->right;
		newRoot->right = treeNext;
		newRoot->left = tree;

		switch (newRoot->balanceFactor) //改变平衡因子
		{
		case 0:
			tree->balanceFactor = 0;
			treeNext->balanceFactor = 0;
			break;
		case 1:
			tree->balanceFactor = 0;
			treeNext->balanceFactor = -1;
			break;
		case -1:
			tree->balanceFactor = 1;
			treeNext->balanceFactor = 0;
			break;
		}
		newRoot->balanceFactor = 0;
		return newRoot; //newRoot为新子树的根
	}
	//重新建立所有节点的父节点连接
	template <class T>
	void AvlTree<T>::buildParent(AvlTreeNode<T>* tree)
	{
		//如果树为空则直接返回，如果树有节点的parent置为当前tree节点
		if (tree == NULL)
		{
			return;
		}
		if (tree->left != NULL)
		{
			tree->left->parent = tree;
		}
		if (tree->right != NULL)
		{
			tree->right->parent = tree;
		}
		//递归遍历子节点建立父节点连接
		buildParent(tree->left);
		buildParent(tree->right);
	}
	//销毁平衡树
	template <class T>
	void AvlTree<T>::Destory(AvlTreeNode<T>* tree)
	{
		//如果树为空
		if (tree == NULL)
			return;
		//如果树不为空
		if (tree->left != NULL || tree->right != NULL)
		{
			//析构左子树
			if (tree->left != NULL)
			{
				Destory(tree->left);
				tree->left = NULL;
			}
			//析构右子树
			if (tree->right != NULL)
			{
				Destory(tree->right);
				tree->right = NULL;
			}
		}
		//析构树
		delete tree;
	}

	template <class T>
	AvlTree<T>::~AvlTree(void)
	{
		Destory(root);
	}

}