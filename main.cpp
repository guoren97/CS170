#include <functional>
#include <iostream>
#include <queue>
#include <time.h>
using namespace std;

/****Data structure define****/
typedef vector<vector<int>> figure;
struct state{
    figure now;
    int depth;
    state(figure Figure1, int i):now(Figure1), depth(i){}
};
struct Existstate{
    figure now;
    figure parent;
    int depth;
    int heuristics;
    Existstate(figure Figure1, figure Figure2, int gn, int hn):now(Figure1), parent(Figure2), depth(gn), heuristics(hn){}
};
figure InitialState, GoalState;
int PuzzleNum;
int operate;
int ExpandednodesNum = 0;
int MaxqueuelementNum = 0;

/****Figure out all possible expand states and return****/
vector<figure> ExpandState(figure &Figure){
    vector<figure> next;
    int x, y;
    for(int i = 0; i < PuzzleNum; i++)
        for(int j = 0; j < PuzzleNum; j++)
            if(Figure[i][j] == 0){  //get the position of 0
                x = i;
                y = j;
            }
    figure TempFigure = Figure;
    if(x > 0){
        TempFigure[x][y] = TempFigure[x - 1][y];
        TempFigure[x - 1][y] = 0;
        next.push_back(TempFigure);
    }
    TempFigure = Figure;
    if(x < PuzzleNum - 1){
        TempFigure[x][y] = TempFigure[x + 1][y];
        TempFigure[x + 1][y] = 0;
        next.push_back(TempFigure);
    }
    TempFigure = Figure;
    if(y > 0){
        TempFigure[x][y] = TempFigure[x][y - 1];
        TempFigure[x][y - 1] = 0;
        next.push_back(TempFigure);
    }
    TempFigure = Figure;
    if(y < PuzzleNum - 1){
        TempFigure[x][y] = TempFigure[x][y + 1];
        TempFigure[x][y + 1] = 0;
        next.push_back(TempFigure);
    }
    TempFigure = Figure;
    return next;
}

int MisplacedTile(state &State){
    int Misplaced = 0;
    for(int i = 0; i < PuzzleNum; i++)
        for(int j = 0; j < PuzzleNum; j++)
            if(State.now[i][j] != GoalState[i][j] && GoalState[i][j] != 0)
                Misplaced++;
    return Misplaced;
}

int ManhattanDist(state &State){
    int Manhattan = 0;
    for(int i1 = 0; i1 < PuzzleNum; i1++)
        for(int j1 = 0; j1 < PuzzleNum; j1++)
            if(State.now[i1][j1] != 0)
                for(int i2 = 0; i2 < PuzzleNum; i2++)
                    for(int j2 =0; j2 < PuzzleNum; j2++)
                        if(State.now[i1][j1] == GoalState[i2][j2])
                            Manhattan = Manhattan + abs(i1 - i2) + abs(j1 - j2);
    return Manhattan;
}

/****Choose the search method****/
int AStarDist(state &State){
    if(operate == 1)
        return State.depth;
    else if(operate == 2)
        return State.depth + MisplacedTile(State);
    else if(operate == 3)
        return State.depth + ManhattanDist(State);
}

/****For dynamically sorted Queue to sort new element****/
struct compare{
    bool operator() (state &state1, state &state2){
        return AStarDist(state1) > AStarDist(state2);   //less A* distance, topper position
    }
};

/****Print a 'figure' type variable****/
void printer(figure &Figure){
    for(int i = 0; i < PuzzleNum; i++){
        for(int j = 0; j < PuzzleNum; j++)
            cout << Figure[i][j] << "   ";
        cout << endl;
    }
    cout << endl;
}


/****Print the ExistedState array based on the last element****/
/****A recursive function****/
void print(vector<Existstate> &Exist, figure &child){
    int flag = 1;
    for(int i = 0; i < Exist.size(); i++)
        if(child == Exist[i].now && Exist[i].parent != InitialState){   //if the element's parent is not the InitialState
            print(Exist, Exist[i].parent);  //find the element's parent
            flag = i;   //signal the subscript
        }
    printer(Exist[flag].parent);    //print from the "parentest" parent (InitialState)
    cout << "The next state's g(n) = " << Exist[flag].depth << " and h(n) = " << Exist[flag].heuristics << endl;
}



int main()
{
    while(1){
        /***Initial the PuzzleNum, InitialState and GoalState***/
        printf("\n\nPlease input your puzzle row number (8-puzzle is 3):\n");
        cin >> PuzzleNum;
        InitialState = figure(PuzzleNum, vector<int>(PuzzleNum));
        printf("Please input your initial state(use a zero to represent the blank)\n");
        for(int i = 0; i < (PuzzleNum*PuzzleNum); i++)
            cin >> InitialState[i/PuzzleNum][i%PuzzleNum];
        GoalState = figure(PuzzleNum, vector<int>(PuzzleNum));
        printf("Please input your goal state\n");
        for(int i = 0; i < (PuzzleNum*PuzzleNum); i++)
            cin >> GoalState[i/PuzzleNum][i%PuzzleNum];
        cout << endl;
        printf("Enter your choice of algorithm\n1.Uniform Cost Search\n2.A* with the Misplaced Tile heuristic\n3.A* with the Manhattan distance heuristic.\n");
        cin >> operate;
        cout << endl;
        ExpandednodesNum = 0;
        MaxqueuelementNum = 0;

        /***Initial the dynamically sorted Queue and the ExistedState array***/
        priority_queue<state, vector<state>, compare> Queue;    //compare is sort function when insert new element
        vector<Existstate> Exist;
        state initial = state(InitialState, 0);
        Exist.push_back(Existstate(InitialState, InitialState, 0, AStarDist(initial)));
        Queue.push(initial);

        /***Solution***/
        cout << "Soluting..." << endl << endl;
        clock_t start = clock();
        while(!Queue.empty()){
            /**Take out (pop) the top element of Queue**/
            state nowstate = Queue.top();
            figure temp = nowstate.now;
            Queue.pop();

            /**When nowstate is GoalState**/
            if(temp == GoalState){
                cout << "Initial State is" << endl;
                print(Exist, temp);
                printer(GoalState);
                break; //jump out the loop
            }

            /**Expand the newstate based on the nowstate**/
            vector<figure> next = ExpandState(temp);
            for(int i = 0; i < next.size(); i++){
                int ExistFlag = 0;
                for(int j = 0; j< Exist.size(); j++)
                    if(Exist[j].now == next[i])   //if the newstate has already existed in ExistedState array
                        ExistFlag = 1;
                if(ExistFlag != 1){ //if a true newstate, push in the ExistedState array and the Queue
                    state newstate = state(next[i], nowstate.depth + 1);
                    Exist.push_back(Existstate(next[i], temp, newstate.depth, AStarDist(newstate) - newstate.depth));
                    Queue.push(newstate);
                    ExpandednodesNum++;
                    if(Queue.size() > MaxqueuelementNum)
                        MaxqueuelementNum = Queue.size();
                }
            }
            if(((clock()-start) * 1.0) / (((clock_t)1000) * 1000) > 30.0){  //if timeout (30s)
                if(operate != 3)
                    cout << "it's tough, try Manhattan Distance heuristics please" << endl;
                else
                    cout << "No solution!" << endl; //if Manhattan Distance heuristics can't solve it in 30s, I think there's no solution
                break;
            }

        }
        clock_t end = clock();
        cout << "Timeusage is " << ((end - start) * 1.0) / (((clock_t)1000) * 1000) << " seconds" << endl;
        cout << "To solve this problem the search algorithm expanded a total of " << ExpandednodesNum << " nodes." << endl;
        cout << "The maximum number of nodes in the queue at any one time was " << MaxqueuelementNum << "." << endl;
    }
    return 0;
}
