#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

enum class ProcessState : int {
	loaded,
	ready,
	running,
	suspended,
	finished
};

struct Process {
    unsigned int pid;
    unsigned int arrival_time;
    unsigned int start_time;
    unsigned int instructions;
    unsigned int wait_time;
    unsigned int end_time;
    ProcessState state;

    Process() {
        pid = 0;
        start_time = 0;
        instructions = 0;
        wait_time = 0;
        end_time = 0;
        state = ProcessState::loaded;
    }
};

class CompareProcess {
    public:
        bool operator() (Process p1, Process p2) {
            return p1.instructions > p2.instructions;
        }
};

class Scheduler {
	private:
		map<int, vector<Process> > to_execute;
		priority_queue<Process, std::vector<Process>, CompareProcess> waiting_queue;
		queue<Process> execute_queue;
		vector<Process> execution_order;

		unsigned int cnt_time, greater_time;
		double average_runtime;
		double average_wait_time;

	public:
		Scheduler() {
			cnt_time = 0;
			greater_time = 0;
			average_runtime = 0.0;
			average_wait_time = 0.0;
		}

		bool execute_step() {
		    if (!execute_queue.empty()) {
		        Process &p = execute_queue.front();

		        if (p.state == ProcessState::ready) {
		        	p.start_time = cnt_time;
		        	p.wait_time = p.start_time - p.arrival_time;

		        	p.state = ProcessState::running;
		        }

		        p.instructions--;

		        if(p.instructions == 0) {
		        	p.end_time = cnt_time;
		        	p.state = ProcessState::finished;

		        	execution_order.push_back(p);
		            execute_queue.pop();
		        }

		        return true;
		    }

		    return false;
		}

		void insert_process(Process& p) {
			if (p.start_time > greater_time) greater_time = p.start_time;

			p.arrival_time = cnt_time;
			p.state = ProcessState::ready;

			to_execute[p.start_time].push_back(p);

		}

		void clear() {
			while(!execute_queue.empty()) execute_queue.pop();
			while(!waiting_queue.empty()) waiting_queue.pop();

			execution_order.clear();
			to_execute.clear();

			cnt_time = 0;
			average_runtime = 0.0;
			average_wait_time = 0.0;
			greater_time = 0;
		}

		void run_all() {
			while(cnt_time <= greater_time) {
				if (to_execute[cnt_time].empty()) {
					cnt_time++;

					execute_step();

					if (execute_queue.empty()) {
						if (!waiting_queue.empty()) {
							execute_queue.push(waiting_queue.top());
							waiting_queue.pop();
						}
					}
					continue;
				}

				vector<Process> v = to_execute[cnt_time];

				for (int i = 0; i < v.size(); i++) {
					waiting_queue.push(v[i]);
				}

				execute_step();

				if (execute_queue.empty()) {
					if (!waiting_queue.empty()) {
						execute_queue.push(waiting_queue.top());
						waiting_queue.pop();
					}
				}

				cnt_time++;
			}

			while(!waiting_queue.empty()) {
				execute_queue.push(waiting_queue.top());
				waiting_queue.pop();
			}

			while(!execute_queue.empty()) {
				execute_step();
				cnt_time++;
			}
		}

		void print_statistics() {
			int n = execution_order.size();
			for (int i = 0; i < n; i++) {
				average_wait_time += execution_order[i].wait_time;
			}

			average_wait_time /= (double) n;

			cout << "Tempo medio de espera: " << average_wait_time << "s" << endl;

			for (int i = 0; i < n; i++) {
				cout << "P" << execution_order[i].pid << " ";
				execution_order.pop_back();
			}
		}

		double test() {
			return average_runtime;
		}
};

int main(int argc, char const *argv[]) {
    int i, n, test = 1;
    Scheduler scheduler;

    while(true) {
        cin >> n;

        if (n == 0) break;

        for (i = 0;i < n; i++) {
            Process p;
            p.pid = i + 1;
            cin >> p.start_time >> p.instructions;

            scheduler.insert_process(p);
        }

        scheduler.run_all();

        cout << "Teste " << test << endl;

        scheduler.print_statistics();
        scheduler.clear();

        cout << "\n\n";

        test++;
    }

    return 0;
}


