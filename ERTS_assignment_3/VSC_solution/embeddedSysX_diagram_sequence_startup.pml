@startuml



Activate EmbeddedSystemX

EmbeddedSystemX -> PowerOnSelfTest: Event::PowerOnSelfTest

Activate PowerOnSelfTest
PowerOnSelfTest -> PowerOnSelfTest: SystemSelfTest()

alt SelfTestFailed  
    PowerOnSelfTest -> Failure : Event::SelfTestFailed
    Activate Failure
    Failure -> Failure : incCount()
    Failure -> PowerOnSelfTest : Event::Restart
    deactivate Failure
else SelfTestOk 
    PowerOnSelfTest -> Initializing : Event::SelfTestOk
    deactivate PowerOnSelfTest
    Activate Initializing
end 

deactivate PowerOnSelfTest
Initializing -> Initializing : startInitializing()
Initializing -> Operational : Event::Initialized
deactivate Initializing
Activate Operational



@enduml
