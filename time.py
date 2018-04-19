import timeit
import linked_list
import random

import sys
sys.setrecursionlimit(10100)


# print out the timing results
def print_timing(desc, iterations, seconds):
   print('{}\n\t{} iterations in {} seconds'.format(desc, iterations, seconds))


# build a list for this test ... the type of list is passed as module
def build_list(n, module, max=10000):
   list = module.empty_list()
   for pos in range(n):
      list = module.add(list, pos, random.randrange(max))

   return list


# the function passed to sort must take an argument, but we
# don't really want to do anything with it for this experiment
def noop(value):
   pass


# timeit expects that the function passed will take no arguments, so
# this function gathers the arguments and returns a new function that
# uses them, but that itself does not take any arguments
def build_operation(list):
   def run_sort():
      linked_list.add(list, 0, random.randrange(10000))
      linked_list.sort(list, linked_list.less_than)
   return run_sort


def run_one_experiment(num_elements, num_iterations, module):
   list = build_list(num_elements, module)
   to_run = build_operation(list)
   seconds = timeit.timeit(to_run, number=num_iterations)

   print_timing('{}.sort identity: {} elements'.format(module.__name__,
      num_elements), num_iterations, seconds)

# let's try just one experiment for now
run_one_experiment(900, 1000, linked_list)
