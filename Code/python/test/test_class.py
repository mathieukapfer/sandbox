class MyClass(object):
    """ My first class """
    nb_instances = 0

    def __init__(self,name):
        MyClass.nb_instances += 1
        print("create an instance")
        self.name = 0


myInstance = MyClass("toto")
myInstance2 = MyClass("titi")

print MyClass.nb_instances
