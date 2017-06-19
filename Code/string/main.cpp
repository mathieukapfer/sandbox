#include <string>
#include <stdio.h>

int main(int argc, char *argv[])
{
  std::string aString;

  aString = "pre";

  printf("my string:'%s' strat with %c",
         (std::string(aString + "body").c_str()),
         aString[0]
    );



  return 0;
}
