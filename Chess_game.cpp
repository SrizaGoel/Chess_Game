#include<iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>
using namespace std;
//ChessGame
class Piece
{
protected:
    string colour;
public:
    Piece(const string &c)
    {
        colour=c;
    }
    virtual bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)=0;
    virtual string get_symbol()=0;

    string get_colour()
    {
        return colour;
    }
    virtual ~Piece(){}
};

class Empty_Place:public Piece
{
public:
    Empty_Place():Piece(""){}
    string get_symbol()
    {
        return ". ";
    }
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        return false;
    }
};

//Pawn class

class Pawn:public Piece
{
public:
    Pawn(const string&c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BP":"WP";
    }
    // Check pawn movement: forward or diagonal capture

    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int direction=(colour=="white"?1:-1); //black moves -1(comes down) and white moves +1(moves up)
        if(fromcol==tocol && torow-fromrow==direction && board_piece[torow][tocol]->get_colour()=="")
        {
            return true;
        }
        //diagonal move
        if(abs(tocol-fromcol)==1 && torow - fromrow == direction && board_piece[torow][tocol]->get_colour()!="" && board_piece[torow][tocol]->get_colour()!=colour)
        {
            return true;
        }
        return false;

    }
};

//Bishop Class

class Bishop:public Piece
{
public:
    Bishop(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BB":"WB";
    }
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        if(abs(fromrow-torow)!=abs(fromcol-tocol))
        {
            return false;
        }

        // Determine direction of movement

        int row_dir=(-fromrow+torow)/(abs(fromrow-torow));
        int col_dir=(-fromcol+tocol)/(abs(fromcol-tocol));
        int move_r=row_dir+fromrow;
        int move_c=col_dir+fromcol;
        while(move_r!=torow && move_c!=tocol)
        {
            if(board_piece[move_r][move_c]->get_colour()!="")
                return false;
            move_r+=row_dir;
            move_c+=col_dir;
        }
        if(board_piece[torow][tocol]!=nullptr && board_piece[torow][tocol]->get_colour()== colour)
        {
            return false;
        }
        return true;
    }
};
//Knight Class

class Knight:public Piece
{
public:
    Knight(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BN":"WN";
    }
    // Knight moves in L-shape

    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int r=abs(torow-fromrow);
        int c=abs(tocol-fromcol);
        if ((r==1 && c==2 )||(r==2 && c==1))
        {
            if(board_piece[torow][tocol]==nullptr || board_piece[torow][tocol]->get_colour()!=colour)
            {
                return true;
            }
        }
        return false;
    }
};

//Rook Class

class Rook : public Piece
{
public:
    Rook(const string& c):Piece(c){}
    string get_symbol()
    {
        return(colour=="black")?"BR":"WR";
    }

    // Rook moves straight in rows or columns
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>& board_piece)
    {
        if (fromrow!=torow && fromcol!=tocol)
        {
            return false; // Not moving straight
        }

        int row_step=(torow-fromrow)==0?0:(torow - fromrow)/abs(torow - fromrow);
        int col_step=(tocol-fromcol)==0?0:(tocol - fromcol)/abs(tocol - fromcol);
        int r=fromrow + row_step;
        int c=fromcol + col_step;
        while (r != torow || c != tocol)
        {

            if (board_piece[r][c]->get_colour() != "")
            {
                return false;
            }
            r += row_step;
            c += col_step;
        }
        // Final destination check
        if (board_piece[torow][tocol]->get_colour() == "" || board_piece[torow][tocol]->get_colour() != colour)
        {
            return true;
        }

        return false;
    }

};

//Queen Class

class Queen:public Piece
{
    Rook rook;
    Bishop bishop;

public:
    Queen(const string &c):Piece(c),bishop(c),rook(c){}
    string get_symbol()
    {
        return (colour=="black")?"BQ":"WQ";
    }

    // Queen = Bishop + Rook movements
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        if(bishop.move_validity(fromrow,fromcol,torow,tocol,board_piece) || rook.move_validity(fromrow,fromcol,torow,tocol,board_piece))
        {
            return true;
        }
        return false;

    }
};

//King Class

class King:public Piece
{
public:
    King(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BK":"WK";
    }

     // King moves one square in any direction
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int row_move=abs(torow-fromrow);
        int col_move=abs(tocol-fromcol);
        if((row_move==0 && col_move==1)||(row_move==1 && col_move==0)||(row_move==1 && col_move==1))
        {
            if(board_piece[torow][tocol] && board_piece[torow][tocol]->get_colour()!=colour)
            {
                return true;
            }
            if(board_piece[torow][tocol]==nullptr)
            {
                return true;
            }
        }
        return false;
    }
};

//ChessBoard create

class ChessBoard
{
    vector<vector<Piece*>>board;
public:

    ChessBoard()
    {
        // Initialize board with nullptr
        for (int i=0;i<8;i++)
        {
            vector<Piece*>row;
            for(int j=0;j<8;j++)
            {
                row.push_back(nullptr);
            }
            board.push_back(row);
        }
        setupBoard();
    }

    ~ChessBoard()
    {
        // Cleanup memory
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; ++j)
            {
                delete board[i][j];
            }
        }
    }

    // Setup all pieces on initial chess board
    void setupBoard()
    {
        //Pawn
        for(int i=0;i<8;i++)
        {
            board[1][i]=new Pawn("white");
            board[6][i]=new Pawn("black");
        }
        //white
        board[0][0]=new Rook("white");
        board[0][7]=new Rook("white");
        board[0][1]=new Knight("white");
        board[0][6]=new Knight("white");
        board[0][2]=new Bishop("white");
        board[0][5]=new Bishop("white");
        board[0][3]=new Queen("white");
        board[0][4]=new King("white");

        //black
        board[7][0]=new Rook("black");
        board[7][7]=new Rook("black");
        board[7][1]=new Knight("black");
        board[7][6]=new Knight("black");
        board[7][2]=new Bishop("black");
        board[7][5]=new Bishop("black");
        board[7][3]=new Queen("black");
        board[7][4]=new King("black");

        // Empty Spaces
        for(int i=2;i<6;i++)
        {
            for(int j=0;j<8;j++)
            {
                board[i][j]=new Empty_Place();
            }
        }
    }

    void printBoard()
    {
        for (int i = 7; i >= 0; --i)
        {
            cout << i + 1 << "  ";
            for (int j = 0; j < 8; ++j)
            {
                cout << board[i][j]->get_symbol()<< " ";
            }
            cout << " " << endl;
        }
        cout << "   1  2  3  4  5  6  7  8\n";
    }
    bool movement(int fromrow,int fromcol,int torow,int tocol,const string&current_player)
    {
        Piece*from=board[fromrow][fromcol];
        Piece*to=board[torow][tocol];
        if(from==nullptr || (from->get_colour()!=current_player))
        {
            return false;
        }
        if(to && (to->get_colour()==current_player))
        {
            return false;
        }
        if(!from->move_validity(fromrow,fromcol,torow,tocol,board))
        {
            return false;
        }
        board[torow][tocol]=from;
        board[fromrow][fromcol]=new Empty_Place();
        return true;
    }
    bool is_Checkmate(const string&current_player)
    {
        int torow,tocol;
        string opponent_colour=(current_player=="black")?"white":"black";
        string opponent_king=(opponent_colour=="white")?"WK":"BK";
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                if(this->board[i][j]->get_symbol()==opponent_king)
                {
                    torow=i;
                    tocol=j;
                    break;
                }
            }
        }

        bool in_check = false;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Piece* piece = this->board[i][j];
                if (piece != nullptr && piece->get_colour() == current_player)
                {
                    if (piece->move_validity(i, j, torow, tocol, board))
                    {
                        in_check = true;
                        break;
                    }
                }
            }
        }
        if (!in_check)
        {
            return false; // Not in check, so not checkmate
        }
        return true;
    }
    bool is_King_Alive(const string &colour)
    {
        string symbol=(colour=="black")?"BK":"WK";
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                if(board[i][j]->get_symbol()==symbol)
                {
                    return true;
                }
            }
        }
        cout<<endl<<symbol<<" captured ";
        string winner=(symbol=="WK")?"Black":"White";
        cout<<endl<<"Game over !";
        cout<<endl<<winner<<" Player wins the game !";
        return false;
    }
};
class Chess_Game
{
    ChessBoard chess;
    string currentplayer;
public:
    //starts game with white
    Chess_Game()
    {
        currentplayer="white";
    }
    void start()
    {
        while(true)
        {
            int from_row,to_row,from_col,to_col;
            chess.printBoard();
            cout<<endl<<"Enter "<<currentplayer<<"'s move (Which piece to move ?) : ";
            cin>>from_row>>from_col;
            cout<<endl<<"Enter the final position : ";
            cin>>to_row>>to_col;
            from_col--;from_row--;to_col--;to_row--;
            cout<<endl;
            if(from_row<0 || from_row>7 || from_col<0 || from_col>7 || to_row<0 || to_row>7 || to_col<0 || to_col>7)
            {
                cout<<endl<<"Invalid input (Coordinates out of bounds) !"<<endl<<endl;
                continue;
            }
            if(!chess.movement(from_row,from_col,to_row,to_col,currentplayer))
            {
                cout<<endl<<"Invalid move !"<<endl<<endl;
                continue;
            }
            if(chess.is_Checkmate(currentplayer))
            {
                cout<<endl<<" CHECKMATE ! "<<endl<<endl;
            }
            string opponent=(currentplayer=="white")?"black":"white";
            if (!chess.is_King_Alive(opponent))
            {
                break;
            }
            currentplayer=(currentplayer=="white")?"black":"white";
        }
    }
};
class Begin
{
public:
    
    void Chess_Intro()
    {
        cout<<"==========================================================================="<<endl;

        cout<<endl<<endl<<"                   WELCOME TO CHESS GAME          "<<endl<<endl<<endl;

        cout<<"==========================================================================="<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;

        cout<<" Keys to symbols used : "<<endl<<endl<<endl;
        cout<<" . = white space"<<endl;
        cout<<" Blank space = black space"<<endl;
        cout<<" WP = White pawn"<<endl;
        cout<<" WN = White Knight"<<endl;
        cout<<" WB = White Bishop"<<endl;
        cout<<" WR = White Rook"<<endl;
        cout<<" WQ = White Queen"<<endl;
        cout<<" WK = White King"<<endl;
        cout<<" BP = Black pawn"<<endl;
        cout<<" BN = Black Knight"<<endl;
        cout<<" BB = Black Bishop"<<endl;
        cout<<" BR = Black Rook"<<endl;
        cout<<" BQ = Black Queen"<<endl;
        cout<<" BK = Black King"<<endl<<endl;

        cout<<"Rule for move is : "<<endl;

        cout<<"Move by selecting row & column to another valid location using row & column. "<<endl<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;
    }
    
};
int main()
{
    Begin b;
    cout<<"Launching Chess Game..."<<endl<<endl;
    b.Chess_Intro();
    Chess_Game g;
    g.start();    
    return 0;
}