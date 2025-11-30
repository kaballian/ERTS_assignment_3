@startuml
scale 350 width

[*] --> PowerOnSelfTest

PowerOnSelfTest --> Failure : Event::SelfTestFailed
Failure --> PowerOnSelfTest : Event::Restart 
PowerOnSelfTest --> Initializing : Event::SelfTestOk
Initializing --> Operational : Event::Initialized

State Operational {
    [*] --> Ready
    
    Ready -left-> Configuration : Event::Configure
    Configuration -right-> Ready : Event::ConfigurationEnded
    Suspended --> Ready : Event::Stop
    Suspended --> RealTimeLoop : Event::Resume
    Ready -left-> RealTimeLoop : Event::Run/Start 
    RealTimeLoop --> Ready : Event::Stop
    RealTimeLoop --> Suspended : Event::Suspend

}

State RealTimeLoop{
    [*] --> Mode1 
    Mode1 --> Mode2 : chMode2
    Mode2 --> Mode3 : chMode3
    Mode3 --> Mode1 : chMode1  
}


@enduml