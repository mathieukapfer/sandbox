#ifndef _H_TESTB_SM
#define _H_TESTB_SM

/*
 * ex: set ro:
 * DO NOT EDIT.
 * generated by smc (http://smc.sourceforge.net/)
 * from file : testB_sm.sm
 */


#include <statemap.h>

namespace ProdB
{
    // Forward declarations.
    class prod;
    class prod_Inside;
    class prod_InsideState2;
    class prod_Default;
    class common;
    class common_Init;
    class common_InsideSubmap;
    class common_Finish;
    class common_Default;
    class TestBState;
    class testBContext;
    class TestB;

    class TestBState :
        public statemap::State
    {
    public:

        TestBState(const char *name, int stateId)
        : statemap::State(name, stateId)
        {};

        virtual void Entry(testBContext&) {};
        virtual void Exit(testBContext&) {};

        virtual void backToInit(testBContext& context);
        virtual void goBack(testBContext& context);
        virtual void goInside(testBContext& context);
        virtual void goState2(testBContext& context);
        virtual void leaveInit(testBContext& context);

    protected:

        virtual void Default(testBContext& context);
    };

    class prod
    {
    public:

        static prod_Inside Inside;
        static prod_InsideState2 InsideState2;
    };

    class prod_Default :
        public TestBState
    {
    public:

        prod_Default(const char *name, int stateId)
        : TestBState(name, stateId)
        {};

        virtual void goBack(testBContext& context);
    };

    class prod_Inside :
        public prod_Default
    {
    public:
        prod_Inside(const char *name, int stateId)
        : prod_Default(name, stateId)
        {};

        void goState2(testBContext& context);
    };

    class prod_InsideState2 :
        public prod_Default
    {
    public:
        prod_InsideState2(const char *name, int stateId)
        : prod_Default(name, stateId)
        {};

    };

    class common
    {
    public:

        static common_Init Init;
        static common_InsideSubmap InsideSubmap;
        static common_Finish Finish;
    };

    class common_Default :
        public TestBState
    {
    public:

        common_Default(const char *name, int stateId)
        : TestBState(name, stateId)
        {};

    };

    class common_Init :
        public common_Default
    {
    public:
        common_Init(const char *name, int stateId)
        : common_Default(name, stateId)
        {};

        void Entry(testBContext&);
        void Exit(testBContext&);
        void goInside(testBContext& context);
        void leaveInit(testBContext& context);
    };

    class common_InsideSubmap :
        public common_Default
    {
    public:
        common_InsideSubmap(const char *name, int stateId)
        : common_Default(name, stateId)
        {};

        void Entry(testBContext&);
        void Exit(testBContext&);
    };

    class common_Finish :
        public common_Default
    {
    public:
        common_Finish(const char *name, int stateId)
        : common_Default(name, stateId)
        {};

        void backToInit(testBContext& context);
    };

    class testBContext :
        public statemap::FSMContext
    {
    public:

        testBContext(TestB& owner)
        : FSMContext(common::Init),
          _owner(owner)
        {};

        testBContext(TestB& owner, const statemap::State& state)
        : FSMContext(state),
          _owner(owner)
        {};

        virtual void enterStartState()
        {
            getState().Entry(*this);
            return;
        }

        TestB& getOwner() const
        {
            return (_owner);
        };

        TestBState& getState() const
        {
            if (_state == NULL)
            {
                throw statemap::StateUndefinedException();
            }

            return (dynamic_cast<TestBState&>(*_state));
        };

        void backToInit()
        {
            setTransition("backToInit");
            (getState()).backToInit(*this);
            setTransition(NULL);
        };

        void goBack()
        {
            setTransition("goBack");
            (getState()).goBack(*this);
            setTransition(NULL);
        };

        void goInside()
        {
            setTransition("goInside");
            (getState()).goInside(*this);
            setTransition(NULL);
        };

        void goState2()
        {
            setTransition("goState2");
            (getState()).goState2(*this);
            setTransition(NULL);
        };

        void leaveInit()
        {
            setTransition("leaveInit");
            (getState()).leaveInit(*this);
            setTransition(NULL);
        };

    private:

        TestB& _owner;
    };
}


/*
 * Local variables:
 *  buffer-read-only: t
 * End:
 */

#endif // _H_TESTB_SM
