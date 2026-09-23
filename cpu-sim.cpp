/*
 * CPU Scheduling Simulation 
 * Compatible with C-Free / MinGW
 */

#include <iostream>
#include <string>

using namespace std;

// Operation details
struct Operation {
    double left;
    char op;
    double right;
    double result;
    string expr;

    Operation() {
        left = 0;
        op = ' ';
        right = 0;
        result = 0;
        expr = "";
    }
};

// Process structure
struct Process {
    string id; // p1, p2, ...
    Operation op; 
    double arrivalTime;
    double burstTime;
    int priority;

    double waitingTime;
    double turnaroundTime;
    double completionTime;
    double remainingTime;

    Process() {
        id = "";
        arrivalTime = 0;
        burstTime = 0;
        priority = 0;
        waitingTime = 0;
        turnaroundTime = 0;
        completionTime = 0;
        remainingTime = 0;
    }
};

// ----------------- 1. Linked List -----------------
class ProcessList {
private:
    struct Node {
        Process val;
        Node* next;
        Node(Process p) {
            val = p;
            next = NULL;
        }
    };

    Node* head;
    Node* tail;
    int total;

public:
    ProcessList() {
        head = NULL;
        tail = NULL;
        total = 0;
    }

    ~ProcessList() {
        Node* curr = head;
        while (curr != NULL) {
            Node* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void insert(Process p) {
        Node* n = new Node(p);
        if (head == NULL) {
            head = tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
        total++;
    }

    int getLength() {
        return total;
    }

    Process getAt(int index) {
        Node* curr = head;
        for (int i = 0; i < index && curr != NULL; i++) {
            curr = curr->next;
        }
        return curr->val;
    }
};

// ----------------- 2. Standard Queue -----------------
class IntQueue {
private:
    struct QNode {
        int item;
        QNode* next;
        QNode(int val) {
            item = val;
            next = NULL;
        }
    };

    QNode* front;
    QNode* rear;

public:
    IntQueue() {
        front = NULL;
        rear = NULL;
    }

    ~IntQueue() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(int val) {
        QNode* n = new QNode(val);
        if (rear == NULL) {
            front = rear = n;
        } else {
            rear->next = n;
            rear = n;
        }
    }

    void pop() {
        if (front == NULL) return;
        QNode* temp = front;
        front = front->next;
        if (front == NULL) {
            rear = NULL;
        }
        delete temp;
    }

    int peek() {
        if (front != NULL) {
            return front->item;
        }
        return -1;
    }

    bool isEmpty() {
        return front == NULL;
    }
};

// ----------------- 3. Priority Queue (Min-Priority) -----------------
class IntPriorityQueue {
private:
    struct PQNode {
        int item;        // Process Index
        double priority; // Key value (smaller value = served first)
        PQNode* next;
        PQNode(int val, double pri) {
            item = val;
            priority = pri;
            next = NULL;
        }
    };

    PQNode* head;

public:
    IntPriorityQueue() {
        head = NULL;
    }

    ~IntPriorityQueue() {
        while (!isEmpty()) {
            pop();
        }
    }

    // Inserts in ascending order: smallest priority value stays at the front
    void push(int val, double pri) {
        PQNode* n = new PQNode(val, pri);
        if (head == NULL || pri < head->priority) {
            n->next = head;
            head = n;
        } else {
            PQNode* curr = head;
            while (curr->next != NULL && curr->next->priority <= pri) {
                curr = curr->next;
            }
            n->next = curr->next;
            curr->next = n;
        }
    }

    void pop() {
        if (head == NULL) return;
        PQNode* temp = head;
        head = head->next;
        delete temp;
    }

    int peek() {
        if (head != NULL) {
            return head->item;
        }
        return -1;
    }

    bool isEmpty() {
        return head == NULL;
    }
};

// ----------------- 4. Stack -----------------
class StringStack {
private:
    struct StackNode {
        string data;
        StackNode* next;
        StackNode(string s) {
            data = s;
            next = NULL;
        }
    };

    StackNode* topNode;

public:
    StringStack() {
        topNode = NULL;
    }

    ~StringStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(string s) {
        StackNode* n = new StackNode(s);
        n->next = topNode;
        topNode = n;
    }

    void pop() {
        if (topNode == NULL) return;
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }

    string peek() {
        if (topNode != NULL) {
            return topNode->data;
        }
        return "";
    }

    bool isEmpty() {
        return topNode == NULL;
    }
};

// Helper function to turn int to string
string toStr(int n) {
    if (n == 0) return "0";
    string res = "";
    bool isNeg = false;
    if (n < 0) {
        isNeg = true;
        n = -n;
    }
    while (n > 0) {
        char c = (n % 10) + '0';
        res = c + res;
        n /= 10;
    }
    if (isNeg) res = "-" + res;
    return res;
}

// Function to print text with spacing for tables
void printText(string s, int width) {
    cout << s;
    for (int i = s.length(); i < width; i++) {
        cout << " ";
    }
}

// Function to print numbers with decimal point
void printNum(double val, int width) {
    int whole = (int)val;
    int dec = (int)((val - whole) * 100);
    if (dec < 0) dec = -dec;

    string s = toStr(whole) + ".";
    if (dec < 10) s += "0";
    s += toStr(dec);

    printText(s, width);
}

// Store results for the final comparison table
struct AlgoSummary {
    string name;
    double avgWait;
    double avgTurn;
};

// ----------------- FCFS (Uses IntQueue) -----------------
AlgoSummary executeFCFS(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].waitingTime = 0;
        p[i].turnaroundTime = 0;
        p[i].completionTime = 0;
        p[i].remainingTime = p[i].burstTime;
    }

    IntQueue q;
    bool inQueue[50];
    for (int i = 0; i < n; i++) inQueue[i] = false;

    string sequence[100];
    int seqCount = 0;

    double currentTime = 0.0;
    int done = 0;

    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrivalTime <= currentTime) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (q.isEmpty()) {
            double nextTime = 999999.0;
            for (int i = 0; i < n; i++) {
                if (!inQueue[i] && p[i].arrivalTime < nextTime) {
                    nextTime = p[i].arrivalTime;
                }
            }
            currentTime = nextTime;
            continue;
        }

        int currIdx = q.peek();
        q.pop();

        p[currIdx].waitingTime = currentTime - p[currIdx].arrivalTime;
        if (p[currIdx].waitingTime < 0) p[currIdx].waitingTime = 0;

        currentTime += p[currIdx].burstTime;
        p[currIdx].completionTime = currentTime;
        p[currIdx].turnaroundTime = currentTime - p[currIdx].arrivalTime;

        sequence[seqCount++] = p[currIdx].id;
        done++;
    }

    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << "| 1. First Come First Serve (FCFS)                                      |" << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << endl << " Execution Path: ";
    for (int i = 0; i < seqCount; i++) {
        if (i > 0) cout << " -> ";
        cout << "[" << sequence[i] << "]";
    }
    cout << endl << endl;

    cout << "-------------------------------------------------------------------------" << endl;
    printText("PID", 8);
    printText("Arrival", 14);
    printText("Burst", 14);
    printText("Completion", 16);
    printText("Waiting", 12);
    printText("Turnaround", 12);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    double sumW = 0, sumT = 0;
    for (int i = 0; i < n; i++) {
        printText(p[i].id, 8);
        printNum(p[i].arrivalTime, 14);
        printNum(p[i].burstTime, 14);
        printNum(p[i].completionTime, 16);
        printNum(p[i].waitingTime, 12);
        printNum(p[i].turnaroundTime, 12);
        cout << endl;

        sumW += p[i].waitingTime;
        sumT += p[i].turnaroundTime;
    }
    cout << "-------------------------------------------------------------------------" << endl;

    AlgoSummary res;
    res.name = "FCFS";
    res.avgWait = sumW / n;
    res.avgTurn = sumT / n;

    cout << ">> Average Waiting Time    : ";
    printNum(res.avgWait, 10);
    cout << endl;
    cout << ">> Average Turnaround Time : ";
    printNum(res.avgTurn, 10);
    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;

    return res;
}

// ----------------- SJF (Uses IntPriorityQueue sorted by burstTime) -----------------
AlgoSummary executeSJF(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].waitingTime = 0;
        p[i].turnaroundTime = 0;
        p[i].completionTime = 0;
        p[i].remainingTime = p[i].burstTime;
    }

    IntPriorityQueue pq;
    bool inQueue[50];
    for (int i = 0; i < n; i++) inQueue[i] = false;

    string sequence[100];
    int seqCount = 0;

    double currentTime = 0.0;
    int done = 0;

    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrivalTime <= currentTime) {
                pq.push(i, p[i].burstTime);
                inQueue[i] = true;
            }
        }

        if (pq.isEmpty()) {
            double nextTime = 999999.0;
            for (int i = 0; i < n; i++) {
                if (!inQueue[i] && p[i].arrivalTime < nextTime) {
                    nextTime = p[i].arrivalTime;
                }
            }
            currentTime = nextTime;
            continue;
        }

        int currIdx = pq.peek();
        pq.pop();

        p[currIdx].waitingTime = currentTime - p[currIdx].arrivalTime;
        if (p[currIdx].waitingTime < 0) p[currIdx].waitingTime = 0;

        currentTime += p[currIdx].burstTime;
        p[currIdx].completionTime = currentTime;
        p[currIdx].turnaroundTime = currentTime - p[currIdx].arrivalTime;

        sequence[seqCount++] = p[currIdx].id;
        done++;
    }

    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << "| 2. Shortest Job First (SJF) [Non-Preemptive]                          |" << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << endl << " Execution Path: ";
    for (int i = 0; i < seqCount; i++) {
        if (i > 0) cout << " -> ";
        cout << "[" << sequence[i] << "]";
    }
    cout << endl << endl;

    cout << "-------------------------------------------------------------------------" << endl;
    printText("PID", 8);
    printText("Arrival", 14);
    printText("Burst", 14);
    printText("Completion", 16);
    printText("Waiting", 12);
    printText("Turnaround", 12);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    double sumW = 0, sumT = 0;
    for (int i = 0; i < n; i++) {
        printText(p[i].id, 8);
        printNum(p[i].arrivalTime, 14);
        printNum(p[i].burstTime, 14);
        printNum(p[i].completionTime, 16);
        printNum(p[i].waitingTime, 12);
        printNum(p[i].turnaroundTime, 12);
        cout << endl;

        sumW += p[i].waitingTime;
        sumT += p[i].turnaroundTime;
    }
    cout << "-------------------------------------------------------------------------" << endl;

    AlgoSummary res;
    res.name = "SJF (Non-Preemptive)";
    res.avgWait = sumW / n;
    res.avgTurn = sumT / n;

    cout << ">> Average Waiting Time    : ";
    printNum(res.avgWait, 10);
    cout << endl;
    cout << ">> Average Turnaround Time : ";
    printNum(res.avgTurn, 10);
    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;

    return res;
}

// ----------------- Priority  -----------------
AlgoSummary executePriority(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].waitingTime = 0;
        p[i].turnaroundTime = 0;
        p[i].completionTime = 0;
        p[i].remainingTime = p[i].burstTime;
    }

    IntPriorityQueue pq;
    bool inQueue[50];
    for (int i = 0; i < n; i++) inQueue[i] = false;

    string sequence[100];
    int seqCount = 0;

    double currentTime = 0.0;
    int done = 0;

    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrivalTime <= currentTime) {
                pq.push(i, (double)p[i].priority);
                inQueue[i] = true;
            }
        }

        if (pq.isEmpty()) {
            double nextTime = 999999.0;
            for (int i = 0; i < n; i++) {
                if (!inQueue[i] && p[i].arrivalTime < nextTime) {
                    nextTime = p[i].arrivalTime;
                }
            }
            currentTime = nextTime;
            continue;
        }

        int currIdx = pq.peek();
        pq.pop();

        p[currIdx].waitingTime = currentTime - p[currIdx].arrivalTime;
        if (p[currIdx].waitingTime < 0) p[currIdx].waitingTime = 0;

        currentTime += p[currIdx].burstTime;
        p[currIdx].completionTime = currentTime;
        p[currIdx].turnaroundTime = currentTime - p[currIdx].arrivalTime;

        sequence[seqCount++] = p[currIdx].id;
        done++;
    }

    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << "| 3. Priority Scheduling [Non-Preemptive]                               |" << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << endl << " Execution Path: ";
    for (int i = 0; i < seqCount; i++) {
        if (i > 0) cout << " -> ";
        cout << "[" << sequence[i] << "]";
    }
    cout << endl << endl;

    cout << "-------------------------------------------------------------------------" << endl;
    printText("PID", 8);
    printText("Arrival", 14);
    printText("Burst", 14);
    printText("Completion", 16);
    printText("Waiting", 12);
    printText("Turnaround", 12);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    double sumW = 0, sumT = 0;
    for (int i = 0; i < n; i++) {
        printText(p[i].id, 8);
        printNum(p[i].arrivalTime, 14);
        printNum(p[i].burstTime, 14);
        printNum(p[i].completionTime, 16);
        printNum(p[i].waitingTime, 12);
        printNum(p[i].turnaroundTime, 12);
        cout << endl;

        sumW += p[i].waitingTime;
        sumT += p[i].turnaroundTime;
    }
    cout << "-------------------------------------------------------------------------" << endl;

    AlgoSummary res;
    res.name = "Priority Scheduling";
    res.avgWait = sumW / n;
    res.avgTurn = sumT / n;

    cout << ">> Average Waiting Time    : ";
    printNum(res.avgWait, 10);
    cout << endl;
    cout << ">> Average Turnaround Time : ";
    printNum(res.avgTurn, 10);
    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;

    return res;
}

// ----------------- Round Robin (Uses IntQueue) -----------------
AlgoSummary executeRoundRobin(Process p[], int n, double quantum) {
    for (int i = 0; i < n; i++) {
        p[i].waitingTime = 0;
        p[i].turnaroundTime = 0;
        p[i].completionTime = 0;
        p[i].remainingTime = p[i].burstTime;
    }

    IntQueue q;
    bool inQueue[50];
    for (int i = 0; i < n; i++) inQueue[i] = false;

    string sequence[100];
    int seqCount = 0;

    double currentTime = 0.0;
    int done = 0;

    while (done < n) {
        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrivalTime <= currentTime) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (q.isEmpty()) {
            double nextTime = 999999.0;
            for (int i = 0; i < n; i++) {
                if (!inQueue[i] && p[i].arrivalTime < nextTime) {
                    nextTime = p[i].arrivalTime;
                }
            }
            currentTime = nextTime;
            continue;
        }

        int currIdx = q.peek();
        q.pop();

        sequence[seqCount++] = p[currIdx].id;

        double slice = quantum;
        if (p[currIdx].remainingTime < slice) {
            slice = p[currIdx].remainingTime;
        }

        p[currIdx].remainingTime -= slice;
        currentTime += slice;

        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && p[i].arrivalTime <= currentTime) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (p[currIdx].remainingTime > 0.0001) {
            q.push(currIdx);
        } else {
            p[currIdx].remainingTime = 0.0;
            p[currIdx].completionTime = currentTime;
            p[currIdx].turnaroundTime = currentTime - p[currIdx].arrivalTime;
            p[currIdx].waitingTime = p[currIdx].turnaroundTime - p[currIdx].burstTime;
            done++;
        }
    }

    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << "| 4. Round Robin (RR) [Quantum = ";
    printNum(quantum, 4);
    cout << "]                            |" << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;
    cout << endl << " Execution Path: ";
    for (int i = 0; i < seqCount; i++) {
        if (i > 0) cout << " -> ";
        cout << "[" << sequence[i] << "]";
    }
    cout << endl << endl;

    cout << "-------------------------------------------------------------------------" << endl;
    printText("PID", 8);
    printText("Arrival", 14);
    printText("Burst", 14);
    printText("Completion", 16);
    printText("Waiting", 12);
    printText("Turnaround", 12);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    double sumW = 0, sumT = 0;
    for (int i = 0; i < n; i++) {
        printText(p[i].id, 8);
        printNum(p[i].arrivalTime, 14);
        printNum(p[i].burstTime, 14);
        printNum(p[i].completionTime, 16);
        printNum(p[i].waitingTime, 12);
        printNum(p[i].turnaroundTime, 12);
        cout << endl;

        sumW += p[i].waitingTime;
        sumT += p[i].turnaroundTime;
    }
    cout << "-------------------------------------------------------------------------" << endl;

    AlgoSummary res;
    res.name = "Round Robin (q=" + toStr((int)quantum) + ")";
    res.avgWait = sumW / n;
    res.avgTurn = sumT / n;

    cout << ">> Average Waiting Time    : ";
    printNum(res.avgWait, 10);
    cout << endl;
    cout << ">> Average Turnaround Time : ";
    printNum(res.avgTurn, 10);
    cout << endl;
    cout << "+-----------------------------------------------------------------------+" << endl;

    return res;
}

// ----------------- Comparison Table -----------------
void showComparison(AlgoSummary list[], int total) {
    cout << endl << endl;
    cout << "=========================================================================" << endl;
    cout << "                      PERFORMANCE COMPARISON                             " << endl;
    cout << "=========================================================================" << endl;
    cout << endl;

    printText("Algorithm", 32);
    printText("Avg Waiting", 18);
    printText("Avg Turnaround", 18);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    for (int i = 0; i < total; i++) {
        printText(list[i].name, 32);
        printNum(list[i].avgWait, 18);
        printNum(list[i].avgTurn, 18);
        cout << endl;
    }

    cout << "-------------------------------------------------------------------------" << endl;
}

// ----------------- Main Program -----------------
int main() {
    cout << "=========================================================================" << endl;
    cout << "         CPU SCHEDULING ALGORITHMS SIMULATION (DATA STRUCTURES)          " << endl;
    cout << "=========================================================================" << endl << endl;

    cout << "Enter total number of processes (e.g. enter 3): ";
    int n;
    cin >> n;

    cout << "Enter Round Robin Time Quantum (e.g. enter 2): ";
    double quantum;
    cin >> quantum;

    ProcessList pList;

    for (int i = 1; i <= n; i++) {
        Process p;
        p.id = "P" + toStr(i);

        cout << endl << "-------------------- Process " << p.id << " --------------------" << endl;
        cout << "Enter arithmetic operation (e.g. 10 + 20): ";
        cin >> p.op.left >> p.op.op >> p.op.right;

        // Calculate operation result directly
        if (p.op.op == '+') p.op.result = p.op.left + p.op.right;
        else if (p.op.op == '-') p.op.result = p.op.left - p.op.right;
        else if (p.op.op == '*') p.op.result = p.op.left * p.op.right;
        else if (p.op.op == '/') {
            if (p.op.right != 0) p.op.result = p.op.left / p.op.right;
            else p.op.result = 0;
        }

        // Build expression string for the summary table
        p.op.expr = toStr((int)p.op.left) + " " + p.op.op + " " + toStr((int)p.op.right);
        cout << "   -> Operation Result: " << p.op.result << endl;

        cout << "Arrival Time (time process arrives, e.g. 0): ";
        cin >> p.arrivalTime;

        cout << "Burst Time (total CPU execution time, e.g. 5): ";
        cin >> p.burstTime;

        cout << "Priority (smaller number = higher priority, e.g. 1 or 2): ";
        cin >> p.priority;

        pList.insert(p);
    }

    // Copy to regular array for processing
    Process procs[50];
    for (int i = 0; i < n; i++) {
        procs[i] = pList.getAt(i);
    }

    cout << endl << endl;
    cout << "========================= INPUT PROCESS SUMMARY =========================" << endl;
    printText("PID", 8);
    printText("Operation", 18);
    printText("Result", 12);
    printText("Arrival", 12);
    printText("Burst", 10);
    printText("Priority", 10);
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    for (int i = 0; i < n; i++) {
        printText(procs[i].id, 8);
        printText(procs[i].op.expr, 18);
        printNum(procs[i].op.result, 12);
        printNum(procs[i].arrivalTime, 12);
        printNum(procs[i].burstTime, 10);
        printText(toStr(procs[i].priority), 10);
        cout << endl;
    }

    // Run the algorithms
    AlgoSummary summaries[4];

    summaries[0] = executeFCFS(procs, n);
    summaries[1] = executeSJF(procs, n);
    summaries[2] = executePriority(procs, n);
    summaries[3] = executeRoundRobin(procs, n, quantum);

    // 5th Table: Comparison
    showComparison(summaries, 4);

    // Stack verification
    StringStack s;
    for (int i = 0; i < n; i++) {
        s.push(procs[i].id);
    }

    cout << endl << ">> [Stack Demonstration (LIFO Verification)]" << endl;
    cout << "   Reverse Process Insertion Order: ";
    bool first = true;
    while (!s.isEmpty()) {
        if (!first) cout << " -> ";
        cout << "[" << s.peek() << "]";
        s.pop();
        first = false;
    }
    cout << endl;

    // Acknowledgements
    cout << endl;
    cout << "=========================================================================" << endl;
    cout << "Made By Eng.Mohammed Al-Shaheri." << endl;
    cout << "Thanks For The G.O.A.T , Phenomenal , Legendary , The Best : ENG.Mohammed Al-Sayanni" << endl;
    cout << "=========================================================================" << endl;

    return 0;
}
