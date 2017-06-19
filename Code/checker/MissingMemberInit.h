
class MissingMemberInit {
 public:
  // cpplint: MissingMemberInit.h:5:  Single-parameter constructors should be marked explicit.  [runtime/explicit] [4]
  MissingMemberInit(int a);
  virtual ~MissingMemberInit() {};

  int _a_param;
};
