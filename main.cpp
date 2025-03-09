#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
using namespace std;
template<typename T>
bool tcmp(T v1,T v2,T v3)
{
    return (v1==v2)&&(v2==v3);
}
class Board
{
public:
    array<array<char,3>,3> board{};
    char currentTurn='X';
    unsigned freeFields{9};
    Board()
    {
        for_each(board.begin(),board.end(),[](array<char,3>& t){for_each(t.begin(),t.end(),[](char& t){t='.';});});
    }
    char& getField(unsigned x,unsigned y)
    {
        return board[x][y];
    }
    char& getField(unsigned field)
    {
        unsigned x=field%3;
        unsigned y=field/3;
        return board[x][y];
    }
    constexpr char readField(unsigned field) const
    {
        unsigned x=field%3;
        unsigned y=field/3;
        return board[x][y];
    }
    void printBoard()
    {
        cout<<" ----- \n|"<<getField(0,0)<<'|'<<getField(1,0)<<'|'<<getField(2,0)<<"|\n";
        cout<<" ----- \n|"<<getField(0,1)<<'|'<<getField(1,1)<<'|'<<getField(2,1)<<"|\n";
        cout<<" ----- \n|"<<getField(0,2)<<'|'<<getField(1,2)<<'|'<<getField(2,2)<<"|\n ----- \n";
    }
    void advanceTurn()
    {
        currentTurn=currentTurn^'X'^'O';
    }
    bool setField(unsigned field)
    {
        unsigned x=field%3;
        unsigned y=field/3;
        if(getField(x,y)!='.'||field>=9)return true;
        getField(x,y)=currentTurn;
        freeFields--;
        advanceTurn();
        return false;
    }
    char victoryCheck()
    {
        if(tcmp(getField(0,0),getField(1,0),getField(2,0)))if(getField(0,0)!='.')return getField(0,0);
        if(tcmp(getField(0,1),getField(1,1),getField(2,1)))if(getField(0,1)!='.')return getField(0,1);
        if(tcmp(getField(0,2),getField(1,2),getField(2,2)))if(getField(0,2)!='.')return getField(0,2);

        if(tcmp(getField(0,0),getField(0,1),getField(0,2)))if(getField(0,0)!='.')return getField(0,0);
        if(tcmp(getField(1,0),getField(1,1),getField(1,2)))if(getField(1,0)!='.')return getField(1,0);
        if(tcmp(getField(2,0),getField(2,1),getField(2,2)))if(getField(2,0)!='.')return getField(2,0);

        if(tcmp(getField(0,0),getField(1,1),getField(2,2)))if(getField(0,0)!='.')return getField(0,0);
        if(tcmp(getField(0,2),getField(1,1),getField(2,0)))if(getField(0,2)!='.')return getField(0,2);
        return '.';
    }
    Board& operator=(Board& b)
    {
        for(unsigned i=0;i<9;i++)
        {
            getField(i)=b.getField(i);
        }
        currentTurn=b.currentTurn;
        freeFields=b.freeFields;
        return *this;
    }
    Board& operator=(Board&& b)
    {
        board=b.board;
        currentTurn=b.currentTurn;
        freeFields=b.freeFields;
        return *this;
    }
    constexpr Board(const Board& b)
    {
        for(unsigned i=0;i<9;i++)
        {
            getField(i)=b.readField(i);
            currentTurn=b.currentTurn;
            freeFields=b.freeFields;
        }
    }
};

class Bot
{
public:
    class State
    {
    public:
        State(Board& b):board{b}{}
        Board board;
        bool collapsed{0};
        bool initial{0};
        char result{'.'};
        vector<State> substates{};
        void collapseSubstates()
        {
            for(unsigned i=0;i<9;i++)
            {
                if(board.getField(i)!='.'&&board.getField(i)!='X'&&board.getField(i)!='O')cout<<"ERROR: CORRUPT BOARD!\n";
                if(board.getField(i)=='.');
                {
                    substates.push_back(State(board));
                    if(substates.back().board.setField(i))
                    {
                        substates.pop_back();
                        continue;
                    }
                    //substates.back().board.advanceTurn();
                    //cout<<"substate test: \n";
                    //substates.back().board.printBoard();
                    if(substates.back().board.victoryCheck()=='.'&&substates.back().board.freeFields!=0)substates.back().collapseSubstates();
                    else
                    {
                        substates.back().result=substates.back().board.victoryCheck();
                    }
                }
            }
            char currentTurn=board.currentTurn;
            if(find_if(substates.begin(),substates.end(),[&currentTurn](State& t){return t.result==currentTurn;})!=substates.end())  result=currentTurn;
            else if(find_if(substates.begin(),substates.end(),[](State& t){return t.result=='.';})!=substates.end())  result='.';
            else result=currentTurn^'X'^'O';
            collapsed=1;
            if(initial==1)return;
            substates.erase(substates.begin(),substates.end());
            substates.shrink_to_fit();
        }
    };
    void run(Board& board)
    {
        State state(board);
        state.initial=1;
        state.collapseSubstates();
        
        unsigned first_win=9;
        unsigned first_draw=9;
        for(unsigned i=0;i<state.substates.size();i++)
        {
            if(state.substates[i].result==board.currentTurn)
            {
                first_win=min(first_win,i);
            }
            else if(state.substates[i].result=='.')
            {
                first_draw=min(first_draw,i);
            }
        }
        if(first_win!=9)board=state.substates[first_win].board;
        else if(first_draw!=9)board=state.substates[first_draw].board;
        else board=state.substates[0].board;

        if(first_win!=9)cout<<"winwards\n";
        else if(first_draw!=9)cout<<"drawwards\n";
        else cout<<"losswards\n";
    }
}bot;

int main()
{
    Board mainBoard;
    while(true)
    {
        mainBoard.printBoard();
        cout<<"Current Turn: "<<mainBoard.currentTurn<<'\n';
        unsigned field;
        cin>>field;
        while(mainBoard.getField(field-1)!='.')cin>>field;
        mainBoard.setField(field-1);
        //bot.run(mainBoard);
        if(mainBoard.victoryCheck()!='.')
        {
            cout<<mainBoard.victoryCheck()<<" WINS!\n";
            break;
        }
        if(mainBoard.freeFields==0)
        {
            cout<<"DRAW!\n";
            break;
        }

        bot.run(mainBoard);
        if(mainBoard.victoryCheck()!='.')
        {
            cout<<mainBoard.victoryCheck()<<" WINS!\n";
            break;
        }
        if(mainBoard.freeFields==0)
        {
            cout<<"DRAW!\n";
            break;
        }
    }
    mainBoard.printBoard();
}