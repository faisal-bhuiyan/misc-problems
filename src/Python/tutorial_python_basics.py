#------------------------------------------------------------------------------------------------
# Lists
#------------------------------------------------------------------------------------------------

print("\nLists")
print(f"Lists are ordered collections of objects. They are mutable and can be changed after creation.")
print("--------------------------------------------------------")

# Lists are ordered collections of objects.
numbers = [1, 2, 3, 4, 5]
print(f"This is the list we are working with: {numbers}")

# You can add to a list with the append method - this is a O(1) operation.
numbers.append(6)
print(f"This is the list after adding 6: {numbers}")

# You can remove from a list with the remove method - this is a O(n) operation.
numbers.remove(3)
print(f"This is the list after removing 3: {numbers}")

# You can sort a list with the sort method - this is a O(n log n) operation.
numbers.sort()
print(f"This is the list after sorting: {numbers}")

# You can reverse a list with the reverse method - this is a O(n) operation.
numbers.reverse()
print(f"This is the list after reversing: {numbers}")


#--------------------------------------------------------
# More list operations
#--------------------------------------------------------
print("\nLists store sequences")
li = []
print(f"This is the empty list: {li}")

# You can start with a prefilled list
other_li = [4, 5, 6]

# Add stuff to the end of a list with append
li.append(1)    # li is now [1]
print(f"This is the list after adding 1: {li}")
li.append(2)    # li is now [1, 2]
print(f"This is the list after adding 2: {li}")
li.append(4)    # li is now [1, 2, 4]
print(f"This is the list after adding 4: {li}")
li.append(3)    # li is now [1, 2, 4, 3]
print(f"This is the list after adding 3: {li}")
# Remove from the end with pop
li.pop()        # => 3 and li is now [1, 2, 4]
print(f"This is the list after popping the last element: {li}")
# Let's put it back
li.append(3)    # li is now [1, 2, 4, 3] again.
print(f"This is the list after adding 3 again: {li}")

# Access a list like you would any array
li[0]   # => 1
print(f"This is the first element of the list: {li[0]}")
# Look at the last element
li[-1]  # => 3
print(f"This is the last element of the list: {li[-1]}")

# Looking out of bounds is an IndexError
print(f"Trying to access the 5th element of the list")
try:
    li[4]  # Raises an IndexError
except IndexError as e:
    print(f"IndexError: There is no element at index 4: {e}")

# You can look at ranges with slice syntax.
# The start index is included, the end index is not
# (It's a closed/open range for you mathy types.)
li[1:3]   # Return list from index 1 to 2 => [2, 4]
print(f"This is the list li[1:3] from index 1 to 2: {li[1:3]}")
li[2:]    # Return list starting from index 2 => [4, 3]
print(f"This is the list li[2:] from index 2 to the end: {li[2:]}")
li[:3]    # Return list from beginning until index 3  => [1, 2, 4]
print(f"This is the list li[:3] from the beginning to index 3: {li[:3]}")
li[::2]   # Return list selecting elements with a step size of 2 => [1, 4]
print(f"This is the list li[::2] selecting elements with a step size of 2: {li[::2]}")
li[::-1]  # Return list in reverse order => [3, 4, 2, 1]
print(f"This is the list li[::-1] in reverse order: {li[::-1]}")
# Use any combination of these to make advanced slices
# li[start:end:step]

# Make a one layer deep copy using slices
print(f"Making a one layer deep copy of the list li...")
li2 = li[:]  # => li2 = [1, 2, 4, 3] but (li2 is li) will result in false.
print(f"This is the copy of the list li: {li2}")

# Remove arbitrary elements from a list with "del"
del li[2]  # li is now [1, 2, 3]
print(f"This is the list after deleting the element at index 2: {li}")

# Remove first occurrence of a value
li.remove(2)  # li is now [1, 3]
print(f"This is the list after removing the first occurrence of 2: {li}")
#li.remove(2)  # Raises a ValueError as 2 is not in the list

# Insert an element at a specific index - this is a O(n) operation.
li.insert(1, 2)  # li is now [1, 2, 3] again
print(f"This is the list after inserting 2 at index 1: {li}")

# Get the index of the first item found matching the argument
li.index(2)  # => 1
#li.index(4)  # Raises a ValueError as 4 is not in the list
print(f"The index of the first item found matching the argument 2 is: {li.index(2)}")
#print(f"The index of the first item found matching the argument 4 is: {li.index(4)}")

# You can add lists
# Note: values for li and for other_li are not modified.
print(f"Adding the lists li and other_li...")
print(f"li: {li}")
print(f"other_li: {other_li}")
li + other_li  # => [1, 2, 3, 4, 5, 6]
print(f"The result of adding the lists li and other_li is: {li + other_li}")

# Concatenate lists with "extend()" - this is a O(n) operation.
li.extend(other_li)  # Now li is [1, 2, 3, 4, 5, 6]
print(f"The list li after extending it with the list other_li is: {li}")

# Check for existence in a list with "in"
1 in li  # => True
print(f"1 in li: {1 in li}")
# Examine the length with "len()"
len(li)  # => 6
print(f"len(li): {len(li)}")

#------------------------------------------------------------------------------------------------
# Tuples
#------------------------------------------------------------------------------------------------

print("\nTuples")
print(f"Tuples are like lists but are immutable, i.e. cannot be changed after creation.")
print("--------------------------------------------------------")

# Tuples are like lists but are immutable.
tup = (1, 2, 3)
print(f"tup: {tup}")
tup[0]      # => 1
print(f"tup[0]: {tup[0]}")
#tup[0] = 3  # Raises a TypeError

# Note that a tuple of length one has to have a comma after the last element but
# tuples of other lengths, even zero, do not.
type((1))   # => <class 'int'>
print(f"type((1)): {type((1))}")
type((1,))  # => <class 'tuple'>
print(f"type((1,)): {type((1,))}")
type(())    # => <class 'tuple'>
print(f"type(()): {type(())}")

# You can do most of the list operations on tuples too
len(tup)         # => 3
print(f"len(tup): {len(tup)}")
tup + (4, 5, 6)  # => (1, 2, 3, 4, 5, 6)
print(f"tup + (4, 5, 6): {tup + (4, 5, 6)}")
tup[:2]          # => (1, 2)
print(f"tup[:2]: {tup[:2]}")
2 in tup         # => True
print(f"2 in tup: {2 in tup}")

# You can unpack tuples (or lists) into variables
a, b, c = (1, 2, 3)  # a is now 1, b is now 2 and c is now 3
print(f"a, b, c = (1, 2, 3): a: {a}, b: {b}, c: {c}")
# You can also do extended unpacking
a, *b, c = (1, 2, 3, 4)  # a is now 1, b is now [2, 3] and c is now 4
print(f"a, *b, c = (1, 2, 3, 4): a: {a}, b: {b}, c: {c}")
# Tuples are created by default if you leave out the parentheses
d, e, f = 4, 5, 6  # tuple 4, 5, 6 is unpacked into variables d, e and f
# respectively such that d = 4, e = 5 and f = 6
print(f"d, e, f = 4, 5, 6: d: {d}, e: {e}, f: {f}")
# Now look how easy it is to swap two values
e, d = d, e  # d is now 5 and e is now 4
print(f"e, d = d, e: e: {e}, d: {d}")

#------------------------------------------------------------------------------------------------
# Sets
#------------------------------------------------------------------------------------------------

print("\nSets")
print(f"Sets are unordered collections of unique, hashable elements. Duplicates are ignored; membership tests are typically O(1) average time.")
print("--------------------------------------------------------")

# Sets are unordered collections of unique, hashable elements.
# Duplicates are ignored; membership tests are typically O(1) average time.
empty_set = set()
print(f"empty set: {empty_set}")

# Literal syntax uses braces (but {} alone is an empty dict, not an empty set)
some_set = {1, 2, 2, 3, 3, 3}
print(f"Adding the numbers 1, 2, 2, 3, 3, 3 to the set...")
print(f"some_set (duplicates collapsed): {some_set}")

# Build a set from any iterable — handy for deduplication
print(f"Building a set from the list [1, 2, 2, 3]...")
from_list = set([1, 2, 2, 3])
print(f"set from list: {from_list}")

# Add one element — average O(1)
s = {1, 2, 3}
print(f"Adding the number 4 to the set...")
s.add(4)
print(f"after add(4): {s}")
s.add(4)  # no error; set unchanged
print(f"Adding the number 4 to the set again...")
print(f"after add(4) again: {s}")

# Remove: remove() raises KeyError if missing; discard() is silent
print(f"Removing the number 3 from the set...")
s.remove(3)
print(f"after remove(3): {s}")
# s.remove(99)  # KeyError
print(f"Discarding the number 99 from the set...")
s.discard(99)   # no error
print(f"after discard(99) (missing): {s}")

# Membership
print(f"Checking if the number 2 is in the set...")
print(f"2 in s: {2 in s}")
print(f"Checking if the number 99 is in the set...")
print(f"99 in s: {99 in s}")

# Set algebra (return new sets unless noted)
a = {1, 2, 3, 4}
b = {3, 4, 5, 6}
print(f"Sets a and b: {a} and {b}")
print(f"Union of sets a and b: {a | b}")
print(f"Intersection of sets a and b: {a & b}")
print(f"Difference of sets a and b: {a - b}")
print(f"Symmetric difference of sets a and b: {a ^ b}")

# In-place updates
u = {1, 2}
u.update({2, 3})       # same as |=
print(f"Sets u after updating with {2, 3}: {u}")

# Subset / superset
print(f"Checking if {1, 2} is a subset of {1, 2, 3}...")
print(f"{{1, 2}} <= {{1, 2, 3}}: {({1, 2} <= {1, 2, 3})}")

# Elements must be hashable (like dict keys): no lists as members
# invalid = {[1, 2]}  # TypeError: unhashable type: 'list'
valid_set = {(1, 2), 3, "x"}
print(f"Valid set with tuple, int, str: {valid_set}")

# frozenset is an immutable set (hashable) — can be a dict key or set member
fs = frozenset([1, 2, 3])
print(f"Frozen set: {fs}")

# Set comprehension (like list comprehension, but unique values)
squares_mod = {x * x % 7 for x in range(10)}
print(f"Set comprehension (x*x % 7) for x in range(10): {squares_mod}")

#------------------------------------------------------------------------------------------------
# Dictionaries
#------------------------------------------------------------------------------------------------

print("\nDictionaries")
print(f"Dictionaries store mappings from keys to values. They are mutable and can be changed after creation.")
print(f"Keys must be immutable, values can be of any type.")
print("--------------------------------------------------------")

# Dictionaries store mappings from keys to values
empty_dict = {}
# Here is a prefilled dictionary
filled_dict = {"one": 1, "two": 2, "three": 3}
print(f"filled_dict: {filled_dict}")

# Note keys for dictionaries have to be immutable types. This is to ensure that
# the key can be converted to a constant hash value for quick look-ups.
# Immutable types include ints, floats, strings, tuples.

#invalid_dict = {[1,2,3]: "123"}  # => Yield a TypeError: unhashable type: 'list'
valid_dict = {(1,2,3):[1,2,3]}   # Values can be of any type, however.
print(f"valid_dict: {valid_dict}")

# Look up values with []
filled_dict["one"]  # => 1
print(f"filled_dict['one']: {filled_dict['one']}")

# Get all keys as an iterable with "keys()". We need to wrap the call in list()
# to turn it into a list. We'll talk about those later.  Note - for Python
# versions <3.7, dictionary key ordering is not guaranteed. Your results might
# not match the example below exactly. However, as of Python 3.7, dictionary
# items maintain the order at which they are inserted into the dictionary.
list(filled_dict.keys())  # => ["three", "two", "one"] in Python <3.7
list(filled_dict.keys())  # => ["one", "two", "three"] in Python 3.7+
print(f"filled_dict.keys(): {filled_dict.keys()}")

# Get all values as an iterable with "values()". Once again we need to wrap it
# in list() to get it out of the iterable. Note - Same as above regarding key
# ordering.
list(filled_dict.values())  # => [3, 2, 1]  in Python <3.7
list(filled_dict.values())  # => [1, 2, 3] in Python 3.7+
print(f"filled_dict.values(): {filled_dict.values()}")

# Check for existence of keys in a dictionary with "in"
"one" in filled_dict  # => True
1 in filled_dict      # => False
print(f"1 in filled_dict: {1 in filled_dict}")

# Looking up a non-existing key is a KeyError
#filled_dict["four"]  # KeyError

# Use "get()" method to avoid the KeyError
filled_dict.get("one")      # => 1
print(f"filled_dict.get('one'): {filled_dict.get('one')}")
filled_dict.get("four")     # => None
print(f"filled_dict.get('four'): {filled_dict.get('four')}")
# The get method supports a default argument when the value is missing
filled_dict.get("one", 4)   # => 1
print(f"filled_dict.get('one', 4): {filled_dict.get('one', 4)}")
filled_dict.get("four", 4)  # => 4
print(f"filled_dict.get('four', 4): {filled_dict.get('four', 4)}")

# "setdefault()" inserts into a dictionary only if the given key isn't present
filled_dict.setdefault("five", 5)  # filled_dict["five"] is set to 5
filled_dict.setdefault("five", 6)  # filled_dict["five"] is still 5
print(f"filled_dict.setdefault('five', 5): {filled_dict.setdefault('five', 5)}")
print(f"filled_dict.setdefault('five', 6): {filled_dict.setdefault('five', 6)}")

# Adding to a dictionary
filled_dict.update({"four":4})  # => {"one": 1, "two": 2, "three": 3, "four": 4}
filled_dict["four"] = 4         # another way to add to dict
print(f"filled_dict.update({'four':4}): {filled_dict.update({'four':4})}")
print(f"filled_dict['four']: {filled_dict['four']}")

# Remove keys from a dictionary with del
del filled_dict["one"]  # Removes the key "one" from filled dict
print(f"filled_dict after deleting 'one': {filled_dict}")

#------------------------------------------------------------------------------------------------
# List comprehensions
#------------------------------------------------------------------------------------------------

print("\nList comprehensions")

numbers = [1, 2, 3, 4, 5]
print(f"This is the list we are working with: {numbers}")
# Use ternary operator to filter even numbers
filtered_numbers = [x if x % 2 == 0 else "Odd" for x in numbers]
print("Filtered list:", filtered_numbers)
# Filtered list: ['Odd', 2, 'Odd', 4, 'Odd']

list_integers = [0, 1, 2, 3, 4, 5, 6, 7]
print(f"This is the list we are working with: {list_integers}")
print(f"Printing the square of only the odds: {[x**2 for x in list_integers if x % 2 != 0]}")

#------------------------------------------------------------------------------------------------
# Enumerate and zip
#------------------------------------------------------------------------------------------------

print("\nEnumerate and zip")
list1 = [1, 2, 3]
list2 = [4, 5, 6]
print(f"We need to merge list_1: {list1} w/ list_2 {list2}")

print(f"Enumerating list1 members...")
for index, value in enumerate(list1):
    print(f"index: {index}, value: {value}")

print(f"Enumerating list2 members...")
for index, value in enumerate(list2):
    print(f"index: {index}, value: {value}")

print(f"Let's zip list1 with list2...")
for a, b in zip(list1, list2):
    print(f"a: {a}, b: {b}")

print(f"Let's zip list1 with list2 and print the result...")
for item in zip(list1, list2):
    print(f"item: {item}")

#------------------------------------------------------------------------------------------------
# Unpacking
#------------------------------------------------------------------------------------------------

print("Unpacking")

a, b, c = (1, 2, 3)
print(f"Unpacking the tuple (1, 2, 3) into a, b, c: a: {a}, b: {b}, c: {c}")

first, *rest = [1, 2, 3, 4, 5]
print(f"Unpacking the list [1, 2, 3, 4, 5] into first: {first} and rest: {rest}")

#------------------------------------------------------------------------------------------------
# Sorting with key
#------------------------------------------------------------------------------------------------

print("\nSorting with key")
edges = [(0, 1, 10), (1, 2, 20), (0, 2, 30)]
edges.sort(key=lambda x: x[2])          # sort by weight
print(f"Edges sorted by weight: {edges}")

class Bus:
    def __init__(self, voltage):
        self.voltage = voltage

print(f"Sorting buses by voltage in descending order...")
buses = [Bus(10), Bus(20), Bus(30)]
buses.sort(key=lambda b: (-b.voltage))  # descending
print(f"Buses sorted by voltage in descending order: {[bus.voltage for bus in buses]}")
