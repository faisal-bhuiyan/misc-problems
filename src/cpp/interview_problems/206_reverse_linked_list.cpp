/*
 * Given the head of a singly linked list, reverse the list, and return the reversed list.

 * Example 1:
 * Input: head = [1,2,3,4,5]
 * Output: [5,4,3,2,1]

 * Example 2:
 * Input: head = [1,2]
 * Output: [2,1]
 *
 * Example 3:
 * Input: head = []
 * Output: []
 */

#include <memory>

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */

// version 1.00: Iterative solution w/ two pointers
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        // Degenerate case: empty linked list
        if (head == nullptr) {
            return head;
        }

        // Reverse the linked list iteratively
        ListNode* previous{nullptr};  // pointer 1
        auto current = head;          // pointer 2
        while (current != nullptr) {
            // Store the next pointer from current before we break it
            auto next = current->next;
            // Now we can set current.next to point to the previous pointer
            current->next = previous;
            // Time to update the two pointer: previous -> current and current -> next
            previous = current;
            current = next;
        }
        // previous should be the new head
        return previous;
    }
};

// version 1.01: Recursive solution (not because it's faster but it came up during an inteview)
class Solution {
public:
    ListNode* RecursiveLinkedListReversal(ListNode* current, ListNode* prev) {
        // base case
        if (current == nullptr) {
            return prev;
        }

        // recursive case
        auto next = current->next;
        current->next = prev;
        return RecursiveLinkedListReversal(next, current);
    }

    ListNode* reverseList(ListNode* head) {
        // Degenerate case: empty linked list
        if (head == nullptr) {
            return head;
        }
        return RecursiveLinkedListReversal(head, nullptr);
    }
};
