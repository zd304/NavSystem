#ifndef __QUAD_TREE_NODE_H__
#define __QUAD_TREE_NODE_H__

/*
//四叉树节点类，用头节点代表四叉树;
//坐标系坐上角为原点，左往右为x轴递增，上往下y轴递增;
//本四叉树的策略是：1，插入时动态分配节点和删除节点，不是满树；
//					2，当矩形区域完全包含某个节点时才获取或剔除；
//					3，对象放在完全包含它的区域节点内，非根节点也存储对象;
*/
#pragma once
#include <list>

// 四叉树类型枚举;
enum QuadType
{
	ROOT,			// 根;
	UP_RIGHT,		//象限Ⅰ;
	UP_LEFT,		//象限Ⅱ;
	BOTTOM_LEFT,	//象限Ⅲ;
	BOTTOM_RIGHT,	//象限Ⅳ;
};

// 类型T必须有成员：{ x, y, width, height }
template <typename T>
class QuadTreeNode
{
public:
	QuadTreeNode(float _x, float _y, float _width, float _height, int _level, int _maxLevel, QuadType _quadType, QuadTreeNode* _parent);
	~QuadTreeNode();
public:
	void InsertObject(T* object); // 插入对象;
	std::list<T*> GetObjectsAt(float px, float py, float w, float h); // 查询对象，获得一片区域里的对象链表，完全包围;
	std::list<T*> GetObjectsAt(float px, float py);
	void RemoveObjectsAt(float px, float py, float w, float h); // 删除对象，删除一片区域里的对象和节点，完全包围;

	void GetRect(float* x, float* y, float* width, float* height);
	void GetMaxLevel(int* maxLevel);
private:
	bool IsContain(float px, float py, float w, float h, T* object) const; // 判断某个区域是否包含某对象;
	bool IsContain(float px, float py, float w, float h, QuadTreeNode<T>* quadTreeNode) const; // 判断某个区域是否包含某对象;
	bool IsContainPoint(QuadTreeNode<T>* quadTreeNode, float px, float py) const;
private:
	std::list<T*> objects;

	// 父、子节点，分四象限;
	QuadTreeNode* parent;
	QuadTreeNode* upRightNode;
	QuadTreeNode* upLeftNode;
	QuadTreeNode* bottomLeftNode;
	QuadTreeNode* bottomRightNode;
	// 节点类型;
	QuadType quadType;
	// 坐标和长宽属性，左上角为锚点;
	float x;
	float y;
	float width;
	float height;

	int level;		// 当前深度;
	int maxLevel;	// 最大深度;
};

template <typename T>
QuadTreeNode<T>::QuadTreeNode(float _x, float _y, float _width, float _height,
	int _level, int _maxLevel, QuadType _quadType, QuadTreeNode *_parent) :
	quadType(_quadType),
	x(_x),
	y(_y),
	width(_width),
	height(_height),
	level(_level),
	maxLevel(_maxLevel)
{
	parent = _parent;
	upRightNode = NULL;
	upLeftNode = NULL;
	bottomLeftNode = NULL;
	bottomRightNode = NULL;
}


template <typename T>
QuadTreeNode<T>::~QuadTreeNode()
{
	// 如果不是叶子节点，就销毁子节点;
	if (upRightNode)
	{
		delete upRightNode;
		upRightNode = NULL;
	}
	if (upLeftNode)
	{
		delete upLeftNode;
		upLeftNode = NULL;
	}
	if (bottomLeftNode)
	{
		delete bottomLeftNode;
		bottomLeftNode = NULL;
	}
	if (bottomRightNode)
	{
		delete bottomRightNode;
		bottomRightNode = NULL;
	}

	parent = NULL;
}

template <typename T>
bool QuadTreeNode<T>::IsContain(float px, float py, float w, float h, T *object) const
{
	if (object->x >= px
		&&object->x + object->width <= px + w
		&&object->y >= py
		&&object->y + object->height <= py + h)
		return true;
	return false;
}


template <typename T>
bool QuadTreeNode<T>::IsContain(float px, float py, float w, float h, QuadTreeNode<T> *quadTreeNode) const
{
	if (quadTreeNode->x >= px
		&&quadTreeNode->x + quadTreeNode->width <= px + w
		&&quadTreeNode->y >= py
		&&quadTreeNode->y + quadTreeNode->height <= py + h)
		return true;
	return false;
}

template <typename T>
bool QuadTreeNode<T>::IsContainPoint(QuadTreeNode<T>* quadTreeNode, float px, float py) const
{
	if (quadTreeNode->x <= px
		&& quadTreeNode->y <= py
		&& quadTreeNode->x + quadTreeNode->width >= px
		&& quadTreeNode->y + quadTreeNode->height >= py)
	{
		return true;
	}
	return false;
}

template <typename T>
void QuadTreeNode<T>::InsertObject(T* object)
{
	// 如果是叶子节点，则存在叶子节点;
	if (level == maxLevel)
	{
		objects.push_back(object);
		return;
	}

	// 非叶子节点，如果下层节点可以包含该对象，则递归构建子节点并插入对象,边构建边插入;
	if (IsContain(x + width / 2, y, width / 2, height / 2, object))
	{
		if (!upRightNode) // 避免重复创建覆盖掉原来的节点;
			upRightNode = new QuadTreeNode(x + width / 2, y, width / 2, height / 2, level + 1, maxLevel, UP_RIGHT, this);//如果没有子节点就创建子节点，parent节点是当前节点;
		upRightNode->InsertObject(object);
		return;
	}
	else if (IsContain(x, y, width / 2, height / 2, object))
	{
		if (!upLeftNode)
			upLeftNode = new QuadTreeNode(x, y, width / 2, height / 2, level + 1, maxLevel, UP_LEFT, this);
		upLeftNode->InsertObject(object);
		return;
	}
	else if (IsContain(x, y + height / 2, width / 2, height / 2, object))
	{
		if (!bottomLeftNode)
			bottomLeftNode = new QuadTreeNode(x, y + height / 2, width / 2, height / 2, level + 1, maxLevel, BOTTOM_LEFT, this);
		bottomLeftNode->InsertObject(object);
		return;
	}
	else if (IsContain(x + width / 2, y + height / 2, width / 2, height / 2, object))
	{
		if (!bottomRightNode)
			bottomRightNode = new QuadTreeNode(x + width / 2, y + height / 2, width / 2, height / 2, level + 1, maxLevel, BOTTOM_RIGHT, this);
		bottomRightNode->InsertObject(object);
		return;
	}
	// 下层节点不能完全包含该对象，则插入到当前非叶子节点;
	// 这个判断也可以省去;
	if (IsContain(x, y, width, height, object))
		objects.push_back(object);
}

template <typename T>
std::list<T*> QuadTreeNode<T>::GetObjectsAt(float px, float py, float w, float h)
{
	std::list<T*> resObjects;
	// 如果当前节点完全被包含，把当前节点存的对象放到列表末尾,空链表也行;
	if (IsContain(px, py, w, h, this))
	{
		resObjects.insert(resObjects.end(), objects.begin(), objects.end());
		// 最后一层;
		if (level == maxLevel)
			return resObjects;
	}

	// 如果有下层节点就把下层节点包含的对象加进来;
	if (upRightNode)
	{
		std::list<T*> upRightChild;
		upRightChild = upRightNode->GetObjectsAt(px, py, w, h);
		resObjects.insert(resObjects.end(), upRightChild.begin(), upRightChild.end());
	}
	if (upLeftNode)
	{
		std::list<T*> upLeftChild;
		upLeftChild = upLeftNode->GetObjectsAt(px, py, w, h);
		resObjects.insert(resObjects.end(), upLeftChild.begin(), upLeftChild.end());
	}
	if (bottomLeftNode)
	{
		std::list<T*> bottomLeftChild;
		bottomLeftChild = bottomLeftNode->GetObjectsAt(px, py, w, h);
		resObjects.insert(resObjects.end(), bottomLeftChild.begin(), bottomLeftChild.end());
	}
	if (bottomRightNode)
	{
		std::list<T*> bottomRightChild;
		bottomRightChild = bottomRightNode->GetObjectsAt(px, py, w, h);
		resObjects.insert(resObjects.end(), bottomRightChild.begin(), bottomRightChild.end());
	}
	return resObjects;
}

template <typename T>
std::list<T*> QuadTreeNode<T>::GetObjectsAt(float px, float py)
{
	std::list<T*> resObjects;
	if (!IsContainPoint(this, px, py))
	{
		return resObjects;
	}
	resObjects.insert(resObjects.end(), objects.begin(), objects.end());
	// 最后一层;
	if (level == maxLevel)
		return resObjects;

	// 如果有下层节点就把下层节点包含的对象加进来;
	if (upRightNode)
	{
		std::list<T*> upRightChild;
		upRightChild = upRightNode->GetObjectsAt(px, py);
		resObjects.insert(resObjects.end(), upRightChild.begin(), upRightChild.end());
	}
	if (upLeftNode)
	{
		std::list<T*> upLeftChild;
		upLeftChild = upLeftNode->GetObjectsAt(px, py);
		resObjects.insert(resObjects.end(), upLeftChild.begin(), upLeftChild.end());
	}
	if (bottomLeftNode)
	{
		std::list<T*> bottomLeftChild;
		bottomLeftChild = bottomLeftNode->GetObjectsAt(px, py);
		resObjects.insert(resObjects.end(), bottomLeftChild.begin(), bottomLeftChild.end());
	}
	if (bottomRightNode)
	{
		std::list<T*> bottomRightChild;
		bottomRightChild = bottomRightNode->GetObjectsAt(px, py);
		resObjects.insert(resObjects.end(), bottomRightChild.begin(), bottomRightChild.end());
	}
	return resObjects;
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
void QuadTreeNode<T>::RemoveObjectsAt(float px, float py, float w, float h)
{
	// 如果本层节点被包含则删除本层节点的对象;
	// 这个判断主要是对根节点起作用，其他子节点实际在上层都做了判断;
	if (IsContain(px, py, w, h, this))
	{
		// 清除本节点层的对象;
		objects.clear();
		// 最后一层;
		if (level == maxLevel)
			return;

	}
	// 如果有子节点且被包含就销毁子节点，注意别产生野指针;
	// 其实只要上层被包含了，下层肯定被包含，代码还需改进;
	if (upRightNode&&IsContain(px, py, w, h, upRightNode))
	{
		upRightNode->RemoveObjectsAt(px, py, w, h);
		delete upRightNode;
		upRightNode = NULL;

	}
	if (upLeftNode&&IsContain(px, py, w, h, upLeftNode))
	{
		upLeftNode->RemoveObjectsAt(px, py, w, h);
		delete upLeftNode;
		upLeftNode = NULL;

	}
	if (bottomLeftNode&&IsContain(px, py, w, h, bottomLeftNode))
	{
		bottomLeftNode->RemoveObjectsAt(px, py, w, h);
		delete bottomLeftNode;
		bottomLeftNode = NULL;

	}
	if (bottomRightNode&&IsContain(px, py, w, h, bottomRightNode))
	{
		bottomRightNode->RemoveObjectsAt(px, py, w, h);
		delete bottomRightNode;
		bottomRightNode = NULL;
	}
}

template <typename T>
void QuadTreeNode<T>::GetRect(float* _x, float* _y, float* _width, float* _height)
{
	(*_x) = x;
	(*_y) = y;
	(*_width) = width;
	(*_height) = height;
}

template <typename T>
void QuadTreeNode<T>::GetMaxLevel(int* _maxLevel)
{
	(*_maxLevel) = maxLevel;
}

#endif
