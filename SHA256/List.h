//List.h
//Jacob Schaupp
//EECS 2510, Fall 2018
//12/13/2018
//Contains logic for the list structure

#pragma once

template <typename item>
class List
{
public:
	//this list can be used for storing all of the nodes, or all of the edges associated with a vertex 
	struct listElement
	{
		int indexNum;
		item *item;
		listElement *nextElement = nullptr;
	};
	//constructor 
	List()
	{
		isEmpty = true;
		numElements = 0;
	}

	//add an element to the list
	void Add(item *newItem)
	{
		if (firstElement == nullptr)
		{
			//create first element and add it to the list 
			firstElement = new listElement();
			//populate list information
			firstElement->item = newItem;
			firstElement->indexNum = numElements;
			lastElement = firstElement;

			numElements++; //increment number of elements 
			this->isEmpty = false;
		}
		else
		{
			//create new element and add it to last element in the list 
			//listElement *newElement = new listElement();
			listElement *Element = new listElement();
			Element->item = newItem;
			Element->indexNum = numElements;
			lastElement->nextElement = Element;
			lastElement = Element; //set last element in list as the new element 

			numElements++; //increment number of elements 
		}
	}

	//returns item at an index refrenced from the beginning -- 0 based
	item* get(int index)
	{
		if (firstElement != nullptr)
		{
			listElement *currentElement = firstElement;
			for (int i = 0; i <= index - 1; i++)
			{
				currentElement = currentElement->nextElement;
			}
			return currentElement->item;
		}
	}

	//bugs with regular get, no time to correct logic 
	item* newGet(int index)
	{
		listElement *currentElement = nullptr;
		if (firstElement != nullptr)
		{
			for (int i = 0; i < index; i++)
			{
				if (currentElement == nullptr) currentElement = firstElement;
				else currentElement = currentElement->nextElement;
			}
		}
	}

	item* getFirst()
	{
		return firstElement->item;
	}
	//replace a list element's item with a new item
	void replaceElementItem(int elementIndex, item *replacement)
	{
		listElement *targetElement;
		if (firstElement != nullptr)
		{
			targetElement = firstElement; //set the target element to the first element in the lsit 
			for (int i = 0; i <= elementIndex - 1; i++)
			{
				targetElement = targetElement->nextElement;
			}
			targetElement->item = replacement; //set the new item to the target element 
		}
	}


	//get index of listElement that contains the item -- should be used when finding the set of a vertex
	int getIndex(item *targetItem)
	{
		listElement *currentElement;
		if (firstElement != nullptr)
		{
			currentElement = firstElement;
			if (currentElement->item == targetItem)
			{
				return currentElement->indexNum;
			}
		}
	}

	//return if the list is empty 
	bool is_empty()
	{
		return isEmpty;
	}

	//return number of elements 
	int num_element()
	{
		return numElements + 1;
	}

	void deleteElement(int index)
	{
		listElement *lastElementBF;
		lastElementBF = firstElement;
		int i = 0;
		for (i; i < index; i++)
		{
			lastElementBF = lastElementBF->nextElement;
		}
		for (int j = i + 1; j <= numElements - 1; j++)
		{
			lastElementBF->item = lastElementBF->nextElement->item;  //move the rest of the items down 1 index value to fill the hole
			lastElementBF = lastElementBF->nextElement;
		}
		numElements = numElements - 1;
	}

	//returns true if list contains element
	bool contains(item *target)
	{
		listElement *currentElement = nullptr;
		for (int i = 0; i < numElements; i++)
		{
			if (currentElement == nullptr) currentElement = firstElement;
			if (currentElement->item == target)
			{
				return true;
			}
			currentElement = currentElement->nextElement;
		}
		return false;
	}

private:
	bool isEmpty;
	int numElements = 0; //will be 0 based like an array 
	listElement *firstElement = nullptr; //use this for the first element in the list
	listElement *lastElement = nullptr; //use this for quick insert, keep track of last element in list
};