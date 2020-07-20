#pragma once

template <typename T>
class CList
{
private:
	struct Node
	{
		T data;
		Node* prev;
		Node* next;

		bool deleteCheck;

		// 생성자입니다.
		Node(T data = NULL)
		{
			this->data = data;
			prev = nullptr;
			next = nullptr;
			deleteCheck = false;
		}
	};

	Node* head;
	Node* tail;
public:

	class Iterator
	{
	private:
		Node* node;

	public:


		friend class CList;

		Iterator(Node* node = nullptr)
		{
			this->node = node;
		}

		Iterator NextIter()
		{
			Iterator iter(this->node->next);
			return iter;
		}


		// 연산자 오버로딩

		const Iterator& operator++(int)
		{
			const Iterator iterator(this->node->next);
			return iterator;
		}

		Iterator& operator++()
		{
			this->node = this->node->next;
			return *this;
		}


		const Iterator& operator--(int)
		{
			const Iterator iterator(this->node->prev);

			return iterator;
		}

		Iterator& operator--()
		{
			this->node = this->node->prev;

			return *this;
		}

		Node* operator*() const
		{
			return this->node;
		}

		T operator->() const
		{
			return this->node->data;
		}

		bool operator==(const Iterator& iter) const
		{
			return this->node->data == iter.node->data;
		}

		bool operator!=(const Iterator& iter) const
		{
			return !(this->node->data == iter.node->data);
		}
	};


public:

	CList()
	{
		head = new Node();
		tail = new Node();

		head->next = tail;
		tail->prev = head;

		tail->deleteCheck = true;

		head->prev = nullptr;
		tail->next = nullptr;
	}

	~CList()
	{
		Node* prev = nullptr;

		while (head != nullptr)
		{
			prev = head;
			head = head->next;
			delete prev;
		}
	}

	void PushBack(T data)
	{
		Node* node = new Node(data);


		// 테일 뒤에 추가
		InputNode(tail, node);
	}

	void PushFront(T data)
	{
		Node* node = new Node(data);

		InputNode(head->next, node);
	}

	void insert(Iterator iter, T data)
	{
		Node* node = new Node(data);

		InputNode(iter.node, node);
	}

	Iterator begin()
	{

		return Iterator(head->next);
	}

	Iterator end()
	{
		return Iterator(tail);
	}

	Iterator erase(Iterator iter)
	{
		Iterator iterBuffer = iter;

		iterBuffer.node->prev->next = iterBuffer.node->next;

		iterBuffer.node->next->prev = iterBuffer.node->prev;

		Iterator iterator(iterBuffer.node->next);

		delete iterBuffer.node;

		return iterator;
	}

	bool Remove(T data)
	{
		Iterator iterE = this->end();

		for (Iterator iter = this->begin(); iter != iterE; ++iter)
		{

			if (iter.node->data == data)
			{

				this->erase(iter);

				return true;
			}
		}
		return false;
	}

private:

	// 특정 노드뒤에 input
	void InputNode(Node* node, Node* newNode)
	{
		newNode->prev = node->prev;
		newNode->next = node;

		node->prev->next = newNode;
		node->prev = newNode;
	}

};

