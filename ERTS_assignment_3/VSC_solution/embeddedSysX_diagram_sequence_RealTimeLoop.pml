@startuml


Activate Operational
Activate RealTimeLoop 

Activate RTL_Proxy
Activate RTL_Scheduler

Activate RTL_ActivationQueue

RTL_Scheduler --> RTL_Scheduler : thread is waiting
Operational --> RealTimeLoop : (current opFsm state)
RealTimeLoop -> RealTimeLoop : onEntry()
Operational -> RTL_Proxy : rtl->getProxy()
RTL_Proxy --> Operational : acquire the reference to order worker
Operational -> Future : pendingFutures.push_back(proxy.chMode2()) (order the work)
Operational -> RTL_Proxy : pendingFutures.push_back(proxy.chMode2())
RTL_Proxy -> RTL_Proxy : chMode2() (creates a request)
RTL_Proxy -> RTL_Scheduler : sched:enqueue(request)
RTL_Scheduler -> RTL_Scheduler : enqueue()
RTL_Scheduler -> RTL_ActivationQueue : queue.insert()
RTL_ActivationQueue -> RTL_ActivationQueue : insert() (pushes and notifies)
RTL_Scheduler --> RTL_Scheduler : thread starts work
RTL_Scheduler -> RTL_ActivationQueue : queue.remove()
RTL_ActivationQueue -> RTL_ActivationQueue : queue.remove()
RTL_ActivationQueue -> RTL_Scheduler : request
RTL_Scheduler -> ChMode2Req : req->can_run()
Activate ChMode2Req
ChMode2Req --> RTL_Scheduler
RTL_Scheduler -> ChMode2Req : req->call()
ChMode2Req --> RTL_Scheduler
RTL_Scheduler -> ChMode2Req : call() 
ChMode2Req -> RTL_Servant : chMode2_impl()
Activate RTL_Servant
RTL_Servant -> RTL_Servant : Mode = Mode2
RTL_Servant --> ChMode2Req : promise.setvalue()
ChMode2Req --> Future : promise delivered
Operational --> Future : promise can be accessed 






@enduml