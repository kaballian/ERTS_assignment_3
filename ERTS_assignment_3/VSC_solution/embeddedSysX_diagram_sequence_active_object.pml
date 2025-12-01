@startuml


Main.cpp ->EmbeddedSystemX : System.Configure()
EmbeddedSystemX -> StateMachine : sm.dispatch(Event::Configure)
Activate EmbeddedSystemX
Activate StateMachine
Activate opFsm

Activate Operational
StateMachine -> Operational : HandleEvent(Event::configure) default case:
Operational -> Operational : HandleEvent : default 
Operational -> opFsm : opFsm.dispatch(Event::configure)

opFsm -> Ready : HandleEvent(Event::Configure)



Activate Ready
Ready -> opFsm : singleton<configuration>::instance()
opFsm -> Ready : onExit()
Ready -> Ready : onExit()
Ready -> opFsm
deactivate Ready

opFsm -> Configuration : onEntry()
Activate Configuration
Configuration -> Configuration : readConfigurationInfo()
Configuration -> Configuration : sm.dispatch(Event::ConfigurationEnded)

Configuration -> opFsm : singleton<Ready>::instance()
opFsm -> Configuration : onExit()
Configuration -> Configuration : onExit()
Configuration -> opFsm
deactivate Configuration

opFsm -> Ready : onEntry()
Activate Ready
Ready -> Ready : onEntry()
Ready --> Operational
Operational --> EmbeddedSystemX : unwind and return control 
EmbeddedSystemX --> Main.cpp

Main.cpp -> EmbeddedSystemX : System.Start/run()
EmbeddedSystemX -> StateMachine : sm.dispatch(Event::Start/run)
StateMachine -> Operational : HandleEvent(Event::Start)
Operational -> Operational : opFsm.dispatch(Event::Start)

opFsm -> Ready : HandleEvent(event::Start/run)
Ready -> opFsm : singleton<RealTimeLoop>::instance()
opFsm -> Ready : onExit()
Ready -> Ready : onExit()
Ready -> opFsm
deactivate Ready

opFsm -> RealTimeLoop : onEntry()
Activate RealTimeLoop
RealTimeLoop -> RealTimeLoop : onEntry() scheduler starts
RealTimeLoop --> Operational : opFsm is set, return control to Operational::eventHandler







@enduml