# a list
a_list = [1, 2, 3, 4, 'five']
a_tupple = (1, 2, 3)
a_set = {1, 2, 3}
a_dic = {1: 'one', 2: 'two'}

a_list.append('six')
a_tupple.count(3)
a_set.add(4)
a_dic.viewitems()

print("list:{}, tupple:{}, set:{}, dic:{}".
      format(a_list, a_tupple, a_set, a_dic))

# manipulate list
another_list = a_list       # this is not a copy !
another_list2 = a_list[:]   # this is a copy
another_list.append('777')  # change a_list too !
another_list2.append('888')

print (" a_list:        {}\n another_list:  {}\n another_list2: {}".
       format(a_list, another_list, another_list2))

# list comprehension
list_int = [item for item in a_list if type(item) == int]
print("list_int:       {}".format(list_int))
