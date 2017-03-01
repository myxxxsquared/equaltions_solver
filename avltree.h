#pragma once

//��Դ��http://www.cnblogs.com/hxf829/archive/2009/04/10/1659805.html

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
		//���ҵ�λ��
		AvlTreeNode<T>* tempPosition = this->root;
		AvlTreeNode<T>* pre = tempPosition;
		for (;;)
		{
			//����Ѿ�����Ҫ����ĵط���������½ڵ�
			if (tempPosition == NULL)
			{
				//ʵ�����½ڵ�
				AvlTreeNode<T>* newNode = new AvlTreeNode<T>(item);
				//�������λ��Ϊ�丸�ڵ����ڵ㣬����½ڵ�ҽӵ��丸�ڵ����ڵ���,����ҽӵ��丸�ڵ���ҽڵ���
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
				//����tempPositionǰһ�ڵ�
				pre = tempPosition;
				//��������Ѱ�Ҳ����
				tempPosition = tempPosition->left;
			}
			else if (item > tempPosition->data)
			{
				//����tempPositionǰһ�ڵ�
				pre = tempPosition;
				//��������Ѱ�Ҳ����
				tempPosition = tempPosition->right;
			}
			else
			{
				//�Ѵ��ڴ�Ԫ��
				return false;
			}
		}
		//������ʹ����ƽ��
		int bf = 0;
		while (tempPosition->parent)
		{
			//bf��ʾƽ�����ӵĸı��������½ڵ���뵽����������ƽ������+1
			//���½ڵ���뵽����������ƽ������-1
			bf = item < tempPosition->parent->data ? 1 : -1;

			tempPosition = tempPosition->parent;//��ָ��ָ�򸸽ڵ�

			tempPosition->balanceFactor += bf;//�ı丸�ڵ��ƽ������

			bf = tempPosition->balanceFactor;//��ȡ��ǰ�ڵ��ƽ������
			//�жϵ�ǰ�ڵ�ƽ�����ӣ����Ϊ��ʾ��������ƽ�⣬����Ҫ�ڻ���
			//
			if (bf == 0)
			{
				return true;
			}
			else if (bf == 2 || bf == -2)
			{
				//��������ʹ��ƽ��
				RotateSubTree(tempPosition);
				buildParent(root);
				root->parent = NULL;
				return true;
			}
		}
		return true;
	}
	//Ѱ��ֵΪitem��AvlTree�ڵ�
	template <class T>
	T* AvlTree<T>::Search(const T item)
	{
		AvlTreeNode<T>* current;
		current = this->root;
		for (;;)
		{
			//currentΪNULL��û�д˽ڵ�
			if (current == NULL)
				return NULL;
			//current��ΪNULL������Ѱ��
			if (item == current->data)
				return &current->data;
			else if (item < current->data)
				current = current->left;
			else if (item > current->data)
				current = current->right;
		}
	}

	//ɾ��һ���ڵ�
	template <class T>
	bool AvlTree<T>::Delete(const T item)
	{
		AvlTreeNode<T>* deleteNode = Search(item);
		//�����ɾ�ڵ㲻���ڣ�����
		if (deleteNode == NULL)
			return false;
		//pre�����µĳ�������tempPosition�ĸ��ڵ�
		AvlTreeNode<T>*  tempPosition = NULL;
		AvlTreeNode<T>* pre = tempPosition;
		//�洢����ɾ���Ľڵ�
		AvlTreeNode<T>* trueDeleteNode;
		//����ɾ����������������ʱ
		if (deleteNode->left != NULL && deleteNode->right != NULL)
		{
			//��ȡ������
			tempPosition = deleteNode->left;
			//��ȡdeleteNode���������ǰ����㣬�������tempPosition��
			while (tempPosition->right != NULL)
			{
				//�ҵ��������е������½��
				tempPosition = tempPosition->right;
			}
			//���������ǰ������ֵ���汻ɾ������ֵ
			deleteNode->data = tempPosition->data;

			if (tempPosition->parent == deleteNode)
			{
				//�����ɾԪ�ص�ǰ����������
				tempPosition->parent->left = tempPosition->left;
			}
			else
			{
				tempPosition->parent->right = tempPosition->left;
			}
			//�õ�����ɾ���Ľڵ�
			trueDeleteNode = tempPosition;
		}
		else //��ֻ������������������ΪҶ�ӽ��ʱ
		{
			//�����ҵ�Ωһ�ĺ��ӽ��
			pre = deleteNode->parent;
			tempPosition = deleteNode->left;
			if (tempPosition == NULL) //���ֻ���Һ��ӻ�û����
			{
				tempPosition = deleteNode->right;
			}

			if (deleteNode != root)
			{
				//���ɾ���ڵ㲻�Ǹ��ڵ�
				if (deleteNode->parent->left == deleteNode)
				{   //�����ɾ���������
					deleteNode->parent->left = tempPosition;
				}
				else
				{   //�����ɾ������Һ���
					deleteNode->parent->right = tempPosition;
				}
			}
			else
			{
				//��ɾ�����Ǹ����ʱ
				root = tempPosition;
			}
			//�õ�����ɾ���Ľڵ�
			trueDeleteNode = deleteNode;
		}
		//preΪ����ɾ���ڵ�ĸ��ڵ�
		pre = trueDeleteNode == NULL ? NULL : trueDeleteNode->parent;
		//ɾ����������ת������preָ��ʵ�ʱ�ɾ���Ľ��
		while (pre)
		{   //bf��ʾƽ�����ӵĸı�������ɾ�������������еĽ��ʱ��ƽ������-1
			//��ɾ�������������ĺ���ʱ��ƽ������+1
			int bf = (trueDeleteNode->data <= pre->data) ? -1 : 1;
			pre->balanceFactor += bf; //�ı䵱������ƽ������
			tempPosition = pre;
			pre = pre->parent;
			bf = tempPosition->balanceFactor; //��ȡ��ǰ����ƽ������
			if (bf != 0) //���bf==0�������߶Ƚ��ͣ��������ϻ���
			{
				//���bfΪ��-1��˵���߶�δ�䣬ֹͣ���ݣ����Ϊ��-2���������ת
				//����ת��߶Ȳ��䣬��ֹͣ����
				if (bf == 1 || bf == -1 || !RotateSubTree(tempPosition))
				{
					break;
				}
			}
		}
		buildParent(root);
		if (root != NULL)
			root->parent = NULL;
		delete trueDeleteNode;//��������ɾ���Ľڵ�
		size--;
		return true;
	}
	//��������
	template <class T>
	bool AvlTree<T>::RotateSubTree(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* current;
		current = tree;
		bool tallChange = true;
		int bf = tree->balanceFactor;
		AvlTreeNode<T>* newRoot = NULL;

		if (bf == 2) //��ƽ������Ϊʱ��Ҫ������ת����
		{
			int leftBF = current->left->balanceFactor;//�õ���������ƽ������
			if (leftBF == -1)
			{
				newRoot = LR(tree);//LR����ת(�������в����Һ��ӣ��������˫����ת)
			}
			else if (leftBF == 1)
			{
				newRoot = LL(tree); //LL����ת(�������в������ӣ��ҵ�ת)
			}
			else //����ת�����ӵ�bfΪʱ��ֻ��ɾ��ʱ�Ż����
			{
				newRoot = LL(tree);
				tallChange = false;
			}
		}
		if (bf == -2) //��ƽ������Ϊ-2ʱ��Ҫ������ת����
		{
			int rightBF = current->right->balanceFactor; //��ȡ��ת���Һ��ӵ�ƽ������
			if (rightBF == 1)
			{
				newRoot = RL(tree); //RL����ת(�������в������ӣ����Һ���˫����ת)
			}
			else if (rightBF == -1)
			{
				newRoot = RR(tree); //RR����ת(�������в����Һ��ӣ���ת)
			}
			else //����ת�����ӵ�bfΪʱ��ֻ��ɾ��ʱ�Ż����
			{
				newRoot = RR(tree);
				tallChange = false;
			}
		}
		//�����µ�������
		if (current->parent != NULL)
		{
			//�����ת��Ϊ����AVL���ĸ�

			//newRootΪ����ת��õ��ĵ�ǰ�����ĸ���һ���жϵ�������
			//���ԭ���ĸ����丸�ڵ�������������¸�Ҳ�ҽӵ��丸�ڵ����������
			//����ҽӵ�������
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
			//�����ת��ΪAVL���ĸ�����ָ����AVL�������
			this->root = newRoot;
		}
		return tallChange;
	}

	//LL����ת��������ת�����������
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
		else //treeNext->balanceFactor==0�������ɾ��ʱ��
		{
			tree->balanceFactor = 1;
			treeNext->balanceFactor = -1;
		}
		return treeNext; //treeNextΪ�������ĸ�
	}
	//LR����ת��������ת�����������
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::LR(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->left;
		AvlTreeNode<T>* newRoot = treeNext->right;
		tree->left = newRoot->right;
		treeNext->right = newRoot->left;
		newRoot->left = treeNext;
		newRoot->right = tree;

		switch (newRoot->balanceFactor) //�ı�ƽ������
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
		return newRoot; //newRootΪ�������ĸ�
	}
	//RR����ת��������ת�����������
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
		else //treeNext->balanceFactor==0�������ɾ��ʱ��
		{
			tree->balanceFactor = -1;
			treeNext->balanceFactor = 1;
		}
		return treeNext; //treeNextΪ�������ĸ�
	}
	//RL����ת��������ת�����������
	template <class T>
	AvlTreeNode<T>* AvlTree<T>::RL(AvlTreeNode<T>* tree)
	{
		AvlTreeNode<T>* treeNext = tree->right;
		AvlTreeNode<T>* newRoot = treeNext->left;
		tree->right = newRoot->left;
		treeNext->left = newRoot->right;
		newRoot->right = treeNext;
		newRoot->left = tree;

		switch (newRoot->balanceFactor) //�ı�ƽ������
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
		return newRoot; //newRootΪ�������ĸ�
	}
	//���½������нڵ�ĸ��ڵ�����
	template <class T>
	void AvlTree<T>::buildParent(AvlTreeNode<T>* tree)
	{
		//�����Ϊ����ֱ�ӷ��أ�������нڵ��parent��Ϊ��ǰtree�ڵ�
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
		//�ݹ�����ӽڵ㽨�����ڵ�����
		buildParent(tree->left);
		buildParent(tree->right);
	}
	//����ƽ����
	template <class T>
	void AvlTree<T>::Destory(AvlTreeNode<T>* tree)
	{
		//�����Ϊ��
		if (tree == NULL)
			return;
		//�������Ϊ��
		if (tree->left != NULL || tree->right != NULL)
		{
			//����������
			if (tree->left != NULL)
			{
				Destory(tree->left);
				tree->left = NULL;
			}
			//����������
			if (tree->right != NULL)
			{
				Destory(tree->right);
				tree->right = NULL;
			}
		}
		//������
		delete tree;
	}

	template <class T>
	AvlTree<T>::~AvlTree(void)
	{
		Destory(root);
	}

}