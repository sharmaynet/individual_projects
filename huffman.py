import unittest
import array_list
import linked_list
from huffman_bits_io import *

#----------------------------------------------------------------------------------

# 2.1
#.txt -> array
#takes in a .txt file and places the amount of occurrence in the ASCII index
def count(inf):
   inFile = open(inf, 'r')
   if inFile.readline() == '':
      inFile.close()
      return array_list.empty_list()
   else:
      inFile.close()
      inFile = open(inf, 'r')
      new = array_list.empty_list()
      for temp in inFile:
         for char in range (len(temp)):
            index = ord(temp[char])
            val = array_list.get(new, index)
            if val == None:
               new = array_list.set(new, index, 1)
            else:
               new = array_list.set(new, index, val+1)
   inFile.close()
   return new
#----------------------------------------------------------------------------------
# 2.2
#a Huffman tree is one of:
    # - Huffman Node
    # - Leaf
class Leaf:
   def __init__(self, character, occurrence):
      self.character = character #character
      self.occurrence = occurrence #int - number of occurrence
      self.chrs = character
   def __eq__(self, other):
      return ((type(other)==Leaf) and self.character == other.character and self.occurrence== other.occurrence)
   def __repr__(self):
      return("Leaf({!r}, {!r})".format(self.character, self.occurrence))
class Node:
   def __init__(self, left, right):
      self.character = 0 #str of chr in Node
      self.occurrence = 0 #int - added thing
      self.left = left #Leaf or Node
      self.right = right #Leaf or Node
      self.chrs = '' #smallest ord of the leaf.character
   def __eq__(self, other):
      return ((type(other)==Node) and self.character == other.character and self.occurrence== other.occurrence and self.chrs == other.chrs and self.left == other.left and self.right == other.right)
   def __repr__(self):
      return("Node({!r}, {!r})".format(self.left, self.right))
#----------------------------------------------------------------------------------
# 2.3
#a and b are leaves
#if the character is less than the other character
def comes_before_characters(a, b):
   if ord(a.chrs) < ord(b.chrs):
      return True
   else:
      return False
#a and b are leaves
#checks lesser occurrence. if equal, checks lessar character
def comes_before(a, b):
   if a.occurrence < b.occurrence:
      return True
   elif a.occurrence == b.occurrence:
      return comes_before_characters(a, b)
   else:
      return False

#x is an array list
#x = count()
#array func -> linked
#returns a linked list of the leaves in occurrence order
def build_leaf(x, func):
   #convert array into linked list
   new = linked_list.empty_list()
   for index in range (x.capacity):
      if(array_list.get(x, index)) != None:
         leaf_node = Leaf(chr(index), array_list.get(x, index))
         new = linked_list.insert_sorted(new, leaf_node, func)
   return new
#leaves is a linked list
#linked -> tree
#build one Node out of two leafs, one leaf and one node, or two nodes
#return a tree of all the nodes with leaf left and right
#leaves = build_leaf(x)
def build_nodes(leaves):
   if leaves.rest == None:
      return leaves.first
   while (leaves.rest != None):
      new = Node(leaves.first, leaves.rest.first)
      new.character = (leaves.first.character) + (leaves.rest.first.character)
      sum = leaves.first.occurrence + leaves.rest.first.occurrence
      new.occurrence+= sum
      if new.left.chrs < new.right.chrs:
          new.chrs = new.left.chrs
      else:
          new.chrs = new.right.chrs
      leaves = linked_list.insert_sorted(leaves, new, comes_before)
      if leaves.rest.rest != None:
         leaves = leaves.rest.rest
   return leaves.first

#----------------------------------------------------------------------------------
# 2.4
#nd is the Node list
#x is an array_list(from x=count())
#Node array -> array
#return an array list of what huffman code a specific leaf should be
def chr_code(nd, x):
   new = array_list.empty_list()
   if type(nd)== Leaf:
      for i in range(x.capacity):
         if array_list.get(x, i)!= None:
            new = array_list.set(new, i, '0')
      return new
   else:
      for i in range (x.capacity):
         if array_list.get(x, i)!= None:
            new = array_list.set(new, i, indv_code(nd, chr(i)))
      return new
#nd is the Node list
#ch is a character(str)
#Node str -> string (of ints)
#finds the character code for an individual character
def indv_code(nd, ch, s=''):
   if type(nd.left)!= Leaf:
      if ch in nd.left.character:
         s+='0'
         return indv_code(nd.left, ch, s)
   if type(nd.right)!= Leaf:
      if ch in nd.right.character:
         s+='1'
         return indv_code(nd.right, ch, s)
   if type(nd.left) == Leaf:
      if nd.left.chrs == ch:
         s+='0'
         return s
   if type(nd.right) == Leaf:
      if nd.right.chrs == ch:
         s+='1'
         return s

#----------------------------------------------------------------------------------
# 2.5
#inf is an infile .txt file
#outf is an outfile .bin file
#.txt .bin -> string
#returns a string of whatever characters are in the Tree. Also writes the .txt file into the .bin file in HEX
def huffman_encode(inf, outf):
   hb_writer = HuffmanBitsWriter(outf)
   c = count(inf)
   x = build_leaf(c, comes_before)
   if linked_list.length(x) == 0:
      hb_writer.write_byte(0)
      hb_writer.close()
      return ''
   lst = build_nodes(x)
   new = chr_code(lst, c)
   y = build_leaf(c, comes_before_characters)
   lgth = linked_list.length(y)
   hb_writer.write_byte(lgth)
   for i in range (lgth):
      hb_writer.write_byte(ord(y.first.character))
      hb_writer.write_int(y.first.occurrence)
      y = y.rest
   if type(lst)== Node:
      inFile = open(inf, 'r')
      s = ''
      for line in inFile:
         for char in line:
            s += str(array_list.get(new, ord(char)))
      hb_writer.write_code(s)
      inFile.close()
   hb_writer.close()
   return lst.character
#inf is an infile .bin file
#outf is an outfile .txt file
#.bin .txt -> None
#returns nothing, but changes the .bin file that is in hex to a .txt file in huffman code
def huffman_decode(inf, outf):
   hb_reader = HuffmanBitsReader(inf)
   outFile = open(outf, 'w')
   new = array_list.empty_list()
   char_num = hb_reader.read_byte()
   if char_num == 0:
      hb_reader.close()
      outFile.close()
      return None
   for i in range(char_num):
      index = hb_reader.read_byte()
      occ = hb_reader.read_int()
      new = array_list.set(new, index, occ)
   x = build_leaf(new, comes_before)
   lst = build_nodes(x)
   string = ''
   if type(lst) == Node:
      temp = lst
      for i in range (check_size(new, chr_code(lst, new))):
         bit = hb_reader.read_bit()
         if bit == True:
            temp = temp.right
         else:
            temp = temp.left
         if type(temp) == Leaf:
            string += temp.character
            temp = lst
   if type(lst) == Leaf:
      for i in range(lst.occurrence):
         string += lst.character
   outFile.write(string)
   hb_reader.close()
   outFile.close()
   return None
#cnt is the count(x)
#n is the array of the huffman code
def check_size(cnt, n):
   sum = 0
   for i in range (n.capacity):
      if array_list.get(cnt, i)!= None:
         x = len(array_list.get(n, i)) * (array_list.get(cnt, i))
         sum += x
   return sum

class TestList(unittest.TestCase):
   def test_class_leaf(self):
      a = Leaf('a', 2)
      b = Leaf('a', 2)
      self.assertEqual(repr(a), "Leaf('a', 2)")
      self.assertEqual(a, b)
   def test_class_node(self):
      a = Node(Leaf('a', 2), Leaf('b', 3))
      b = Node(Leaf('a', 2), Leaf('b', 3))
      self.assertEqual(repr(a), "Node(Leaf('a', 2), Leaf('b', 3))")
      self.assertEqual(a, b)
   def test_count(self):
      y = array_list.empty_list()
      y = array_list.add(y, ord(' '), 3)
      y = array_list.add(y, ord('a'), 4)
      y = array_list.add(y, ord('b'), 3)
      y = array_list.add(y, ord('c'), 2)
      y = array_list.add(y, ord('d'), 1)
      y.size = 0
      self.assertEqual(count('test.txt'), y)
      x = count('test.txt')
      y = build_leaf(x, comes_before)
      self.assertEqual(build_leaf(x, comes_before), y)
      x = build_leaf(x, comes_before)
      y = build_nodes(x)
      self.assertEqual(build_nodes(x), y)
      lst = build_nodes(x)
      z = array_list.empty_list()
      z = array_list.add(z, ord(' '), '00')
      z = array_list.add(z, ord('a'), '01')
      z = array_list.add(z, ord('b'), '100')
      z = array_list.add(z, ord('c'), '101')
      z = array_list.add(z, ord('d'), '11')
      z.size = 0
      self.assertEqual(chr_code(lst, count('test.txt')),  z)
   def test_comes_before(self):
      a = Leaf('a', 2)
      b = Leaf('b', 1)
      self.assertTrue(comes_before_characters(a, b))
      self.assertFalse(comes_before_characters(b, a))
      self.assertTrue(comes_before(b, a))
      self.assertFalse(comes_before(a, b))
      b = Leaf('b', 2)
      self.assertTrue(comes_before(a, b))
      self.assertFalse(comes_before(b, a))
   def test_length_of_decode(self):
      hb_reader = HuffmanBitsReader('test_encoded.bin')
      new = array_list.empty_list()
      char_num = hb_reader.read_byte()
      for i in range(char_num):
         index = hb_reader.read_byte()
         occ = hb_reader.read_int()
         new = array_list.set(new, index, occ)
      x = build_leaf(new, comes_before)
      lst = build_nodes(x)
      hb_reader.close()
      self.assertEqual(check_size(new, chr_code(lst, new)), 29)
   def test_leaf(self):
      a = count('a.txt')
      a = build_leaf(a, comes_before)
      lst = build_nodes(a)
      self.assertEqual(lst, a.first)
   def test_encoder0(self):
      s = huffman_encode("test.txt", "test_encoded.bin")
      self.assertEqual(s, ' bdca')
   def test_encoder1(self):
      s = huffman_encode("textfile.txt", "textfile_encoded.bin")
      self.assertEqual(s, "acb")
   def test_encoder2(self):
      s = huffman_encode('blank.txt', 'blank_encoded.bin')
      self.assertEqual(s, '')
   def test_encoder3(self):
      s = huffman_encode('a.txt', 'a_encoded.bin')
      self.assertEqual(s, 'a')
   def test_encoder4(self):
      s = huffman_encode('large.txt', 'large_encoded.bin')
      self.assertEqual(s, "dlrgwGKU]BS?;fa \ncbWAotkIDEmT'![J(Q-H.e,uhvPYzjqxCpyFLNMOR)0146Vnsi")
   def test_decoder1(self):
      s = huffman_decode('test_encoded.bin', 'test.txt')
      self.assertEqual(s, None)
   def test_decoder2(self):
      s = huffman_decode('blank_encoded.bin', 'blank.txt')
      self.assertEqual(s, None)
   def test_decoder3(self):
      s = huffman_decode('a_encoded.bin', 'a.txt')
      self.assertEqual(s, None)
if __name__ == '__main__':
   #huffman_encode('textfile.txt', 'textfile_encoded.bin')
   #huffman_decode('test_encoded.bin', 'test.txt')
   unittest.main()
