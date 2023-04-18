#include "deserializer.h"

namespace core {
    Deserializer::Deserializer(_STD wstring& path) : m_Processes(0), m_Sigkills(0), m_SigkillCount(0) {
        m_Stream.open(path, _STD ios::in);
    }

    Deserializer::~Deserializer() {
        //only delete parent array, dont delete the proccesses themselves
        if (m_Processes != 0) {
            delete[] m_Processes;
        }

        if (m_Sigkills != 0) {
            delete[] m_Sigkills;
        }
    }

    bool Deserializer::IsValid() {
        return m_Stream.good();
    }

    int Deserializer::GetSigkillCount() {
        return m_SigkillCount;
    }

    bool Deserializer::Deserialize(DeserializerData& data) {
        //zero out the data
        memset(&data, 0, sizeof(DeserializerData));

        //return false if file doesnt exist
        if (!IsValid()) {
            return false;
        }

        //processors info
        m_Stream >> data.num_processors_fcfs
            >> data.num_processors_sjf
            >> data.num_processors_rr;

        m_Stream >> data.rr_timeslice;

        //attributes
        m_Stream >> data.rtf
            >> data.maxw
            >> data.stl
            >> data.fork_prob;

        //process info
        m_Stream >> data.proc_count;

        //allocate proccesses array
        if (m_Processes != 0) {
            //delete old proc array if exists, but idealy deserialize should be called once
            delete[] m_Processes;
        }

        m_Processes = new Process*[data.proc_count];

        //read processes
        for (int i = 0; i < data.proc_count; i++) {
            int at;
            int pid;
            int ct;
            int ioCount;

            m_Stream >> at
                >> pid
                >> ct
                >> ioCount;

            //temporary alloc an array for io data
            ProcessIOData* procIOData = new ProcessIOData[ioCount];
            
            //zero out the array
            memset(procIOData, 0, sizeof(ProcessIOData) * ioCount);

            //read io data in the format of (x,y),(z, w)
            char tmp;
            for (int j = 0; j < ioCount; j++) {
                ProcessIOData ioData;

                m_Stream >> tmp //(
                    >> ioData.request_time //x
                    >> tmp //,
                    >> ioData.duration //y
                    >> tmp; //)

                procIOData[j] = ioData;

                //if not last element, read a comma
                if (j < ioCount - 1) {
                    m_Stream >> tmp;
                }
            }

            //alloc new proc
            Process* proc = new Process(pid, at, ct, procIOData, ioCount);

            //delete ioData array
            delete[] procIOData;

            //add proc to processes array
            m_Processes[i] = proc;
        }

        data.procs = m_Processes;

        //read sig kill times
        //read till eof
        //in theory, number of sigkills <= proc count
        m_Sigkills = new SigkillTimeInfo[data.proc_count];
        m_SigkillCount = 0;

        while (!m_Stream.eof()) {
            if (m_SigkillCount >= data.proc_count) {
                break;
            }

            SigkillTimeInfo info;

            m_Stream >> info.time
                >> info.proc_pid;

            m_Sigkills[m_SigkillCount++] = info;
        }

        data.sigkills = m_Sigkills;

        return true;
    }
}
