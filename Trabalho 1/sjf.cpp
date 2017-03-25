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
            return p1.process_time < p2.process_time;
        }
};

int main(int argc, char const *argv[]) {
    std::vector<Process> process_list;
    map<int, vector<Process> > to_execute;
    priority_queue<Process, std::vector<Process>, CompareProcess> process_queue;
    queue<Process> execute_queue;

    int n;
    unsigned int major_time;

    while(true) {
        cin >> n;
        major_time = 0;

        for (int i = 0;i < n; i++) {
            Process p;
            p.pid = i + 1;
            cin >> p.start_time >> p.process_time;

            if (p.start_time > major_time) major_time = p.start_time;

            to_execute[p.start_time].push_back(p);
        }

        unsigned int time = 0;

        while(time < major_time) {
            vector<Process> v = to_execute[time];

            for (int i = 0; i < v.size(); i++) {
                process_queue.push(v[i]);
            }
        }

        for (int i = 0; i < process_queue.size(); i++) {
            Process p = process_queue.top();
            process_queue.pop();
            cout << "P" << p.pid << "  " << p.start_time << " | " << p.process_time << "\n";
        }
    }

    return 0;
}


