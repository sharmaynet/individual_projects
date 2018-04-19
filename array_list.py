#a List contains an array, represented with a list, and a size and capacity, both
    #represented with int
    #array is a list
    #size is an int
    #capacity is an int
class List:
   def __init__(self):
      self.array = [None]*256
      self.size = 0
      self.capacity = 256
   def __repr__(self):
      return '{!r}'.format(self.array)

   def __eq__(self, other):
      return (type(other) == List and equalmethod(self, other) and self.size == other.size)


#signature: array array -> bool
#make sure arrays have same value regardless of capacity
def equalmethod(array1, array2):
   if array1.size != array2.size:
      return False
   for i in range(array1.size):
      if array1.array[i] != array2.array[i]:
         return False
   if array1.array[0]== None and array2.array[0] == None:
      return True
   return True

#Data Definition:
    #returns an array
#Signature: None -> array
#Purpose statement: returns an empty array
#header
def empty_list():
   x = List()
   return x

#Data Definition:
    #list is an array
    #index is an int
    #value in an int
    #return an array
#Signature: array index value -> array
#purpose statement: returns an array with the added value in the index
#header

def add(list, index, value):
   if (list.size == 0 and index == 0):
      temp = List()
      temp.array[index] = value
      temp.size += 1
      return temp
   else:
      temp = List()
      temp.size = list.size
      extra = list.size//List().capacity
      for i in range (index):
         temp.array[i] = list.array[i]
      temp.array[index] = value
      temp.size +=1
      for i in range(index, list.capacity-1):
         temp.array[i+1] = list.array[i]
      return temp


#[None, None, None, None, None]
#Data Definition:
    #list is an array
    #return an int
#Signature: list -> int
#purpose statement: returns a number of the length of the array
#header
def length(list):
   return list.size
#data definition:
    #list is an array
    #index is an int
    #returns an int
#signature: array int -> int
#returns the value that is in the index of the list
#header
def get(list, index):
   if index <0 or index > list.capacity-1:
      raise IndexError()
   else:
      return list.array[index]

#data definition:
    #list is an array
    #index is an int
    #value is an int
    #return an array
#signature: array int int -> array
#purpose: returns a new value on the index in the list
#header
def set(list, index, value):
   list.array[index] = value
   return list

#data definition:
    #list is an array
    #index is an int
    #returns an tuple
        #any value
        #array
#signature: array int -> tuple
#returns the element and the index and the new list without the index linked list
    #pos keeps count of what index the linked list is on
#header
def remove(list, index):
   if index <0 or (index+1) > list.capacity or list == empty_list():
      raise IndexError()
   else:
      value = list.array[index]
      temp = List()
      temp.size = list.size
      extra = list.size//List().capacity
      for i in range (index):
         temp.array[i] = list.array[i]
      temp.array[index] = list.array[index+1]
      temp.size -=1
      for i in range(index+1,list.capacity):
         temp.array[i-1] = list.array[i]
      return (value, temp)
