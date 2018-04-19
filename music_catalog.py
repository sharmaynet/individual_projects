from sys import *
from array_list import *
#a SongFile contains a title, artist, song, and number
    #title is a string
    #artist is a string
    #album is a string
    #number is an int
class Song:
   def __init__(self, title, artist, album):
      self.number = 0 #int - value will be changed
      self.title = title #str
      self.artist = artist #str
      self.album = album #str
   def __eq__(self, other):
      return ((type(other)==Song) and self.title == other.title and self.artist== other.artist and self.album == other.album)
   def __repr__(self):
      return("Song({!r}, {!r}, {!r})".format(self.title, self.artist, self.album))
   def __str__(self):
      return("{:d}--{:s}--{:s}--{:s}".format(self.number, self.title, self.artist, self.album))
#data defition: accepts an SongFile and returns an int
# SongFile -> int
#returns the sum of a SongFile
def length_song(i):
   sum = 0
   for count in i:
      sum+=1
   return sum
#data definitions: accepts a SongFile and returns a SongFile
#SongFile -> SongFile
#returns a new SongFile that is valid, with numbers, and prints out the invalid options
def read(files):
   count = 0
   error = 0
   new = empty_list()
   for temp in files:
      temp = temp.rstrip('\n').split('--')
      if length_song(temp) < 3 and error==0 and temp[0] != '':
         print('Catalog input errors:')
         print('line %d: malformed song information' % (count+1))
         error+=1
      elif length_song(temp) < 3 and error!=0 and temp[0] != '':
         print('line %d: malformed song information' % (count+1))
      elif length_song(temp) == 3:
         new_song = Song(temp[0], temp[1], temp[2])
         new_song.number = count
         new = add(new, count, new_song)
         count+=1
   return new

#prints the list
def print_(list):
   print(str(list))

#data definition: files is a SongFile and x is an int
    #returns printed value
#SongFile int -> print
#prints the info of the song (value 2)
def info(files, x):
   max_size = length(files)
   if max_size <= x or x<0:
      print('\n... Invalid song number')
   else:
      a = file_by_number(files)
      value = get(a, x)
      print('\nSong information ...')
      print('   Number: {:d}'.format(value.number))
      print('   Title: {:s}'.format(value.title))
      print('   Artist: {:s}'.format(value.artist))
      print('   Album: {:s}'.format(value.album))
def file_by_number(files):
   x = empty_list()
   x = sort(files, less_than_number)
   return x
#data definition: x is a SongFile and y is a SongFile
    #returns a bool
#SongFile Songfile -> bool
#compares the number of the SongFile
def less_than_number(x, y):
   if x.number<y.number:
      return True
   elif x.number == y.number:
       if x.artist < y.artist:
          return True
       elif x.artist == y.artist:
          if x.album < y.album:
             return True
          elif x.album == y.album:
             if x.title < y.title:
                return True
             else:
                return False
          else:
             return False
       else:
          return False
   else:
      return False
#data definition: x is a SongFile and y is a SongFile
    #returns a bool
#SongFile Songfile -> bool
#compares the title name of the SongFile
def less_than_title(x, y):
   if x.title<y.title:
      return True
   elif x.title == y.title:
      if x.artist < y.artist:
         return True
      elif x.artist == y.artist:
         if x.album < y.album:
            return True
         elif x.album == y.album:
            if x.number < y.number:
               return True
            else:
               return False
         else:
            return False
      else:
         return False
   else:
      return False
#data definition: x is a SongFile and y is a SongFile
    #returns a bool
#SongFile Songfile -> bool
#compares the artist name of the SongFile
def less_than_artist(x, y):
   if x.artist<y.artist:
      return True
   elif x.artist == y.artist:
       if x.album < y.album:
          return True
       elif x.album == y.album:
          if x.title < y.title:
             return True
          elif x.title == y.title:
             if x.number < y.number:
                return True
             else:
                return False
          else:
             return False
       else:
          return False
   else:
      return False
#data definition: x is a SongFile and y is a SongFile
    #returns a bool
#SongFile Songfile -> bool
#compares the song name of the SongFile
def less_than_album(x, y):
   if x.album<y.album:
      return True
   elif x.album == y.album:
       if x.artist < y.artist:
          return True
       elif x.artist == y.artist:
          if x.title < y.title:
             return True
          elif x.title == y.title:
             if x.number < y.number:
                return True
             else:
                return False
          else:
             return False
       else:
          return False
   else:
      return False
#data definition: new and old are both lists
    #returns list
# list list -> list
#puts new songs into old song list
def add_list(new, old):
   count = length(old)
   for temp in new:
      temp = temp.rstrip('\n').split('--')
      if length_song(temp) == 3:
         new_song = Song(temp[0], temp[1], temp[2])
         new_song.number = count
         old = add(old, count, new_song)
         count+=1
   return old

#data definition: takes in file1 which is a list of Songs,
    #accum is a string
    #returns one of
        #recursion - menu(file, accum)
        #quit()
#list str -> function
#menu for what to do with the song
def menu(file1, accum=''):
   if accum == 'number':
      file1 = sort(file1, less_than_number)
   if accum == 'title':
      file1 = sort(file1, less_than_title)
   if accum == 'artist':
      file1 = sort(file1, less_than_artist)
   if accum == 'album':
      file1 = sort(file1, less_than_album)
   print('\nSong Catalog')
   print('   1) Print Catalog')
   print('   2) Song Information')
   print('   3) Sort')
   print('   4) Add Songs')
   print('   0) Quit')
   try:
      x = int(input('Enter selection: '))
   except:
      print("\nInvalid option")
      menu(file1, accum)

   if x ==1:
      foreach(file1, print_)
      menu(file1, accum)
   if x ==2:
      try:
         y = int(input('Enter song number: '))
      except:
         print("\nInvalid option")
         menu(file1, accum)
      info(file1, y)
      menu(file1, accum)
   if x == 3:
      print('\nSort songs')
      print('   0) By Number')
      print('   1) By Title')
      print('   2) By Artist')
      print('   3) By Album')
      try:
         y = int(input('Sort by: '))
      except:
         print("\n... Invalid sort option")
         menu(file1, accum)
      if y == 0:
         file1 = sort(file1, less_than_number)
         menu(file1, 'number')
      if y == 1:
         file1 = sort(file1, less_than_title)
         menu(file1, 'title')
      if y == 2:
         file1 = sort(file1, less_than_artist)
         menu(file1, 'artist')
      if y == 3:
         file1 = sort(file1, less_than_album)
         menu(file1, 'album')
      else:
         print("\n... Invalid sort option")
         menu(file1)
   if x ==4:
      try:
         new_file = str(input('Enter name of file to load: '))
         newFile = open(new_file, 'r')
      except:
         print("\n'{}': No such file or directory".format(new_file))
         menu(file1, accum)
      file1 = add_list(newFile, file1)
      menu(file1, accum)
   if x ==0:
      quit()
   else:
      print("\n'%r': No such file or directory" % (x))
      menu(file1, accum)
# None -> function
#main function that gets called when name gets called
def main():
   inFile = open(argv[1], 'r')
   file1 = read(inFile)
   menu(file1)


if __name__ == '__main__':
   main()
