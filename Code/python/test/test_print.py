for var in range(10):
    print var

a_list = [1, 2, 3, 4, '5']

for var in a_list:
    print ("a_list %s" % var)

a_dic = {1: 'one', 2: 'two', 'pi': 3.1415926}

for var in a_dic:
    # old way: may failed if wrong type is provide
    # print ("[old way ] a dic %d : %s" % (var, a_dic[var]))
    # new way: no fail + optional positioning
    print ("[new way ] a dic {} : {}".format(var, a_dic[var]))
    # new way: + optional positioning
    print ("[new way2] a dic {0} : {1}".format(var, a_dic[var]))

print ("pi:{0:1.2f}".format(a_dic['pi']))
print ("pi:{0:09d}".format(int(a_dic['pi'])))
