#AnyList can be one of
    #string None, or
    #Pair(first, rest)
        #Pair is  a class
        #first is one of:
            #string
            #float
            #int
        #rest is an AnyList
class Pair:
   def __init__(self, first, rest):
      self.first = first
      self.rest = rest
   def __eq__(self, other):
      return ((type(other)==Pair) and self.first == other.first and self.rest== other.rest)
   def __repr__(self):
      return("Pair({!r}, {!r})".format(self.first, self.rest))

#data defintion: returns a list
#signature: None -> list
#purpose statement: returns an empty list
#header
def empty_list():
   return None

#data definition: list is an AnyList
    #list is AnyList
    #index is an int
    #Any is one of
        #int
        #float
        #string
    #pos is int
    #returns AnyList
#signature: AnyList, int, Any int -> AnyList
#purpose statement: returns a list with the added Pair in the given index with the given value
    #pos keeps count of what index the linked list is on
#header
def add(list, index, value, pos=0):
   if index < 0 or (index > pos and list == None):
      raise IndexError()
   if list == None and (pos==index or index==0):
      return Pair(value, None)
   else:
      if pos == index:
         return Pair(value, list)
      else:
         return Pair(list.first, add(list.rest, index, value, pos+1))


#Data Definition:
    #linklist is a AnyList
    #returns an integer
#Signature: AnyList -> int
#Purpose Statement: returns number of values in the list
#Header
def length(linklist):
   if (linklist == None):
      return 0
   else:
      return 1+length(linklist.rest)

#data definition
    #list is AnyList
    #index is int
    #pos is int
    #returns value - one of:
        #-int
        #float
        #string
#signature: AnyList int int ->value
#purpose: gets the value of the index in the list
    #pos keeps count of what index the linked list is on
#header
def get(list, index, pos=0):
   if index <0 or list==None:
      raise IndexError()
   else:
      if pos == index:
         return list.first
      else:
         return get(list.rest, index, 1+pos)

#data definition
    #list is AnyList
    #index is an int
    #Any is one of
        #int
        #float
        #string
    #pos is int
    #returns Anylist
#signature: AnyList, int, any, int -> AnyList
#purpose: returns a new value on the index in the list
#header
def set(list, index, value, pos=0):
   if index <0 or (list==None):
      raise IndexError()
   else:
      if index == pos:
         return Pair(value, list.rest)
      else:
         return Pair(list.first, set(list.rest, index, value, 1+pos))

#Data Definition
    #list is AnyList
    #index in an int
    #pos is an accumulator -> int
    #x is an accumulator - anyList
    #returns a ClassShape
#signature: AnyList int int anyList -> tuple
#returns the element and the index and the new list without the index linked list
    #pos keeps count of what index the linked list is on
    #x stores the remove_pair aka the AnyList without the value of the index
#header
def remove(list, index, pos=0, x=None):
   if index <0 or list == None:
      raise IndexError()
   else:
      if index == pos and pos!=0:
         temp = list.first
      elif index ==0:
         temp = list.first
         x = remove_pair(list, index)
      elif pos == 0:
         return remove(list.rest, index, pos+1, remove_pair(list, index))
      else:
         return remove(list.rest, index, pos+1, x)
   return (temp, x)

#Data Definition
    #list is AnyList
    #index in an int
    #pos is int
    #returns tuple
        #-value
        #-AnyList
#signature: AnyList int int -> tuple
#returns the Pair (field part of the ClassShape) of the remove function
#header
def remove_pair(list, index, pos=0):
   if index <0 or list == None:
      raise IndexError()
   else:
      if index == pos:
         return list.rest
      else:
         return Pair(list.first, remove_pair(list.rest, index, pos+1))
#sorted_linked_list int function -> list
#returns new list that inserts a value into a sorted list in ascending order
def insert_sorted(list, val, func):
   if list == None:
      return Pair(val, None)
   else:
      if func(val, list.first):
         return Pair(val, list)
      else:
         return Pair(list.first, insert_sorted(list.rest, val, func))
