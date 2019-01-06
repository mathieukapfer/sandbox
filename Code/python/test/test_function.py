def my_first_function():
    """ This is my first function"""
    print("hello")

my_first_function()

# with arguments
def say_hello(a, b):
    """ Use argumente """
    print("hello {} {}".format(a,b))

say_hello('mathieu','kapfer')

# with default argument
def say_hello2(first_name, optional_last_name=''):
    """ Use optional arguments """
    print("hello {} {}".format(first_name, optional_last_name))

say_hello2("math")
say_hello2("math","kapf")
say_hello2(first_name="bob")

# with list as arguments
def say_hello3(names):
    msg = 'hello '
    for name in names:
        msg += '{}, '.format(name)
    print msg

names = ['bob', 'toto', 'jon', 'tata' ]
say_hello3(names)

# with variable number of arguements
def say_hello4(*names):
    msg = 'Hellooooo '
    for name in names:
        msg += name + ' '
    print msg

say_hello4('lolo', 'zozo', 'rototo')

# with variable number of key arguements
def say_hello4(name, **user_info):
    msg="hello {}, this is your info:".format(name)
    for key,value in user_info.items():
        msg+="{}:{}, ".format(key, value)
    print msg

say_hello4(name='toto', age=12, job='hacker')

print vars()


# Question
#  - global variable ? execution in same shell ?
#  - surcharge
