#include <iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <thread>
#include <chrono>

template<typename T>
class Singleton
{
public:
	static T* Instance() 
	{
		if(_instance == nullptr){
			_instance = new T;
		}
		return _instance;
	}
protected:
	Singleton() = default;
	~Singleton() = default;
	
	Singleton(const Singleton&)				= delete;
	Singleton& operator=(const Singleton&)	= delete;

private:
	static T* _instance;
};


template <typename T>
T* Singleton<T>::_instance = nullptr;

// forward declaration of Statemachine
class StateMachine;

enum class Event
{
	Initial,
	SelfTestOk,
	SelfTestFailed,
	Error,
	Initialized,
	Restart,
	Exit,
	ready,
    Configure,
    ConfigurationEnded,
    Stop,
    Resume,
    Start,
    Run,
    Suspend,

};

/*
entering the classes,  should automatically start given functions
, if all passes it triggers the .dispatch in the statemachine.
this triggers the next to be State* next = current->handleEvent(e);

when a child overrides the State* handleEvent() function, it must return
either the this pointer
or a reference to a new object which will move the cycle on

back in the statemachine this will trigger the onExit() function due to
next != current, this will delete the current state and set the current to the next.
This is necessary to avoid memory leaks. lastly it will trigger the onEntry() event again
and cycle continues.
*/
class State
{
public:
	virtual State* handleEvent(StateMachine& sm, Event e) = 0;
	/*
	after creation of object, state machie calls onEntry as overriden by children*/
	virtual void onEntry(StateMachine& sm) {};

	virtual void onExit(StateMachine& sm) {};
	virtual ~State() = default;

};

// inline std::ostream& operator<<(std::ostream& os, const StateMachine& sm)
// {
    
// }


inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
    switch (e)
    {
    case Event::Initial:                    return os <<"Event::Initial";
	case Event::SelfTestOk:                 return os <<"Event::SelfTestOk";
	case Event::SelfTestFailed:             return os <<"Event::SelfTestFailed";
	case Event::Error:                      return os <<"Event::Error";
	case Event::Initialized:                return os <<"Event::Initialized";
	case Event::Restart:                    return os <<"Event::Restart";
	case Event::Exit:                       return os <<"Event::Exit";
    case Event::ready:                      return os <<"Event::Ready";
    case Event::Configure:                  return os <<"Event::Configure";
    case Event::ConfigurationEnded:         return os <<"Event::ConfigurationEnded";
    case Event::Stop:                       return os <<"Event::Stop";
    case Event::Resume:                     return os <<"Event::Resume";
    case Event::Start:                      return os <<"Event::Start";
    case Event::Run:                        return os <<"Event::Run";
    case Event::Suspend:                    return os <<"Event::Suspend";
    default:                                return os <<"Event::<UNKNOWN";
    }
}

class StateMachine
{

private:
	State* current;

public:
    StateMachine() : current(nullptr) {}
    //avoid implicit conversions. compiler would make State*->Statemachine
	explicit StateMachine(State* initialState) : current(initialState)
	{
		if(current)
			current->onEntry(*this); // the initial state at creation
	}

	void dispatch(Event e)
	{
		/*sequence explained in State.h*/
        std::cout << "[DISPATCH]:["<< e << "]" << std::endl;
		State* next = current->handleEvent(*this,e); 
		if (next != current) 
		{

			//use of this
			//this -> pointer to the object itself
			//*this -> reference to the statemachine
			current->onExit(*this);
			// delete current;  //this might be a fix to the segmentation fault
			current = next;
			if (current)
			{
				current->onEntry(*this);
			}
		} //a condition to check if next and current is the same can be set here. 
        // else the SM will set the end of execution here.
        //exeuction delay here
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
};

// operator overload for =



//ALL META CONSTRUCTIONS SHOULD BE COMPLETED AT THIS POINT
//__________________________________________________
// STATE CLASSES
/*
initials:

    PowerOnSelfTest
    Initializing
    Failure

oprationals:


real time loop
*/

//forward declaration of outer classes
class PowerOnSelfTest;
class Failure;
class Initializing;
class Operational;


//forward declaration of inner classes
class Ready;
class Configuration;
class Suspended;
class RealTimeLoop;


//class declarations no, method bodies

class PowerOnSelfTest : public State
{
    friend class Singleton<PowerOnSelfTest>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine& sm, Event e) override;

private:
    PowerOnSelfTest() = default;
    ~PowerOnSelfTest() override = default;

    PowerOnSelfTest(const PowerOnSelfTest&) = delete;
    PowerOnSelfTest& operator=(const PowerOnSelfTest&) = delete;

    int systemSelfTest();
    int ErrorNo = 0;
    bool testResult = false;
};

class Failure : public State
{
    friend class Singleton<Failure>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine& sm, Event e) override;
    void incCount();

private:
    Failure() = default;
    ~Failure() override = default;

    Failure(const Failure&) = delete;
    Failure& operator=(const Failure&) = delete;

    void display(int ErrorNo);
    int FailCount =0;
};


class Initializing : public State
{
    friend class Singleton<Initializing>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine& sm, Event e) override;
    

private:
    Initializing() = default;
    ~Initializing() override = default;

    Initializing(const Initializing&) = delete;
    Initializing& operator=(const Initializing&) = delete;

    void startInitializing();
};


class Operational : public State
{
    friend class Singleton<Operational>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine& sm, Event e) override;

private:
    Operational() = default;
    ~Operational() override = default;

    Operational(const Operational&) = delete;
    Operational& operator=(const Operational&) = delete;

    bool restartCondition = false;
    StateMachine opFsm; //internal FSM, is invoked when initialized event is called
};


/*
this class lacks the SYSTEM EXIT FUNCTION*/


/*
OPERATIONAL FSM DECLARATIONS
*/
class Ready : public State
{
    friend class Singleton<Ready>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine &sm, Event e) override;

private:
    Ready() = default;
    ~Ready() override = default;

    Ready(const Ready&) = delete;
    Ready& operator=(const Ready&) = delete;
};
class Configuration : public State
{
    friend class Singleton<Configuration>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine &sm, Event e) override;
    void readConfigurationInfo();

private:
    Configuration() = default;
    ~Configuration() override = default;

    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;
};
class Suspended : public State
{
    friend class Singleton<Suspended>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine &sm, Event e) override;

private:
    Suspended() = default;
    ~Suspended() override = default;

    Suspended(const Suspended&) = delete;
    Suspended& operator=(const Suspended&) = delete;
};

class RealTimeLoop : public State
{
    friend class Singleton<RealTimeLoop>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine &sm, Event e) override;

private:
    RealTimeLoop() = default;
    ~RealTimeLoop() override = default;

    RealTimeLoop(const RealTimeLoop&) = delete;
    RealTimeLoop& operator=(const RealTimeLoop&) = delete;
    

};








// IMPLEMENTATIONS

void Failure::incCount()
{
    FailCount++;
}
void Failure::onEntry(StateMachine& sm) 
{
    incCount();
    std::cout << "[FUNC]:[onENTRY]:Failure: #" << FailCount << std::endl;
    if(FailCount < 10)
    {
        sm.dispatch(Event::Restart);
    }
    
}
void Failure::onExit(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onEXIT]:Failure" << std::endl;
}

State* Failure::handleEvent(StateMachine& sm, Event e) 
{
    switch (e)
    {
    case Event::Restart:
        return Singleton<PowerOnSelfTest>::Instance();
        break;
    

    default:
        std::cout << "invalid state request" << std::endl;
        return this;
    }
    return this;
}
void Failure::display(int ErrorNo) { std::cout << "Failed: error#:" << ErrorNo << std::endl; }




//INITIALIZING
void Initializing::onEntry(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onENTRY]:Initializing" << std::endl;
    startInitializing();
    sm.dispatch(Event::Initialized);
}
void Initializing::onExit(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onExit]:Initializing" << std::endl;

}
State* Initializing::handleEvent(StateMachine& sm, Event e) 
{
    switch (e)
    {
    case Event::Initialized:
        return Singleton<Operational>::Instance();
        break;
    default:
        std::cout << "invalid state request" << std::endl;
        return this;
    }
    return this;
}

void Initializing::startInitializing() {
    std::cout << "starting initialization ..." << std::endl;
    std::cout << "initialized." << std::endl;
}


//OPERATIONAL
void Operational::onEntry(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onENTRY]:Operational" << std::endl;
    
    if(restartCondition)
    {
        sm.dispatch(Event::Restart);
    }
    /*SPIN UP INTERNAL FSM*/
    opFsm = StateMachine(Singleton<Ready>::Instance());
    //BE WARNED, FSM TRANSITION IS HAPPENING AUTOMATICALLY.   
}

void Operational::onExit(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onEXIT]:Operational" << std::endl;
}
State* Operational::handleEvent(StateMachine& sm, Event e) 
{
    switch (e)
    {

    case Event::Restart:
        return Singleton<PowerOnSelfTest>::Instance();
        break;
    
    default:
        
        opFsm.dispatch(e);
        return this;
    }
}




void PowerOnSelfTest::onEntry(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onENTRY]:PowerOnSelfTest" << std::endl;
    this->ErrorNo = systemSelfTest();
    sm.dispatch(ErrorNo == 0 ?
        Event::SelfTestOk :
        Event::SelfTestFailed);
    
}
void PowerOnSelfTest::onExit(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onEXIT]:PowerOnSelfTest" << std::endl;
}
State* PowerOnSelfTest::handleEvent(StateMachine& sm, Event e) 
{
    switch (e)
    {
    case Event::SelfTestOk:
        return Singleton<Initializing>::Instance();
        break;
    case Event::SelfTestFailed:
        return Singleton<Failure>::Instance();
        break;

    
    default:
        std::cout << "invalid state request" << std::endl;
        return this;
    }
}


int PowerOnSelfTest::systemSelfTest() {
    return (std::rand() % 10 == 0) ? 42 : 0;
    // return 1; //test, failure mode
}


//OPERATIONAL INNER FSM IMPLEMENTATION_____________________________________
    //THESE CLASSES ARE STILL PART OF EVENT ENUM, BUT ARE MAINTAINED BY opFsm
    //READY
    void Ready::onEntry(StateMachine& sm) {
        std::cout<<"[FUNC]:[onENTRY]:Ready" << std::endl;
        // sm.dispatch(Event::Configure);
    }
    void Ready::onExit(StateMachine& sm)
    {
        std::cout <<"[FUNC]:[onEXIT]:Ready" << std::endl;
    } 
    State*Ready:: handleEvent(StateMachine &sm, Event e) 
    {
        switch (e)
        {
        case Event::Configure:
            return Singleton<Configuration>::Instance();
            break;
        case Event::Start:
        case Event::Run:
            return Singleton<RealTimeLoop>::Instance();
        default:
            std::cout << "invalid state request" << std::endl;
            return this;
            break;
        }
    }

    //CONFIGURATION
    void Configuration::onEntry(StateMachine& sm)
    {
        std::cout<<"[FUNC]:[onENTRY]:configuration" << std::endl;
        readConfigurationInfo();
        // sm.dispatch(Event::ConfigurationEnded);
    }
    void Configuration::onExit(StateMachine& sm)
    {
        std::cout << "[FUNC]:[onEXIT]:configuration" << std::endl;
    }
    State* Configuration::handleEvent(StateMachine &sm, Event e)
    {
        switch (e)
        {
        case Event::ConfigurationEnded:
            return Singleton<Ready>::Instance();
            break;
        default:
            std::cout << "invalid state request" << std::endl;
            return this;
            break;
        }
    } 
    void Configuration::readConfigurationInfo()
    {
        std::cout << "---CONFIGURATION INFO---\n";
        std::cout << "somebullshit info" << std::endl;
    }


    //SUSPENDED
    void Suspended::onEntry(StateMachine& sm)
    {
        std::cout<<"[FUNC]:[onENTRY]:Suspended" << std::endl;
        // sm.dispatch(Event::Resume);
    } 
    void Suspended::onExit(StateMachine& sm)
    {
        std::cout<<"[FUNC]:[onEXIT]:Suspended" << std::endl;
    } 
    State* Suspended::handleEvent(StateMachine &sm, Event e)
    {
        switch (e)
        {
        case Event::Resume:
            return Singleton<RealTimeLoop>::Instance();
            break;
        
        default:
            std::cout << "invalid state request" << std::endl;
            return this;
            break;
        }
    }

    //REAL TIME LOOP
    void RealTimeLoop::onEntry(StateMachine& sm) 
    {
        std::cout<<"[FUNC]:[onENTRY]:RealTimeLoop" << std::endl;
        // sm.dispatch(Event::Stop);
    }
    void RealTimeLoop::onExit(StateMachine& sm) 
    {
        std::cout<<"[FUNC]:[onEXIT]:RealTimeLoop" << std::endl;
    }
    State* RealTimeLoop::handleEvent(StateMachine &sm, Event e) 
    {
        switch (e)
        {
        case Event::Stop:
            return Singleton<Ready>::Instance();
            break;
        
        case Event::Suspend:
            return Singleton<Suspended>::Instance();
            break;
        default:
            std::cout << "invalid state request" << std::endl;
            return this;
            break;
        }
    }


// END OF IMPLEMENTATION



class EmbeddedSystemX
{
public:
    EmbeddedSystemX() : sm(Singleton<PowerOnSelfTest>::Instance()){} 



    //public interfaces

    void SelfTestOk()               {sm.dispatch(Event::SelfTestOk);}
    void SelftestFailed()           {sm.dispatch(Event::SelfTestFailed);}
    void Error()                    {sm.dispatch(Event::Error);}
    void Initialized()              {sm.dispatch(Event::Initialized);}
    void Restart()                  {sm.dispatch(Event::Restart);}
    void Exit()                     {sm.dispatch(Event::Exit);}
    void ready()                    {sm.dispatch(Event::ready);}
    void Configure()                {sm.dispatch(Event::Configure);}
    void ConfigurationEnded()       {sm.dispatch(Event::ConfigurationEnded);}
    void Stop()                     {sm.dispatch(Event::Stop);}
    void Resume()                   {sm.dispatch(Event::Resume);}                   
    void Start()                    {sm.dispatch(Event::Start);}                  
    void Run()                      {sm.dispatch(Event::Run);}   
    void Suspend()                  {sm.dispatch(Event::Suspend);}             

private:
    StateMachine sm;
};

void printMenu()
{
    
    std::cout << "1 - configuration " << std::endl;
    std::cout << "2 - end configuration " << std::endl;
    std::cout << "3 - start " << std::endl;
    std::cout << "4 - stop" << std::endl;
    std::cout << "5 - suspend" << std::endl;
    std::cout << "6 - resume"<< std::endl;
    std::cout << "7 - restart"<< std::endl;
    std::cout << "8 - exit"<< std::endl;
}

int main(void)
{

    std::cout << "hello world" << std::endl;

    //rand cast for testing purposes
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // StateMachine sm(Singleton<PowerOnSelfTest>::Instance());
    EmbeddedSystemX system;
    std::cout << "System is operational:" << std::endl;

    
    
    
    int cmd{};
    
    while(true)
    {   
        printMenu();
        std::cout << ">";
        if(!(std::cin>> cmd))
            break;
            
        switch (cmd)
        {
        case 1: 
            system.Configure();
            break;
        case 2:
            system.ConfigurationEnded();
            break;
        case 3:
            system.Start();
            break;
        case 4:
            system.Stop();
            break;
        case 5:
            system.Suspend();
            break;
        case 6:
            system.Resume();
            break;
        case 7:
            system.Restart(); 
            break;
        case 8:
            system.Exit();
            break;
        default:
            break;
        }
    }
    return 1;
}


/*
STATUS:

FAILURE -> EXIT STILL TODO


CURRENTLY:
OPERATIONAL SUPERSTATE


RESTART IN READY CLASS NEEDS TO BE IMPLEMENTED; so THAT SUSPENDED AND 
REALTIMELOOP CAN CALL IT

23-11-25_23:51
    INNER FSM IS IMPLEMENTED, NOT TESTED
    program executes, however gets stuck in inner FSM loop. this is okay fow now[done]
    

    next is creating an iteration limit, afterwards defined what configuration is

    then create the backwards transition to outer FSM.

    lastly create the active object pattern.


24-11-25_10:29
    add user command sequence to inner state (requires removal of automatic dispatch in every inner states
    onEntry())

    - solution is as above, remove the automatic dispatch, replace them with 
    the API calls to embeddedsysx, since theyre all defined.
    this now drives the changes.
    this should work, as long as the state* eventhandler in every class
    is designed to move the state forward and return the correct object. 
    (check)

    -make the current state options for any given state visual
        - implement ostream operator<< for statemachine
        so that it prints the current state.

*/