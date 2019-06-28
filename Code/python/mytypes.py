# List
alist = ['this', 'is', 'a', 'list']
print "The first item of 'alist' is '%s'" % alist[0]
aNestedList = [[1,2,3], [4,5,6], [7,8,9]]
aMatrix = aNestedList
print aMatrix[0][0]

# Tupple
aTupple = 1,2,3
theSameTupplle = (1,2,3)
print "this is a tupple", aTupple
print "The first item of list is '%s'" % aTupple[0]

# String
astring = 'this is a string'
print astring
print "The first item of 'astring' is '%s'" % astring[0]
print "The first item of 'astring' is '{}'".format(astring[0])

# Set
aset = { 'this', 'is', 'the', 'first', 'set'}
a2ndset = {  'this', 'is', 'the', '2nd', 'set' }
print(aset - a2ndset)
print(aset | a2ndset)
print(aset & a2ndset)
print(aset ^ a2ndset)

# Dictionary
adic = { 1:'toto', 2:'zozo' }
print adic[1]
print adic[2]





