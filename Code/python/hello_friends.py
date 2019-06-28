friends = ['toto', 'titi', 'tata']

# print the list
print "hello %s" % friends

# =====================================

for i,name in enumerate(friends):
    # C like string formating => arg is in a tupple
    print '%d:hello %s' % (i, name)
    print ('%d:hello %s' % (i, name))
    # another string formating
    print "{it} hello {name}".format(it=i, name=name)
    

# =====================================

def hello(name):
    # call fct with multiple arg
    print "hello", name
    # C like string formating => arg is in a tupple
    print 'hello %s' % name
    print "hello %s" % name
    print ("hello %s" % name)
    print 'hello', name

hello("momo")
hello(friends[2])
hello(friends)
