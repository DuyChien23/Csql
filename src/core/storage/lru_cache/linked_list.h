//
// Created by chiendd on 04/08/2024.
//

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template<typename KeyT, typename ValueT>
struct Node {
    KeyT key;
    ValueT value;
    Node *prev;
    Node *next;

    Node(const ValueT &value) : prev(nullptr), next(nullptr), value(value) {
    }

    Node(const KeyT &key, const ValueT &value) : prev(nullptr), next(nullptr), key(key), value(value) {
    }
};

template<typename KeyT, typename ValueT>
class LinkedList {
protected:
    Node<KeyT, ValueT> *begin;
    Node<KeyT, ValueT> *end;

public:
    LinkedList() : begin(new Node<KeyT, ValueT>(nullptr)), end(new Node<KeyT, ValueT>(nullptr)) {
        begin->prev = begin;
        begin->next = end;
        end->prev = begin;
        end->next = end;
    };

    Node<KeyT, ValueT> *push(KeyT key, ValueT value) {
        Node<KeyT, ValueT> *newNode = new Node<KeyT, ValueT>(key, value);
        end->prev->next = newNode;
        newNode->prev = end->prev;
        newNode->next = end;
        end->prev = newNode;
        return newNode;
    }

    std::pair<KeyT, ValueT> pop() {
        Node<KeyT, ValueT> *removeNode = begin->next;
        begin->next = begin->next->next;
        begin->next->prev = begin;
        return std::make_pair(removeNode->key, removeNode->value);
    }

    void pushToEnd(Node<KeyT, ValueT> *aNode) {
        aNode->prev->next = aNode->next;
        aNode->next->prev = aNode->prev;

        end->prev->next = aNode;
        aNode->prev = end->prev;
        aNode->next = end;
        end->prev = aNode;
    }

    void clear() {
        for (Node<KeyT, ValueT> *iterNode = begin->next, *tempNode = nullptr; iterNode != end;) {
            tempNode = iterNode;
            iterNode = iterNode->next;
            delete tempNode;
        }

        begin->next = end;
        end->prev = begin;
    }
};


#endif //LINKED_LIST_H
