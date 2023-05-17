#include "processor_edf.h"

namespace core {
    ProcessorEDF::ProcessorEDF(Scheduler* scheduler) : Processor(ProcessorType::EDF, scheduler) {
    }

    void ProcessorEDF::ScheduleAlgo() {
        //check for currently running process
        if (m_RunningProcess != 0) {
            //check for first proc in RDY
            Process* p = 0;
            if (m_ReadyProcesses.Peek(&p)) {
                //compare deadlines
                if (p->GetDeadline() < m_RunningProcess->GetDeadline()) {
                    //requeue running process
                    RequeueRunningProcess();
                }
            }
        }

        //if running process has been requeued, the one with the earliest deadline will run below
        //get process from ready
        Process* proc = 0;
        if (m_RunningProcess == 0 && m_ReadyProcesses.Dequeue(&proc)) {
            //run it
            RunProcess(proc);
        }
    }

    void ProcessorEDF::QueueProcess(Process* proc) {
        //update timer
        Processor::QueueProcess(proc);

        //add to ready list
        m_ReadyProcesses.Enqueue(proc);
    }

    void ProcessorEDF::Print(_STD wstringstream& stream) {
        Processor::Print(stream);

        //print ready queue
        stream << m_ReadyProcesses.GetLength() << L" RDY: ";

        //print procs
        m_ReadyProcesses.Print(stream);

        //terminator
        stream << L'\n';
    }

    void ProcessorEDF::RequeueRunningProcess() {
        if (m_RunningProcess != 0) {
            m_ReadyProcesses.Enqueue(m_RunningProcess);
        }

        Processor::RequeueRunningProcess();
    }

    bool ProcessorEDF::GetStealHandle(StealHandle* stealHandle) {
        if (stealHandle == 0) return false;

        //check for any RDY process
        if (m_ReadyProcesses.GetLength() == 0) return false;

        Process* proc = 0;
        if (!m_ReadyProcesses.Peek(&proc) || proc->IsForked()) return false; //no forked processes are applicable

        //process is applicable for stealing
        *stealHandle = {
            proc,
            [&]() -> void {
                //update timer
                DecrementTimer(proc);

                //remove from queue
                m_ReadyProcesses.Dequeue();
            }
        };

        return true;
    }
}