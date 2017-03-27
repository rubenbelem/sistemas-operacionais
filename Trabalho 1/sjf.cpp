#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

struct Process {
    unsigned int pid;
    unsigned int start_time;
    unsigned int process_time;
    unsigned int wait_time;
    unsigned int end_time;

    Process() {
        pid = 0;
        start_time = 0;
        process_time = 0;
        wait_time = 0;
        end_time = 0;
    }
};

class CompareProcess {
    public:
        bool operator() (Process p1, Process p2) {
            return p1.process_time > p2.process_time;
        }
};

void execute_step(queue<Process>& ex_queue) {

    if (!ex_queue.empty()) {
        Process &p = ex_queue.front();
        p.process_time--;

        cout << "P" << p.pid << "  " << p.start_time << " | " << p.process_time << "\n";

        if(p.process_time == 0) {
            ex_queue.pop();
            cout << "--------------\n";
        }
    }
}

int main(int argc, char const *argv[]) {
    std::vector<Process> process_list;
    map<int, vector<Process> > to_execute;
    priority_queue<Process, std::vector<Process>, CompareProcess> process_queue;
    queue<Process> execute_queue;
    int i;
    int n;
    unsigned int major_time;

    while(true) {
        cin >> n;

        if (n == 0) break;

        major_time = 0;

        cout << n << endl;

        cout.flush();

        for (i = 0;i < n; i++) {
            cout << "DEBOA" << endl;
            Process p;
            p.pid = i + 1;
            cin >> p.start_time >> p.process_time;
            cout << p.start_time << " | " << p.process_time << endl;

            if (p.start_time > major_time) major_time = p.start_time;

            to_execute[p.start_time].push_back(p);

        }

        cout.flush();
        cout << "ISSO NAO FAZ SENTIDO!!" << endl << endl;

        int time = 0;

        while(time <= major_time) {
            cout << "At time: " << time << ":  ";

            if (to_execute[time].empty()) {
                time++;
                execute_step(execute_queue);
                if (execute_queue.empty()) {
                    execute_queue.push(process_queue.top());
                    process_queue.pop();
                }
                cout << endl;
                continue;
            }
            vector<Process> v = to_execute[time];

            for (int i = 0; i < v.size(); i++) {
                Process p = v[i];
                //cout << "P" << p.pid << "  " << p.start_time << " | " << p.process_time << "\n";

                process_queue.push(v[i]);
            }

            execute_step(execute_queue);

            if (execute_queue.empty()) {
                execute_queue.push(process_queue.top());
                process_queue.pop();
            }

            time++;
            cout << endl;
        }

        while(!process_queue.empty()) {
            execute_queue.push(process_queue.top());
            process_queue.pop();
        }

        //cout << "\n\nESTOU AQUI\n\n";

        while(!execute_queue.empty()) {
            cout << "At time: " << time << "  :";
            execute_step(execute_queue);

            time++;
        }
    }

    return 0;
}


