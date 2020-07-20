#include "framework.h"
#include "CRingBuffer.h"


CRingBuffer::CRingBuffer(int queueLen)
{
	this->mRingBuffer = new char[queueLen];
	this->mQueueLen = queueLen;
	this->front = 0;
	this->rear = 0;
}

CRingBuffer::~CRingBuffer(void)
{
	delete[] mRingBuffer;
}


// ���� ������� �뷮 ���, Return : ������� �뷮
int CRingBuffer::GetUseSize(void)
{
	// ��Ȯ��
	if (this->rear >= this->front)
	{
		return this->rear - this->front;
	}
	else
	{
		return (this->mQueueLen - this->front) + this->rear;
	}
}

// ���� ���ۿ� ���� �뷮 ���, Return : ���� �뷮
int CRingBuffer::GetFreeSize(void)
{

	if (this->rear >= this->front)
	{
		return (this->mQueueLen - this->rear) + this->front - 1;
	}
	else
	{
		return this->front - this->rear - 1;
	}
}

/////////////////////////////////////////////////////////////////////////
// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
// (������ ���� ����)
//
// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
//
// Parameters: ����.
// Return: (int)��밡�� �뷮.
////////////////////////////////////////////////////////////////////////
int CRingBuffer::DirectEnqueueSize(void)
{

	if (this->rear == this->mQueueLen - 1 && this->front == this->mQueueLen - 1)
	{
		return this->mQueueLen - 1;
	}

	else if (this->rear >= this->front)
	{
		return this->mQueueLen - this->rear - 1;
	}
	else
	{
		return this->front - this->rear - 1;
	}

}

int CRingBuffer::DirectDequeueSize(void)
{
	if (this->front == this->mQueueLen - 1 && this->rear < this->front)
	{
		return this->rear;
	}

	else if (this->rear >= this->front)
	{
		return  this->rear - this->front;
	}
	else
	{
		return this->mQueueLen - this->front - 1;
	}
}

/////////////////////////////////////////////////////////////////////////
// rear �� ����Ÿ ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: (int)���� ũ��.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int queueLen = this->mQueueLen;

	int freeSize = GetFreeSize();

	if (iSize > freeSize)
	{
		iSize = freeSize;
	}

	int sizeCheck = this->rear + iSize;

	if (sizeCheck >= queueLen)
	{
		int directSize = queueLen - this->rear - 1;

		memcpy(&this->mRingBuffer[this->rear + 1], chpData, directSize);

		int addSize = iSize - directSize;

		memcpy(&this->mRingBuffer[(this->rear + directSize + 1) % queueLen], &chpData[directSize], addSize);
	}
	else
	{
		memcpy(&this->mRingBuffer[this->rear + 1], chpData, iSize);

	}

	this->rear = (this->rear + iSize) % queueLen;


	return iSize;
}



/////////////////////////////////////////////////////////////////////////
// front ���� ����Ÿ ������. ReadPos �̵�.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: (int)������ ũ��.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	int queueLen = this->mQueueLen;

	int useSize = GetUseSize();

	if (iSize > useSize)
	{
		iSize = useSize;
	}

	int sizeCheck = this->front + iSize;

	if (sizeCheck >= queueLen)
	{
		int directSize = queueLen - this->front - 1;

		memcpy(chpDest, &this->mRingBuffer[this->front + 1], directSize);

		int addSize = iSize - directSize;

		memcpy(&chpDest[directSize], &this->mRingBuffer[(this->front + directSize + 1) % queueLen], addSize);
	}
	else
	{
		memcpy(chpDest, &this->mRingBuffer[this->front + 1], iSize);

	}

	this->front = (this->front + iSize) % queueLen;

	return iSize;
}




/////////////////////////////////////////////////////////////////////////
// front ���� ����Ÿ �о��. ReadPos ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: (int)������ ũ��.
/////////////////////////////////////////////////////////////////////////
int CRingBuffer::Peek(char* chpDest, int iSize)
{
	int useSize = GetUseSize();

	int cFront = this->front;

	int queueLen = this->mQueueLen;

	if (iSize > useSize)
	{
		iSize = useSize;
	}


	int sizeCheck = cFront + iSize;

	if (sizeCheck >= queueLen)
	{
		int directSize = queueLen - cFront - 1;

		memcpy(chpDest, &this->mRingBuffer[cFront + 1], directSize);

		cFront = cFront + directSize;

		int addSize = iSize - directSize;

		memcpy(&chpDest[directSize], &this->mRingBuffer[(cFront + 1) % queueLen], addSize);

		//cFront = (cFront + addSize) % queueLen;
	}
	else
	{
		memcpy(chpDest, &this->mRingBuffer[cFront + 1], iSize);

		//cFront = cFront + iSize;
	}

	return iSize;
}


/////////////////////////////////////////////////////////////////////////
// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::MoveRear(int iSize)
{
	int queueLen = this->mQueueLen;

	int useSize = GetUseSize();

	if (iSize > useSize)
	{
		iSize = useSize;
	}

	int sizeCheck = this->rear - iSize;

	if (sizeCheck < 0)
	{
		int addSize = iSize - this->rear;

		this->rear = this->rear - this->rear;

		this->rear = queueLen - addSize;
	}
	else
	{
		this->rear = this->rear - iSize;
	}

	return;
}



void CRingBuffer::MoveFront(int iSize)
{
	int queueLen = this->mQueueLen;

	int useSize = GetUseSize();

	if (iSize > useSize)
	{
		iSize = useSize;
	}

	int sizeCheck = this->front + iSize;

	if (sizeCheck >= queueLen)
	{
		int directSize = queueLen - this->front - 1;

		this->front = this->front + directSize;

		int addSize = iSize - directSize;

		this->front = (this->front + addSize) % queueLen;
	}
	else
	{
		this->front = this->front + iSize;
	}
	return;
}



/////////////////////////////////////////////////////////////////////////
// ������ ��� ����Ÿ ����. rear�� front�� ��������.
//
// Parameters: ����.
// Return: ����.
/////////////////////////////////////////////////////////////////////////
void CRingBuffer::ClearBuffer(void)
{
	this->front = 0;
	this->rear = 0;
}


/////////////////////////////////////////////////////////////////////////
// ������ Front ������ ����.
//
// Parameters: ����.
// Return: (char *) ���� ������.
/////////////////////////////////////////////////////////////////////////
char* CRingBuffer::GetFrontBufferPtr(void)
{
	return &this->mRingBuffer[this->front + 1];
}


/////////////////////////////////////////////////////////////////////////
// ������ RearPos ������ ����.
//
// Parameters: ����.
// Return: (char *) ���� ������.
/////////////////////////////////////////////////////////////////////////
char* CRingBuffer::GetRearBufferPtr(void)
{
	return &this->mRingBuffer[this->rear];
}
