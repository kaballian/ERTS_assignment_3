@startuml

Activate OpFsm 

OpFsm --> Ready : creates 

Activate Ready
Ready --> Ready : idle

alt Event::Configure
    Ready -> Configuration : Event::Configure
    deactivate Ready 
    Activate Configuration
    
    Configuration -> Configuration : readConfigurationInfo()

    Configuration -> Ready : ConfigurationEnded
    deactivate Configuration
    Activate Ready


else Event:RealTimeLoop
    Ready -> RealTimeLoop : Event::Start/Run
    deactivate Ready
    Activate RealTimeLoop

    RealTimeLoop -> RealTimeLoop : <working>

    RealTimeLoop -> Suspended : Event::Suspend
    Activate Suspended
    Suspended -> Suspended : Awaiting
    Suspended -> RealTimeLoop : if Event::Resume
    Suspended -> Ready : if Event::Stop
    deactivate Suspended
    Activate Ready
    

    RealTimeLoop -> Ready : Event::Stop
    deactivate RealTimeLoop
    

end 

@enduml