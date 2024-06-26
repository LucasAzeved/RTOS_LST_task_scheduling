#include <iostream>
#include <sstream>
#include <vector>

#define MAX_TASKS 26

using namespace std;

class Task{
private:
    char id;
    unsigned computing;
    unsigned period;
    unsigned deadline;
    unsigned c_rem;
    unsigned p_abs;
    unsigned d_abs;
public:
    Task(){
        id = '.';
        computing = 1;
        period = 1;
        deadline = 1;
        c_rem = 0;
        p_abs = 1;
        d_abs = 1;
    }
    
    Task(char i, unsigned c=0, unsigned p=0, unsigned d=0){
        id = i;
        computing = c;
        period = p;
        deadline = d;
        c_rem = c;
        p_abs = p;
        d_abs = d;
    }
    char getId() {return id;}
    unsigned getComputing() {return computing;}
    unsigned getPeriod() {return period;}
    unsigned getDeadline() {return deadline;}
    unsigned getAbsolutePeriod() {return p_abs;}
    unsigned getRemainigComputing() {return c_rem;}
    void setComputing(unsigned c) {computing = c;}
    void setPeriod(unsigned p) {period = p;}
    void setDeadline(unsigned d) {deadline = d;}
    int getSlack(unsigned t) {
        update(t);
        if (c_rem == 0)
            return 1024;
        return d_abs - t - c_rem;
    }
    char compute(unsigned t) {
        update(t);
        if(id == '.') 
            return '.';
        c_rem--;
        if ((t+1) > d_abs){
            return tolower(id);
        }
        return id;
    }
    void update(unsigned t) {
        if (t == p_abs) 
            c_rem = computing;
        p_abs = (t / period) * period + period;
        d_abs = (t / period) * period + deadline;
    }
    string str() {
        stringstream ss;
        ss  << id 
            // << " C:" << computing 
            // << " P:" << period 
            // << " D:" << deadline
            << " c_rem:" << c_rem
            << " p_abs:" << p_abs
            << " d_abs:" << d_abs;
        return ss.str();
    }
};

// Task* getNextTask(vector<Task>& tasks, int numTasks, unsigned t) {
unsigned getNextTask(Task* tasks[], int numTasks, unsigned t) {
    unsigned ix = numTasks;
    int tmp = 1024, slack = 2048;

    // cout << "   getNxt: ";
    for (unsigned i = 0; i < numTasks; i++) {
        // cout << tasks[i]->getId() << " " << tasks[i]->getSlack(t) << " | ";
        tasks[i]->update(t);
        if (tasks[i]->getRemainigComputing() > 0){
            slack = tasks[i]->getSlack(t);
            if (slack < tmp) {
                tmp = slack;
                ix = i;
            }
        }
    }
    // cout << " [" << tasks[ix]->getId() << " " << tasks[ix]->getSlack(t) << "]" << endl;
    return ix;
}

int main() {
    unsigned numTasks, t_sim, c, p, d;
    Task* tasks[MAX_TASKS];

    while (1) {
        cin >> numTasks >> t_sim; // N T
        if (numTasks == 0 || t_sim == 0) break;

        // obtem parametros das tasks
        for (unsigned i = 0; i < numTasks; ++i) {
            cin >> c >> p >> d;
            char id = 'A' + i;
            tasks[i] = new Task(id, c, p, d);
        }
       
        tasks[numTasks] = new Task();        

        stringstream grade;
        stringstream preemptions;
        preemptions << "Preempcoes: ";

        Task *execTask, *lastTask;
        unsigned nextTsk, prmptns = 0, cntxtSw = 0;
        for (unsigned t_i = 0; t_i < (t_sim+1); t_i++) {
            nextTsk = getNextTask(tasks, numTasks, t_i);
            // cout << "t[" << t_i << "] "<< "Slack: " << tasks[nextTsk]->getSlack(t_i) << " - " << tasks[nextTsk]->str() << endl;

            if (nextTsk == numTasks) {
                if (t_i != t_sim) {
                    grade << tasks[numTasks]->compute(t_i);
                }
                execTask = tasks[numTasks];
            }
            for (unsigned j = 0; j < numTasks; j++){
                if (j == nextTsk) {
                    if (t_i != t_sim) {
                        grade << tasks[j]->compute(t_i);
                    }
                    execTask = tasks[j];
                } else {
                    tasks[j]->update(t_i);
                }
            }

            if(t_i == 0) {
                lastTask = execTask;
            }
            else {
                if (execTask->getId() != lastTask->getId()) { // Tarefas diferentes
                    cntxtSw++;
                    if (lastTask->getRemainigComputing() != 0 && lastTask->getRemainigComputing() != lastTask->getComputing()) {
                        // Tarefa anterior foi preemptada
                        prmptns++;
                        preemptions << execTask->getId() << t_i << " | ";
                    }
                    else if (execTask->getId() == '.') { 
                        // tarefa atual eh idle
                        if (t_i != t_sim){
                            prmptns++;
                            preemptions << execTask->getId() << t_i << " | ";
                        }
                    }
                } else {
                    if (execTask->getId() != '.' 
                        && (execTask->getAbsolutePeriod()-execTask->getPeriod()) == (t_i)) // Mesma tarefa, periodos diferentes
                        cntxtSw++;
                }
                lastTask = execTask;
            }
        }
            cout << grade.str() << endl;
            cout << cntxtSw << " " << prmptns << endl << endl;
            
            // cout << preemptions.str() << endl;
    }
    return 0;
}