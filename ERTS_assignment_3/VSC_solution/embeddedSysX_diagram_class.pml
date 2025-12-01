@startuml
skinparam classAttributeIconSize 0
package "EmbeddedSystemX" #DDDDDD {
    abstract "State" <<virtual>>
    {
        + HandleEvent()
        + onEntry()
        + onExit()
        + getSubstateMachine()
        + Name()
    }
    class PowerOnSelfTest 
    {
        - SystemSelftest(): int
        - ErrorNo: int = 0
        - testResult: bool = false
    }
    class Failure
    {
        + incCount(): void
    }
    class Initializing
    {
        + startInitializing(): void
    }
    class Operational
    {
        + printReqWork(): void 
        + printMode(): void
        - restartCondition: bool = false
        - opFsm: StateMachine 
        - futureMode: std::optional<std::future<RTL_Servant::Mode>>
        - pendingFutures: std::vector<std::future<RTL_Servant::Mode>>
        - lastMade: RTL_Servant::Mode::Mode1
        - configParams: ModeConfig
    }

    Package Operational_SM {
        class Ready
        {

        }
        class Configuration
        {
            +readConfigurationInfo(): void
        }
        class Suspended
        {

        }
        Class RealTimeLoop
        {
            + getProxy(): RTL_Proxy&
            - Servant: RTL_Servant
            - scheduler: RTL_Scheduler
            - proxy: RTL_Proxy
        }
    }


    "State" <|-- PowerOnSelfTest
    "State" <|-- Failure
    "State" <|-- Initializing
    "State" <|-- Operational
    "State" <|-- Ready
    "State" <|-- Configuration
    "State" <|-- Suspended
    "State" <|-- RealTimeLoop


}

@enduml