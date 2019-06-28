
extern "C" {
/* definition */
  /*
  int _exit (int status) { return 0;}
  int _sbrk (int) { return 0;}
  */
}

#if 1
class Toto {
 public:
  Toto(int val):_val(val) {};
  /*virtual*/ ~Toto() {};
 private:
 int _val;
 int _tab[1024];
};
#endif


#if 1
static Toto global_static_obj(0xCAFE);     // #51966
static Toto global_static_obj2(0xFECA);    // #65226
Toto global_obj(0x1234);
static int static_int = 0xABCD; // #43981
static int static_int_uninitialized;
int global_int_uninitialized;

const char msg[] ="this is a rodata section";
int global_int = 0x1234;
#endif

int main(int argc, char *argv[]) {
static Toto local_static_obj2(0xFECA);    // #65226
Toto local_obj(0);
  return 0;
}


/* sample of code with asm

  __asm__(
    "    bl    __libc_init_array \n"
    );

*/
