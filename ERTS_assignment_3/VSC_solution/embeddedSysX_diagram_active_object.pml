@startuml
skinparam classAttributeIconSize 0
package "Operational" {
    package "RealTimeLoop (Active object)" #DDDDDD {

        class RTL_Servant{
            +Mode {mode1, mode2, mode3}: enum class 
            +chMode1_impl(): void
            +chMode2_impl(): void
            +chMode3_impl(): void
            +currentMode(): Mode
            -mode: Mode
        }
        Abstract RTL_Request <<virtual>>{
            +can_run(): virtual bool 
            +call() = 0: virtual void
            +~RTL_Request() : default
        }
        
        package "Requests" {
            class ChMode1Req{
            - s: RTL_Servant&
            - promise: std::promise<RTL_Servant::Mode>
            }
            class ChMode2Req{
            - s: RTL_Servant&
            - promise: std::promise<RTL_Servant::Mode>
            }
            class ChMode3Req{
            - s: RTL_Servant&
            - promise: std::promise<RTL_Servant::Mode>
            }
        }
    

        class RTL_ActivationQueue
        {
            +insert(): void
            +remove(): std::unique_ptr<RTL_Request>
            +set_running(): void 
            -mtex: std::mutex
            -que: std::queue<std::unique_ptr<RTL_Request>>
            -cv: std::condition_variable
            -running = true: bool 
        }

        class RTL_Scheduler {
            +start(): void 
            +stop(): void
            +enqueue(): void
            -queue: RTL_ActivationQueue
            -worker: std::thread
        }

        

        class RTL_Proxy {
            +makeModeRsq<RequestT>(): std::future<void>
            +chMode1(): std::future<std::future<RTL_Servant::Mode>>           
            +chMode2(): std::future<std::future<RTL_Servant::Mode>>
            +chMode3(): std::future<std::future<RTL_Servant::Mode>>
            -serv: RTL_Servant&
            -sched: RTL_Scheduler&
        }


        entity  Future <<void>> {

        }
        RTL_Request <|-- ChMode1Req
        RTL_Request <|-- ChMode2Req
        RTL_Request <|-- ChMode3Req
        RTL_Scheduler -- RTL_ActivationQueue
        RTL_Request <-- RTL_Scheduler
        RTL_Servant <-- RTL_Request
        RTL_Proxy -left- RTL_Scheduler

        RTL_Proxy -[dotted]-> Future : creates
        RTL_Proxy -[dotted]-> RTL_Request : creates

    }


    class outsideStatemachine <<client>> {

    }

    outsideStatemachine -right-> RTL_Proxy
    outsideStatemachine -right-> Future


}

@enduml