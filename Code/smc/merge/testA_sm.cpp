/*
 * ex: set ro:
 * DO NOT EDIT.
 * generated by smc (http://smc.sourceforge.net/)
 * from file : testA_sm.sm
 */

#include "TestA.h"
#include "testA_sm.h"

using namespace statemap;

namespace ProdA
{
    // Static class declarations.
    prod_Inside prod::Inside("prod::Inside", 0);
    prod_InsideState2 prod::InsideState2("prod::InsideState2", 1);
    common_Init common::Init("common::Init", 2);
    common_InsideSubmap common::InsideSubmap("common::InsideSubmap", 3);
    common_Finish common::Finish("common::Finish", 4);

    void TestAState::backToInit(testAContext& context)
    {
        Default(context);
        return;
    }

    void TestAState::goBack(testAContext& context)
    {
        Default(context);
        return;
    }

    void TestAState::goInside(testAContext& context)
    {
        Default(context);
        return;
    }

    void TestAState::goState2(testAContext& context)
    {
        Default(context);
        return;
    }

    void TestAState::leaveInit(testAContext& context)
    {
        Default(context);
        return;
    }

    void TestAState::Default(testAContext& context)
    {
        if (context.getDebugFlag() == true)
        {
            TRACE("TRANSITION   : Default\n\r");
        }

        throw (
            TransitionUndefinedException(
                context.getState().getName(),
                context.getTransition()));

        return;
    }

    void prod_Default::goBack(testAContext& context)
    {
        TestA& ctxt(context.getOwner());

        if (context.getDebugFlag() == true)
        {
            TRACE("LEAVING STATE   : prod::Default\n\r");
        }

        (context.getState()).Exit(context);
        if (context.getDebugFlag() == true)
        {
            TRACE("ENTER TRANSITION: prod::Default::goBack()\n\r");
        }

        context.clearState();
        try
        {
            ctxt.say("back to main");
            if (context.getDebugFlag() == true)
            {
                TRACE("EXIT TRANSITION : prod::Default::goBack()\n\r");
            }

            context.popState();
        }
        catch (...)
        {
            context.popState();
            throw;
        }

        return;
    }

    void prod_Inside::goState2(testAContext& context)
    {
        TestA& ctxt(context.getOwner());

        if (context.getDebugFlag() == true)
        {
            TRACE("LEAVING STATE   : prod::Inside\n\r");
        }

        (context.getState()).Exit(context);
        if (context.getDebugFlag() == true)
        {
            TRACE("ENTER TRANSITION: prod::Inside::goState2()\n\r");
        }

        context.clearState();
        try
        {
            ctxt.say("go to sub state 2");
            if (context.getDebugFlag() == true)
            {
                TRACE("EXIT TRANSITION : prod::Inside::goState2()\n\r");
            }

            context.setState(prod::InsideState2);
        }
        catch (...)
        {
            context.setState(prod::InsideState2);
            throw;
        }
        (context.getState()).Entry(context);

        return;
    }

    void common_Init::Entry(testAContext& context)

{
        TestA& ctxt(context.getOwner());

        ctxt.sayEntry("Common Init");
        return;
    }

    void common_Init::Exit(testAContext& context)

{
        TestA& ctxt(context.getOwner());

        ctxt.sayExit("Common Init");
        return;
    }

    void common_Init::goInside(testAContext& context)
    {

        if (context.getDebugFlag() == true)
        {
            TRACE("LEAVING STATE   : common::Init\n\r");
        }

        (context.getState()).Exit(context);
        if (context.getDebugFlag() == true)
        {
            TRACE("ENTER TRANSITION: common::Init::goInside()\n\r");
        }

        if (context.getDebugFlag() == true)
        {
            TRACE("EXIT TRANSITION : common::Init::goInside()\n\r");
        }

        context.setState(common::InsideSubmap);

        (context.getState()).Entry(context);
        context.pushState(prod::Inside);
        (context.getState()).Entry(context);

        return;
    }

    void common_Init::leaveInit(testAContext& context)
    {

        if (context.getDebugFlag() == true)
        {
            TRACE("LEAVING STATE   : common::Init\n\r");
        }

        (context.getState()).Exit(context);
        if (context.getDebugFlag() == true)
        {
            TRACE("ENTER TRANSITION: common::Init::leaveInit()\n\r");
        }

        if (context.getDebugFlag() == true)
        {
            TRACE("EXIT TRANSITION : common::Init::leaveInit()\n\r");
        }

        context.setState(common::Finish);
        (context.getState()).Entry(context);

        return;
    }

    void common_InsideSubmap::Entry(testAContext& context)

{
        TestA& ctxt(context.getOwner());

        ctxt.sayEntry("InsideSubmap");
        return;
    }

    void common_InsideSubmap::Exit(testAContext& context)

{
        TestA& ctxt(context.getOwner());

        ctxt.sayExit("InsideSubmap");
        return;
    }

    void common_Finish::backToInit(testAContext& context)
    {

        if (context.getDebugFlag() == true)
        {
            TRACE("LEAVING STATE   : common::Finish\n\r");
        }

        (context.getState()).Exit(context);
        if (context.getDebugFlag() == true)
        {
            TRACE("ENTER TRANSITION: common::Finish::backToInit()\n\r");
        }

        if (context.getDebugFlag() == true)
        {
            TRACE("EXIT TRANSITION : common::Finish::backToInit()\n\r");
        }

        context.setState(common::Init);
        (context.getState()).Entry(context);

        return;
    }
}

/*
 * Local variables:
 *  buffer-read-only: t
 * End:
 */