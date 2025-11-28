#include <iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <thread>
#include <chrono>
#include <future>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>
#include <vector>

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
    ChMode1,
    ChMode2,
    ChMode3,
    WaitModeSwitch

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
    case Event::ChMode1:                    return os <<"Event::ChMode1";
    case Event::ChMode2:                    return os <<"Event::ChMode2";
    case Event::ChMode3:                    return os <<"Event::ChMode3";
    case Event::WaitModeSwitch:             return os <<"Event::ModeSwitch";
    default:                                return os <<"Event::<UNKNOWN>";
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

    State* getCurrentState() const{return current;}
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


//forward declaration of active object 
// ACTIVE OBJECT declarations
//must containt (from client) -> (proxy)->scheduler-activation list-method request->servant 
//REAL TIME LOOP SERVANT  


class RTL_Servant {
public:
    enum class Mode {Mode1, Mode2, Mode3};
    RTL_Servant() : mode(Mode::Mode1) {/*sets initial mode to mode 1*/}

    //--> this is where ill do the actual work, unwind and return to where this function is called from
    void chMode1_impl() {
        mode = Mode::Mode1;
        std::cout << "[RTL_servant]:switched to mode 1" << std::endl;
    }
    void chMode2_impl() {
        mode = Mode::Mode2;
        std::cout << "[RTL_servant]:switched to mode 2" << std::endl;
    }
    void chMode3_impl() {
        mode = Mode::Mode3;
        std::cout << "[RTL_servant]:switched to mode 3" << std::endl;
    }

    Mode currentMode() const {return mode;}

private:
    Mode mode;
};

//REAL TIME LOOP REQUEST
class RTL_Request
{
public:
    /*
    there is dependencies, no preconditions, nothing is holding it back
    thus execution be done immidiately, FIFO style.
    */
    virtual bool can_run() const {return true;} 
    virtual void call() = 0; //pure virual, must be overridden
    virtual ~RTL_Request() = default;
};

// each concrete request owns a promise
//REQUEST FUNCTIONS
class ChMode1Req : public RTL_Request{
public:
    ChMode1Req(RTL_Servant& s, std::promise<void> p)
     : s(s), promise(std::move(p))
    {} 

    void call() override {   //--> this is where i call the servant to do the work -->(goto RTL_servant)
        s.chMode1_impl();
        //though the promise is <void> it still sets the shared "thread" state to "ready"
        //--> i've returned to say i've fullfilled the promise
        promise.set_value();
    }

private:
    RTL_Servant& s;
    std::promise<void> promise;
};
class ChMode2Req : public RTL_Request{
public:
    ChMode2Req(RTL_Servant& s, std::promise<void> p)
     : s(s), promise(std::move(p))
    {} 

    void call() override {
        s.chMode2_impl();
        promise.set_value();
    }

private:
    RTL_Servant& s;
    std::promise<void> promise;
};
class ChMode3Req : public RTL_Request{
public:
    ChMode3Req(RTL_Servant& s, std::promise<void> p)
     : s(s), promise(std::move(p))  /*use of move semantics to transfer pointer*/
    {} 

    void call() override {
        s.chMode3_impl();
        promise.set_value();
    }

private:
    RTL_Servant& s;
    std::promise<void> promise;
};

//ACTIVATION LIST / QUEUE

class RTL_ActivationQueue
{
public:
    void insert(std::unique_ptr<RTL_Request> req)
    {
        std::lock_guard<std::mutex> lock(mtex);
        que.push(std::move(req));
        cv.notify_one();
    }
    //this might be overkill
    std::unique_ptr<RTL_Request> remove()
    {
        //create unique pointer of RTL_requests
        std::unique_ptr<RTL_Request> req;

        //this might be super overkill
        //acquire the lock so the que can be used safely, no one else can push/pop
        std::unique_lock<std::mutex> lock(mtex);

        //make the thread sleep until either request is available or system is stopping
        //capture everything by reference, q and running
        cv.wait(lock, [&]{return !que.empty() || !running; });
        //if queue is not empty. undefined behaviour if empty queue is pop'd 
        if(!que.empty())
        {
            //take ownership of the first object in the queue 
            req = std::move(que.front());
            //and pop it, remove the first element;
            que.pop();
        }
        return req;
    }

    void set_running(bool status)
    {   
        //take the lock and lock it now, then automatically unlock it when the scope ends
        /*
        we create a temporary object named lock, that takes types of std::mutex
        the invoked constructor locks the mutex
        when the lock goes out of scope (it leaves this function body) the destructor
        is automatically invoked and the resources is released. */
        std::lock_guard<std::mutex> lock(mtex);
        //indicate we are working
        running = status;
        cv.notify_all();
    }

private:
    std::mutex mtex;
    std::queue<std::unique_ptr<RTL_Request>> que; //queue of type unique pointers of type RTL requests
    std::condition_variable cv; //avoid needles polling, uses notify one on the mutex to signal when ready
    bool running = true;
};

//___________________SCHEDULER____________________
class RTL_Scheduler{
public:
    RTL_Scheduler() = default; 

    void start()
    {
        /*
        start the queue
        set_running contains a lock that acquires the mutex
        "i own the mutex now, no other thread may pass"
        */
        queue.set_running(true);
        //-->spin up the threads with work from the queue
        /*
        lambda invocation
        [this] gives the lambda access to fields/functions from RTL_ActivationQueue
        the while, makes the thread execute as long as the object is alive. */
        worker = std::thread([this](){
            while(true)
            {
                //-->take the request from the queue
                auto req = queue.remove(); 
                if(!req) //if the queue is empty
                {
                    break;
                }
                if(req->can_run()) //-->if the request can be ran, ie. work can be done
                {   
                    //INVOCATION --> (goto) .call chMode1Req
                    req->call(); //this is of type unique_ptr<RTL_Request> 
                }
                
            }
        });
    }
    
    void stop()
    {
        //set the running flag to false
        queue.set_running(false);
        // join all threads if they are done working
        if(worker.joinable())
        {
            //joins the thread, terminates the processing.
            worker.join();
        }
    }


    void enqueue(std::unique_ptr<RTL_Request> req) 
    {
        //add/move work to the queue using insert.¨
        queue.insert(std::move(req));
    }

private:
    RTL_ActivationQueue queue;
    std::thread worker;
    
};



//___________________PROXY______________________
//the proxy must only expose the three methods required by the client
class RTL_Proxy{
public:
    // RTL_Proxy() = default;
    RTL_Proxy(
        RTL_Servant& servant,
        RTL_Scheduler& scheduler)
        : serv(servant), sched(scheduler) {}
    
    //templated type
    template<typename RequestT>
    std::future<void> makeModeRsq() { //function returns a std::future pointer 
        std::promise<void> prom; //declare a std::promise
        auto fut = prom.get_future(); //bridge the future to the promise
        /* -->
        in one foul swoop, allocate memory for RequestT type, with given arguments
        and wraps the object in a std::unique_ptr, without the use of new.
        In other words:
        A promise is created
        A future is extracted
        A request object is created --> this object determines
        - which servant to call (serv)
        - how to call it (through the future)
        - the promise of the work

        this is then sent to the queue by scheduler.
        */
        auto req = std::make_unique<RequestT>(serv, std::move(prom)); 
        //queue the request through the scheduler.
        sched.enqueue(std::move(req)); //--> sheduler thread is the run from RealTimeLoop::onEntry --> (goto)

        return fut;
    }

    //use template to create functions
    std::future<void> chMode1() { return makeModeRsq<ChMode1Req>(); } // --> invokes the makeModeRsq function and specifies what request -->(goto)
    std::future<void> chMode2() { return makeModeRsq<ChMode2Req>(); } 
    std::future<void> chMode3() { return makeModeRsq<ChMode3Req>(); } 



private:
    RTL_Servant& serv;
    RTL_Scheduler& sched;
};

// END OF ACTIVE OBJECT


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

    struct ModeConfig{
        std::vector<RTL_Servant::Mode> req_work;
    };
    ModeConfig& getConfigParams() {return configParams;}

private:
    Operational() = default;
    ~Operational() override = default;

    Operational(const Operational&) = delete;
    Operational& operator=(const Operational&) = delete;

    bool restartCondition = false;
    StateMachine opFsm; //internal FSM, is invoked when initialized event is called
    std::optional<std::future<void>> futureMode;
    ModeConfig configParams;  
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

//INCLUDES ACTIVE OBJECT 
class RealTimeLoop : public State
{
    friend class Singleton<RealTimeLoop>;
public:
    void onEntry(StateMachine& sm) override;
    void onExit(StateMachine& sm) override;
    State* handleEvent(StateMachine &sm, Event e) override;

    //AO interfaces futures
    // std::future<void> chMode1();
    // std::future<void> chMode2();
    // std::future<void> chMode3();

    RTL_Proxy& getProxy() {return proxy;}

private:
    RealTimeLoop();
    ~RealTimeLoop() override = default;

    RealTimeLoop(const RealTimeLoop&) = delete;
    RealTimeLoop& operator=(const RealTimeLoop&) = delete;

    
    //this could potentially be a nested statemachine aswell
    // enum class Mode{Mode1, Mode2, Mode3};
    // Mode currentMode = Mode::Mode1; //initial mode/state
    RTL_Servant servant;
    RTL_Scheduler scheduler;
    RTL_Proxy proxy;

    
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


//OPERATIONAL_______________________________________________________________
void Operational::onEntry(StateMachine& sm) 
{
    std::cout << "[FUNC]:[onENTRY]:Operational" << std::endl;
    
    if(restartCondition)
    {
        sm.dispatch(Event::Restart);
    }
    /*SPIN UP INTERNAL FSM*/
        //BE WARNED, FSM TRANSITION IS HAPPENING AUTOMATICALLY.   
    opFsm = StateMachine(Singleton<Ready>::Instance());
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

    case Event::ChMode1:
    case Event::ChMode2:
    case Event::ChMode3:
    {
        State* curr = opFsm.getCurrentState();
        //dynamic_cast<> runtime type check
        //"is the actual object point to by curr of type RealTimeLoop or a derivative thereof"
        //returns a RealTimeLoop* if true else nullptr
        if(auto* rtl = dynamic_cast<RealTimeLoop*>(curr))
        {
            RTL_Proxy& proxy = rtl->getProxy();
        
            if(e == Event::ChMode1)
                futureMode = proxy.chMode1(); 
            if(e == Event::ChMode2)
                futureMode = proxy.chMode2(); 
            if(e == Event::ChMode3)
                futureMode = proxy.chMode3(); 
        }else{
            std::cout <<"[OPERATIONAL]:EVENT MODE IGNORED" << std::endl;
        }         
        return this;
    }
    case Event::WaitModeSwitch:
    {
        if(futureMode.has_value())
        {
            std::cout <<"[OPERATIONAL]:reading future:" << std::endl;
            futureMode->get();
            std::cout <<"[OPERATIONAL]:mode change complete" << std::endl;
            futureMode.reset();
        }else{
            std::cout << "[OPERATIONAL]: NO FUTURE TO BEHOLD!" << std::endl;
        }
        return this; //were not changing state, only reading the future, like a freaking seer
        
    }

    //this is doing the heavy lifting
    default:
        opFsm.dispatch(e); //any changes to inner statemachine is done through this dispatch!
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
    //THESE CLASSES ARE STILL PART OF EVENT ENUM, BUT ARE MAINTAINED BY opFsm (operational finite statemachine)
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
        sm.dispatch(Event::ConfigurationEnded);
        
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
        std::cout << "--- request work to be done" <<std::endl;
        
        auto* op = Singleton<Operational>::Instance();

        int cmd{};
        while(cmd != 99)
        {
            std::cout << "select work to be done\n";
            std::cout << "1 - MODE1\n";
            std::cout << "2 - MODE2\n";
            std::cout << "3 - MODE3\n";
            std::cout << "3 - end config" << std::endl;
            if(!(std::cin>> cmd))
                break;
            switch (cmd)
            {
            case 1:
                op->getConfigParams().req_work.push_back(RTL_Servant::Mode::Mode1); 
                break;
            case 2:
                op->getConfigParams().req_work.push_back(RTL_Servant::Mode::Mode2);
                break;
            case 3:
                op->getConfigParams().req_work.push_back(RTL_Servant::Mode::Mode3);
                break;
            case 99:
                std::cout << "ending config..." << std::endl;
            default:
                std::cout << "invalid work request" << std::endl;
                break;
            }
        }
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
    RealTimeLoop::RealTimeLoop() : servant(), scheduler(), proxy(servant, scheduler) 
    {}
    void RealTimeLoop::onEntry(StateMachine& sm) 
    {
        std::cout<<"[FUNC]:[onENTRY]:RealTimeLoop" << std::endl;
        //--> when the RTS is entered, start the queue

        scheduler.start(); //--> when the scheduler starts, a thread is spun up -->(goto scheduler) 
    }
    void RealTimeLoop::onExit(StateMachine& sm) 
    {
        std::cout<<"[FUNC]:[onEXIT]:RealTimeLoop" << std::endl;
        //when we exit, stop the queue and join all active threads
        scheduler.stop();
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


// END OF IMPLEMENTATION - INNER







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
    // int VersionNO = 1
    // std::string = "Ver1.1A"

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
        case 9:
            
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

25-11-25_20:43  (jeg var syg i dag, bær over med mig)
    Active object seems to give meaning now

    When we enter the State "RealTimeLoop" this a state in the operational machine, however we must
    discern from "straight" state changes to future based state changes.  
    The core idea is now that the events chModeN which drives the changes are now delivered by the 
    Active object. The whole "thing" inside RealTimeLoop is a FSM in it self however the changes
    are delivered by the active object pattern, meaning that we the client, looking into RealTimeloop(RLT)
    are requesting state changes to the machine inside RTL, but we are promised those changes, thus
    they might not happen instantaneously.

    An important question to raises here is, logically the state requests are sequential, looking
    at the diagram only Mode3 can be the next state after Mode2, however that is not the purpose of the
    active object - "Eeven if the state machien only allows valid transitions, it does NOT mean that the
    client cannot issues invalid or redudant requests".

    ex:
    state = mode1

    client requests
    chMode2 (valid)
    chMode2 (invalid)
    chMode3 (invalid)
    chMode1 (invalid)
    


    also it is important to note, we request events, not states. 
    Because the statemachine itself decides which states it wants to go to based on the received events.


     - In REALTIMELOOP declaration, consider if mode should be a statemachine instead
     of a enum class



26-11-25_12:24
    specified scheduler, explained the code to ewoud and jakob


26-11-25_23:30~~   
    scheduler implemented, moving to client implementation
    added various comments.

27-11-25_12:30
    all AO subparts are implemented.
    next is exposing the interface.

    interface is implemented, i've made a fun little follow the arrow (-->) to see the sequence of execution
    CLIENT (Operational)
    |
    | 1) proxy.chMode1()
    v
    RTL_Proxy
    |
    | 2) create promise + future
    | 3) create ChMode1Req(servant, promise)
    | 4) enqueue request
    v
    RTL_Scheduler THREAD
    | 
    | 5) remove request from queue
    | 6) check can_run()
    | 7) call req->call()
    v
    RTL_Request (ChMode1Req)
    |
    | 8) servant.chMode1_impl()
    | 9) promise.set_value()
    v
    RTL_Servant
    |
    | 10) updates internal mode
    v
    FUTURE (in client)
    |
    | 11) becomes ready
    v
    CLIENT(operational) can now fut.get()



    still contemplating where to put .get()


28-11-25_12:10
    implementation of the exposed interface

    i assume the work done, is to be set in the configuration state, thus a slight rework might be necessary.

    still missing that restart feature from any operational state LULE


    everything compiles, however if one just enters 


    if one is confused about how the inner states are translated through the statemachine in main, it
    is because the default case in operational's eventhandler is doing the heavy lifting
    and forwarding the dispatch call to the inner statemachine!

    -optional
        create a working sub menu system, that only reveals what is possible,
        including something that shows the current queue of work, before the RTS is set to work. 
        should be done in configuration.
    */
